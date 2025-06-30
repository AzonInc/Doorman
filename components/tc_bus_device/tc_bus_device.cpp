#include "tc_bus_device.h"
#include "util.h"

#include "esphome.h"
#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#ifdef USE_ARDUINO
#include "Arduino.h"
#endif

#include <optional>
#include <utility>
#include <vector>
#include <cinttypes>

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"

using namespace esphome;

namespace esphome
{
    namespace tc_bus
    {
        void TCBusDeviceComponent::setup()
        {
            ESP_LOGCONFIG(TAG, "Running setup");

            if(this->tc_bus_->is_failed()) {
                std::string failed_msg = "TC:BUS failed to setup!";
                this->mark_failed(failed_msg.c_str());
                return;
            }
        
            if(!this->tc_bus_->is_ready()) {
                std::string not_ready_msg = "TC:BUS is not setup yet!";
                this->mark_failed(not_ready_msg.c_str());
                return;
            }

            uint32_t hash = fnv1_hash("tc_bus_device_" + this->internal_id_);
            this->pref_ = global_preferences->make_preference<TCBusDeviceSettings>(hash, true);

            TCBusDeviceSettings recovered;

            if (!this->pref_.load(&recovered))
            {
                recovered = {MODEL_NONE, 0, false};
            }

            this->serial_number_ = recovered.serial_number;
            this->force_long_door_opener_ = recovered.force_long_door_opener;
            this->set_model(recovered.model);

            #ifdef USE_NUMBER
                if (this->serial_number_number_ != nullptr)
                {
                    this->serial_number_number_->publish_state(this->serial_number_);
                }
            #endif

            #ifdef USE_SWITCH
                if (this->force_long_door_opener_switch_ != nullptr)
                {
                    this->force_long_door_opener_switch_->publish_state(this->force_long_door_opener_);
                }
            #endif

            #ifdef USE_BINARY_SENSOR
                // Reset Bunary Sensor Listeners
                for (auto &listener : listeners_)
                {
                    listener->turn_off(&listener->timer_);
                }
            #endif

            // Register remote listener
            this->tc_bus_->register_remote_listener(this);
        }

        void TCBusDeviceComponent::set_model(Model model)
        {
            this->model_ = model;
            this->model_data_ = getModelData(model);

            if(model != MODEL_NONE)
            {
                ESP_LOGD(TAG, "Reserve Memory Buffer");
                if(this->model_data_.memory_size > 0)
                {
                    this->memory_buffer_.reserve(this->model_data_.memory_size);
                }
            }

            #ifdef USE_SELECT
                if (this->model_select_ != nullptr)
                {
                    this->model_select_->publish_state(model_to_string(model));
                }
            #endif
        }

        void TCBusDeviceComponent::save_settings()
        {
            TCBusDeviceSettings settings{
                this->model_,
                this->serial_number_,
                this->force_long_door_opener_
            };

            if (!this->pref_.save(&settings))
            {
                ESP_LOGW(TAG, "Failed to save settings to flash memory.");
            }
        }

        void TCBusDeviceComponent::dump_config()
        {
            ESP_LOGCONFIG(TAG, "TC:BUS Device:");

            ESP_LOGCONFIG(TAG, "  Device Group: %d", this->device_group_);
        }

        void TCBusDeviceComponent::loop()
        {
            #ifdef USE_BINARY_SENSOR
                // Turn off binary sensor after ... milliseconds
                uint32_t now_millis = millis();
                for (auto &listener : listeners_)
                {
                    if (listener->timer_ && now_millis > listener->timer_)
                    {
                        listener->turn_off(&listener->timer_);
                    }
                }
            #endif
        }

        bool TCBusDeviceComponent::on_receive(tc_bus::TelegramData telegram_data, bool received)
        {
            if(telegram_data.type == TELEGRAM_TYPE_ACK && (read_memory_flow_ || identify_model_flow_))
            {
                return true;
            }

            if (received)
            {
                // From receiver
                if (read_memory_flow_)
                {
                    ESP_LOGD(TAG, "Received 4 memory blocks from %i to %i | Data: 0x%08X", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4, telegram_data.raw);
                        
                    // Save Data to memory Store
                    memory_buffer_.push_back((telegram_data.raw >> 24) & 0xFF);
                    memory_buffer_.push_back((telegram_data.raw >> 16) & 0xFF);
                    memory_buffer_.push_back((telegram_data.raw >> 8) & 0xFF);
                    memory_buffer_.push_back(telegram_data.raw & 0xFF);

                    // Next 4 Data Blocks
                    reading_memory_count_++;

                    // Memory reading complete
                    if (reading_memory_count_ == reading_memory_max_)
                    {
                        // Turn off
                        this->cancel_timeout("wait_for_memory_reading");

                        ESP_LOGD(TAG, "Reset read_memory_flow_");
                        read_memory_flow_ = false;

                        this->publish_settings();
                        this->read_memory_complete_callback_.call(memory_buffer_);
                    }
                    else
                    {
                        read_memory_block();
                    }

                    // Do not proceed
                    return true;
                }
                else if (identify_model_flow_)
                {
                    ESP_LOGD(TAG, "Received model identification | Data: %s", telegram_data.hex.c_str());

                    ESP_LOGD(TAG, "Reset identify_model_flow_");
                    identify_model_flow_ = false;

                    this->cancel_timeout("wait_for_identification_category_0");
                    this->cancel_timeout("wait_for_identification_category_1");
                    this->cancel_timeout("wait_for_identification_other");

                    ModelData device;
                    device.category = this->tc_bus_->selected_device_group_;
                    device.memory_size = 0;
                    
                    if (telegram_data.hex.substr(4, 1) == "D")
                    {
                        // New models

                        // FW Version
                        device.firmware_version = std::stoi(telegram_data.hex.substr(5, 3), nullptr, 16);
                        device.firmware_major = std::stoi(telegram_data.hex.substr(5, 1), nullptr, 16);
                        device.firmware_minor = std::stoi(telegram_data.hex.substr(6, 1), nullptr, 16);
                        device.firmware_patch = std::stoi(telegram_data.hex.substr(7, 1), nullptr, 16);

                        // HW Version
                        device.hardware_version = std::stoi(telegram_data.hex.substr(0, 1));
                        device.model = identifier_string_to_model(device.category, telegram_data.hex.substr(1, 3), device.hardware_version, device.firmware_version);
                    }
                    else
                    {
                        if(device.category == 0 || device.category == 1)
                        {
                            // Old indoor station models
                            switch(telegram_data.raw)
                            {
                                // TTC-XX
                                case 0x08000040:
                                    device.model = MODEL_TTCXX;
                                    break;

                                // TTS-XX
                                case 0x02010040:
                                    device.model = MODEL_TTSXX;
                                    break;

                                // ISH 1030
                                case 0x08000048:
                                case 0x08080048:
                                    device.model = MODEL_ISH1030;
                                    break;

                                default:
                                    break;
                            }
                        }
                        else if(device.category == 4)
                        {
                            // Old controller models
                            switch(telegram_data.raw)
                            {
                                case 0x877F5804:
                                    device.model = CONTROLLER_MODEL_BVS20;
                                    break;

                                default:
                                    break;
                            }
                        }
                        else
                        {
                            // Old models of other groups
                            // Not implemented
                        }
                    }

                    if (device.model != MODEL_NONE)
                    {
                        // Add missing information
                        device.memory_size = getModelData(device.model).memory_size;

                        ESP_LOGD(TAG, "Identified Hardware: %s (v%i) | Firmware: %i.%i.%i",
                                model_to_string(device.model),
                                device.hardware_version,
                                device.firmware_major,
                                device.firmware_minor,
                                device.firmware_patch);

                        
                        // Update Model
                        if (device.model != this->model_)
                        {
                            this->set_model(device.model);
                            this->save_settings();
                        }
                        this->identify_complete_callback_.call(device);
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Unable to identify Hardware! Unknown model. Data received: %s", telegram_data.hex.c_str());
                        this->identify_unknown_callback_.call();
                    }

                    // Do not proceed
                    return true;
                }
                else
                {
                    // Fire Callback
                    this->received_telegram_callback_.call(telegram_data);

                    #ifdef USE_BINARY_SENSOR
                        // Fire Binary Sensors
                        for (auto &listener : listeners_)
                        {
                            // Listener Telegram lambda or telegram property when not available
                            uint32_t listener_telegram = listener->telegram_.has_value() ? listener->telegram_.value() : 0;
                            if (listener->telegram_lambda_.has_value())
                            {
                                auto optional_value = (*listener->telegram_lambda_)();
                                if (optional_value.has_value())
                                {
                                    listener_telegram = optional_value.value();
                                }
                            }

                            // Listener Serial Number
                            uint32_t listener_serial_number = this->serial_number_;

                            // Listener Address lambda or address property when not available
                            uint8_t listener_address = listener->address_.has_value() ? listener->address_.value() : 0;
                            if (listener->address_lambda_.has_value())
                            {
                                auto optional_value = (*listener->address_lambda_)();
                                if (optional_value.has_value())
                                {
                                    listener_address = optional_value.value();
                                }
                            }

                            // Listener payload lambda or payload property when not available
                            uint32_t listener_payload = listener->payload_.has_value() ? listener->payload_.value() : 0;
                            if (listener->payload_lambda_.has_value())
                            {
                                auto optional_value = (*listener->payload_lambda_)();
                                if (optional_value.has_value())
                                {
                                    listener_payload = optional_value.value();
                                }
                            }

                            // Listener Telegram Type
                            TelegramType listener_type = listener->type_.has_value() ? listener->type_.value() : TELEGRAM_TYPE_UNKNOWN;

                            bool allow_publish = false;

                            // Check if listener matches the telegram
                            if (listener_telegram != 0)
                            {
                                if (telegram_data.raw == listener_telegram)
                                {
                                    allow_publish = true;
                                }
                            }
                            else if (telegram_data.type == listener_type && (telegram_data.address == listener_address || listener_address == 255) && (telegram_data.payload == listener_payload || listener_payload == 255))
                            {
                                if (listener_serial_number != 0)
                                {
                                    if (telegram_data.serial_number == listener_serial_number || listener_serial_number == 255)
                                    {
                                        allow_publish = true;
                                    }
                                }
                                else
                                {
                                    allow_publish = true; // Accept any serial number
                                }
                            }

                            // Trigger listener binary sensor if match found
                            if (allow_publish)
                            {
                                listener->turn_on(&listener->timer_, listener->auto_off_);
                            }
                        }
                    #endif
                }
            }

            return true;
        }

        void TCBusDeviceComponent::publish_settings()
        {
            ESP_LOGD(TAG, "Handset volume (Door Call): %i", get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
            ESP_LOGD(TAG, "Handset volume (Internal Call): %i", get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
            ESP_LOGD(TAG, "Ringtone volume: %i", get_setting(SETTING_VOLUME_RINGTONE));

            ESP_LOGD(TAG, "Entrance Door Call Ringtone: %i", get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL));
            ESP_LOGD(TAG, "Second Entrance Door Call Ringtone: %i", get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL));
            ESP_LOGD(TAG, "Floor Call Ringtone: %i", get_setting(SETTING_RINGTONE_FLOOR_CALL));
            ESP_LOGD(TAG, "Internal Call Ringtone: %i", get_setting(SETTING_RINGTONE_INTERNAL_CALL));

            #ifdef USE_SELECT
                if (this->ringtone_entrance_door_call_select_)
                {
                    this->ringtone_entrance_door_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL)));
                }
                if (this->ringtone_second_entrance_door_call_select_)
                {
                    this->ringtone_second_entrance_door_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL)));
                }
                if (this->ringtone_floor_call_select_)
                {
                    this->ringtone_floor_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_FLOOR_CALL)));
                }
                if (this->ringtone_internal_call_select_)
                {
                    this->ringtone_internal_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_INTERNAL_CALL)));
                }
            #endif

            #ifdef USE_NUMBER
                if (this->volume_handset_door_call_number_)
                {
                    this->volume_handset_door_call_number_->publish_state(get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
                }
                if (this->volume_handset_internal_call_number_)
                {
                    this->volume_handset_internal_call_number_->publish_state(get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
                }
                if (this->volume_ringtone_number_)
                {
                    this->volume_ringtone_number_->publish_state(get_setting(SETTING_VOLUME_RINGTONE));
                }
            #endif
        }

        #ifdef USE_BINARY_SENSOR
            void TCBusDeviceComponent::register_listener(TCBusDeviceListener *listener)
            {
                this->listeners_.push_back(listener);
            }
        #endif

        void TCBusDeviceComponent::send_telegram(uint32_t telegram, uint32_t wait_duration)
        {
            this->tc_bus_->send_telegram(telegram, wait_duration);
        }

        void TCBusDeviceComponent::send_telegram(uint32_t telegram, bool is_long, uint32_t wait_duration)
        {
            this->tc_bus_->send_telegram(telegram, is_long, wait_duration);
        }

        void TCBusDeviceComponent::send_telegram(TelegramType type, uint8_t address, uint32_t payload, uint32_t wait_duration)
        {
            // Use 32-bit protocol
            if(type == TELEGRAM_TYPE_OPEN_DOOR && this->force_long_door_opener_)
            {
                ESP_LOGV(TAG, "Detected 32-bit door protocol override, change telegram telegram to OPEN_DOOR_LONG.");
                type = TELEGRAM_TYPE_OPEN_DOOR_LONG;
            }

            this->tc_bus_->send_telegram(type, address, payload, this->serial_number_, wait_duration);
        }

        void TCBusDeviceComponent::send_telegram(TelegramData telegram_data, uint32_t wait_duration)
        {
            this->tc_bus_->send_telegram(telegram_data, wait_duration);
        }

        void TCBusDeviceComponent::request_version()
        {
            if(this->serial_number_ == 0)
            {
                ESP_LOGE(TAG, "Device model cannot be identified without a serial number!");
                return;
            }

            this->cancel_timeout("wait_for_identification_category_0");
            this->cancel_timeout("wait_for_identification_category_1");
            this->cancel_timeout("wait_for_identification_other");

            ESP_LOGD(TAG, "Set identify_model_flow_");
            this->identify_model_flow_ = true;

            if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
            {
                // Indoor Stations

                // First try with category 0
                ESP_LOGD(TAG, "Identifying device model (Group %i) using serial number: %i...", 0, this->serial_number_);
                send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, 0, 400); // group 0
                send_telegram(TELEGRAM_TYPE_REQUEST_VERSION, 0, 0, 400);

                this->set_timeout("wait_for_identification_category_0", 1000, [this]()
                {
                    // Didn't receive identify result of category 0
                    // Second try with category 1
                    ESP_LOGD(TAG, "Identifying device model (Group %i) using serial number: %i...", 1, this->serial_number_);
                    send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, 1, 400); // group 1
                    send_telegram(TELEGRAM_TYPE_REQUEST_VERSION, 0, 0, 400);

                    this->set_timeout("wait_for_identification_category_1", 1000, [this]() {
                        // Didn't receive identify result of category 1
                        // Failed
                        this->identify_model_flow_ = false;
                        ESP_LOGD(TAG, "Reset wait_for_identification_telegram_");
                        ESP_LOGD(TAG, "Reset identify_model_flow_");

                        this->identify_timeout_callback_.call();
                        ESP_LOGE(TAG, "Identification response not received in time. The device model may not support identification.");
                    });
                });
            }
            else
            {
                // Other Devices

                // Use device group if not 0 and 1
                ESP_LOGD(TAG, "Identifying device model (Category %i) using serial number: %i...", (uint8_t)this->device_group_, this->serial_number_);
                send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, (uint8_t)this->device_group_, 400);
                send_telegram(TELEGRAM_TYPE_REQUEST_VERSION, 0, 0, 400);

                this->set_timeout("wait_for_identification_other", 1000, [this]() {
                    // Failed
                    this->identify_model_flow_ = false;
                    ESP_LOGE(TAG, "Identification response not received in time. The device model may not support identification.");
                });
            }
        }

        void TCBusDeviceComponent::read_memory()
        {
            this->cancel_timeout("wait_for_memory_reading");
            ESP_LOGD(TAG, "Reset read_memory_flow_");
            read_memory_flow_ = false;

            if (this->serial_number_ == 0)
            {
                ESP_LOGE(TAG, "Unable to read device memory without a serial number!");
                return;
            }

            if (this->model_ == MODEL_NONE)
            {
                ESP_LOGE(TAG, "Unable to read device memory without a model!");
                return;
            }

            if (this->model_data_.memory_size == 0)
            {
                ESP_LOGE(TAG, "This model %s (Serial: %i) does not support reading memory!", model_to_string(this->model_), this->serial_number_);
                // Call timeout callback for unsupported models
                this->read_memory_timeout_callback_.call();
                return;
            }
            else
            {
                ESP_LOGD(TAG, "Reading EEPROM for model %s (Serial: %i)...", model_to_string(this->model_), this->serial_number_);
            }

            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.category);
            send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0, 0);

            memory_buffer_.clear();
  
            ESP_LOGD(TAG, "Set read_memory_flow_");
            read_memory_flow_ = true;
            reading_memory_count_ = 0;
            reading_memory_max_ = (this->model_data_.memory_size / 4);

            this->set_timeout("wait_for_memory_reading", reading_memory_max_ * 600, [this]()
            {
                memory_buffer_.clear();
                ESP_LOGD(TAG, "Reset read_memory_flow_");
                read_memory_flow_ = false;
                reading_memory_count_ = 0;
                reading_memory_max_ = 0;

                this->read_memory_timeout_callback_.call();
                ESP_LOGE(TAG, "Memory block not received in time. Reading canceled!");
            });

            read_memory_block();
        }

        void TCBusDeviceComponent::read_memory_block()
        {
            ESP_LOGD(TAG, "Read 4 memory blocks, from %i to %i.", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);
            send_telegram(TELEGRAM_TYPE_READ_MEMORY_BLOCK, reading_memory_count_, 0, 300);
        }

        bool TCBusDeviceComponent::supports_setting(SettingType type)
        {
            if (memory_buffer_.size() == 0)
            {
                return false;
            }

            if (this->model_ == MODEL_NONE)
            {
                return false;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, this->model_);

            if (cellData.index != 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        uint8_t TCBusDeviceComponent::get_setting(SettingType type)
        {
            if (memory_buffer_.size() == 0)
            {
                return 0;
            }

            if (this->model_ == MODEL_NONE)
            {
                ESP_LOGV(TAG, "Model is not specified (NONE). Returning 0.");
                return 0;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, this->model_);

            if (cellData.index != 0)
            {
                return cellData.left_nibble ? ((memory_buffer_[cellData.index] >> 4) & 0xF) : (memory_buffer_[cellData.index] & 0xF);
            }
            else
            {
                ESP_LOGV(TAG, "The setting '%s' is not available for model '%s'.", setting_type_to_string(type), model_to_string(this->model_));
                return 0;
            }
        }

        bool TCBusDeviceComponent::update_setting(SettingType type, uint8_t new_value)
        {
            if (this->memory_buffer_.size() == 0)
            {
                ESP_LOGE(TAG, "Memory buffer is empty. Please read memory before proceeding!");
                return false;
            }

            if (this->serial_number_ == 0)
            {
                ESP_LOGE(TAG, "Unable to update device setting without a serial number!");
                return false;
            }

            if (this->model_ == MODEL_NONE)
            {
                ESP_LOGE(TAG, "Unable to update device setting without a model!");
                return false;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, this->model_);
            if (cellData.index == 0)
            {
                ESP_LOGE(TAG, "Cannot write because setting %s is not available for model %s!", setting_type_to_string(type), model_to_string(this->model_));
                return false;
            }

            ESP_LOGD(TAG, "Writing setting %s (%X) to EEPROM of %s (%i)...", setting_type_to_string(type), new_value, model_to_string(this->model_), this->serial_number_);
            // Apply new nibble and keep other nibble
            uint8_t saved_nibble = (cellData.left_nibble ? memory_buffer_[cellData.index] : (memory_buffer_[cellData.index] >> 4)) & 0xF;
            memory_buffer_[cellData.index] = cellData.left_nibble ? ((new_value << 4) | (saved_nibble & 0xF)) : ((saved_nibble << 4) | (new_value & 0xF));

            // Prepare Transmission
            // Select device category
            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.category);

            // Select memory page %i of serial number %i
            send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0, 0);

            // Transfer new settings value to memory
            uint16_t new_values = (memory_buffer_[cellData.index] << 8) | memory_buffer_[cellData.index + 1];
            send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, cellData.index, new_values);

            // Reset
            send_telegram(TELEGRAM_TYPE_RESET);

            return true;
        }

        bool TCBusDeviceComponent::write_memory()
        {
            if (this->memory_buffer_.size() == 0)
            {
                ESP_LOGE(TAG, "Memory buffer is empty! Please read memory first before proceeding.");
                return false;
            }

            if (this->serial_number_ == 0)
            {
                ESP_LOGE(TAG, "Unable to write memory without a serial number!");
                return false;
            }

            if (this->model_ == MODEL_NONE)
            {
                ESP_LOGE(TAG, "Unable to write memory without a model!");
                return false;
            }

            ESP_LOGD(TAG, "Write memory buffer to EEPROM of %s (%i)...", model_to_string(this->model_), this->serial_number_);

            // Prepare Transmission
            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.category);

            send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0, 0);

            // Transmit Memory
            for (size_t address = 0; address < memory_buffer_.size(); address += 2)
            {
                uint16_t new_value = (memory_buffer_[address] << 8) | memory_buffer_[address + 1];
                send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, address, new_value);
            }

            // Reset
            send_telegram(TELEGRAM_TYPE_RESET);

            return true;
        }

    } // namespace tc_bus
} // namespace esphome