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

#include <optional>
#include <utility>
#include <vector>
#include <cinttypes>

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"

using namespace esphome;

namespace esphome::tc_bus
{
    void TCBusDeviceComponent::setup()
    {
        ESP_LOGCONFIG(TAG, "Running setup");

        if(this->tc_bus_->is_failed()) {
            this->mark_failed(LOG_STR("TC:BUS failed to setup!"));
            return;
        }
    
        if(!this->tc_bus_->is_ready()) {
            this->mark_failed(LOG_STR("TC:BUS is not setup yet!"));
            return;
        }

        uint32_t hash = fnv1_hash("tc_bus_device_" + this->internal_id_);
        this->pref_ = global_preferences->make_preference<TCBusDeviceSettings>(hash, true);

        TCBusDeviceSettings recovered{};

        if (!this->pref_.load(&recovered))
        {
            // Generate serial number for virtual device based on mac address

            // TODO: count internal devices and increment serial number for each device
            // to avoid conflicts when multiple virtual devices are used

            if(this->virtual_device_)
            {
                uint8_t mac[6];
                get_mac_address_raw(mac);
                uint32_t mac_addr = (mac[3] << 16) | (mac[4] << 8) | (mac[5] & 0xF0) >> 4;

                recovered.serial_number = mac_addr;
            }
        }

        this->call_state_ = CallState::IDLE;

        this->set_serial_number(recovered.serial_number, false);
        this->set_model(recovered.model, false);

        this->address_ = recovered.address;
        this->address_divider_ = recovered.address_divider;
        this->door_readiness_duration_ = recovered.door_readiness_duration;
        this->call_time_duration_ = recovered.call_time_duration;
        this->door_opener_duration_ = recovered.door_opener_duration;
        this->force_long_door_opener_protocol_ = recovered.force_long_door_opener_protocol;
        this->auto_answer_call_ = recovered.auto_answer_call;

        if(this->virtual_device_)
        {
            this->memory_buffer_.resize(128, 0); 

            if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
            {
                // not compatible with every indoor station, data will be different
                // this data is from ISH3030

                // Address 0x00
                this->memory_buffer_[0x00] = 0x20; this->memory_buffer_[0x01] = 0x00;
                this->memory_buffer_[0x02] = 0x00; this->memory_buffer_[0x03] = 0x10;
                // Address 0x04
                this->memory_buffer_[0x04] = 0x00; this->memory_buffer_[0x05] = 0x00;
                this->memory_buffer_[0x06] = 0x1F; this->memory_buffer_[0x07] = 0x42;
                // Address 0x08
                this->memory_buffer_[0x08] = 0x40; this->memory_buffer_[0x09] = 0x1F;
                this->memory_buffer_[0x0A] = 0x42; this->memory_buffer_[0x0B] = 0x40;
                // Address 0x0C
                this->memory_buffer_[0x0C] = 0x10; this->memory_buffer_[0x0D] = 0x00;
                this->memory_buffer_[0x0E] = 0x00; this->memory_buffer_[0x0F] = 0x00;
                // Address 0x10
                this->memory_buffer_[0x10] = 0x00; this->memory_buffer_[0x11] = 0x00;
                this->memory_buffer_[0x12] = 0x00; this->memory_buffer_[0x13] = 0x00;
                // Address 0x14
                this->memory_buffer_[0x14] = 0x00; this->memory_buffer_[0x15] = 0x01;
                this->memory_buffer_[0x16] = 0x00; this->memory_buffer_[0x17] = 0x49;
                // Address 0x18
                this->memory_buffer_[0x18] = 0x00; this->memory_buffer_[0x19] = 0x05;
                this->memory_buffer_[0x1A] = 0x00; this->memory_buffer_[0x1B] = 0x05;
                // Address 0x1C
                this->memory_buffer_[0x1C] = 0xFF; this->memory_buffer_[0x1D] = 0xFF;
                this->memory_buffer_[0x1E] = 0xFF; this->memory_buffer_[0x1F] = 0xFF;
            }
            else
            {
                // TODO: Fill memory with default values for other devices
                ESP_LOGW(TAG, "TODO: Fill memory with default values for other devices");
            }
        }


        #ifdef USE_SWITCH
        if (this->force_long_door_opener_protocol_switch_ != nullptr)
        {
            this->force_long_door_opener_protocol_switch_->publish_state(this->force_long_door_opener_protocol_);
        }
        if (this->auto_answer_call_switch_ != nullptr)
        {
            this->auto_answer_call_switch_->publish_state(this->auto_answer_call_);
        }
        #endif

        #ifdef USE_NUMBER
        if (this->address_number_ != nullptr)
        {
            this->address_number_->publish_state(this->address_);
        }
        if (this->address_divider_number_ != nullptr)
        {
            this->address_divider_number_->publish_state(this->address_divider_);
        }
        if (this->door_readiness_duration_number_ != nullptr)
        {
            this->door_readiness_duration_number_->publish_state(this->door_readiness_duration_);
        }
        if (this->call_time_duration_number_ != nullptr)
        {
            this->call_time_duration_number_->publish_state(this->call_time_duration_);
        }
        if (this->door_opener_duration_number_ != nullptr)
        {
            this->door_opener_duration_number_->publish_state(this->door_opener_duration_);
        }
        #endif

        #ifdef USE_BINARY_SENSOR
        // Reset Binary Sensor Listeners
        for (auto &listener : listeners_)
        {
            listener->turn_off(&listener->timer_);
        }
        #endif

        // Register remote listener
        if(this->virtual_device_)
        {
            // Higher priority for virtual devices to ensure they receive telegrams in time
            this->tc_bus_->register_remote_listener(this, 10);
        }
        else
        {
            this->tc_bus_->register_remote_listener(this);
        }

        // Schedule flows for physical devices only
        if(this->virtual_device_ == false && this->auto_configuration_ && this->serial_number_ != 0)
        {
            if(this->model_ != MODEL_NONE)
            {
                ESP_LOGD(TAG, "Schedule flow: Memory reading (setup)");
                read_memory();
            }
            else
            {
                ESP_LOGD(TAG, "Schedule flow: Model identification (setup)");
                identify_device();
            }
        }

        if(this->virtual_device_)
        {
            this->high_freq_.start();
        }
    }

    void TCBusDeviceComponent::set_serial_number(uint32_t serial_number, bool save)
    {
        if(serial_number > 0xFFFFF)
        {
            if(this->virtual_device_)
            {
                // TODO: generate unique serial number for each virtual device
                ESP_LOGW(TAG, "Invalid Serial Number, reset to device MAC.");

                uint8_t mac[6];
                get_mac_address_raw(mac);
                uint32_t mac_addr = (mac[3] << 16) | (mac[4] << 8) | (mac[5] & 0xF0) >> 4;

                serial_number = mac_addr;
            }
            else
            {
                ESP_LOGW(TAG, "Invalid Serial Number, reset to 0.");
                serial_number = 0;
            }
        }

        bool changed = serial_number != this->serial_number_;

        this->serial_number_ = serial_number;

        // Save to flash
        if(save)
        {
            this->save_preferences();
        }

        // Only for physical devices
        if(this->virtual_device_ == false && serial_number != 0)
        {
            // Schedule model identification flow
            if(this->auto_configuration_ && save && changed)
            {
                // Clear memory because device serial number changed
                ESP_LOGD(TAG, "Clear memory buffer");
                this->memory_buffer_.clear();

                ESP_LOGD(TAG, "Schedule flow: Model identification (changed serial number from 0)");

                // Wait for possible doorbell ringtone (setup mode)
                this->set_timeout(3000, [this]()
                {
                    identify_device();
                });
            }
        }

        // Update Entities
        #ifdef USE_NUMBER
        if (this->serial_number_number_ != nullptr)
        {
            this->serial_number_number_->publish_state(serial_number);
        }
        #endif
    }

    void TCBusDeviceComponent::set_model(Model model, bool save)
    {
        bool changed = model != this->model_;
        bool changed_from_none = this->model_ == MODEL_NONE && changed;

        this->model_ = model;
        this->model_data_ = getModelData(model);

        // Save to flash
        if(save)
        {
            this->save_preferences();
        }

        // Only for physical devices
        if(!this->virtual_device_)
        {
            // When model of physical device was changed and new model is not none
            if(model != MODEL_NONE)
            {
                // Reserve memory
                ESP_LOGD(TAG, "Reserve Memory Buffer");
                if(this->model_data_.memory_size > 0)
                {
                    this->memory_buffer_.reserve(this->model_data_.memory_size);
                }

                this->publish_settings();
            }

            // Schedule memory reading flow
            // if model was changed from 'none' to valid model
            // or memory buffer is empty
            if(model != MODEL_NONE && this->auto_configuration_ && save && (changed_from_none || this->memory_buffer_empty()))
            {
                ESP_LOGD(TAG, "Schedule flow: Memory reading (%s)", changed_from_none ? "changed model - from none" : "changed model - buffer empty");
                read_memory();
            }
        }

        // Update Entities
        #ifdef USE_SELECT
        if (this->model_select_ != nullptr)
        {
            this->model_select_->publish_state(model_to_string(model));
        }
        #endif
    }

    void TCBusDeviceComponent::save_preferences()
    {
        TCBusDeviceSettings settings{};
        settings.serial_number = this->serial_number_;
        settings.address = this->address_;
        settings.model = this->model_;

        settings.address_divider = this->address_divider_;
        settings.door_readiness_duration = this->door_readiness_duration_;
        settings.call_time_duration = this->call_time_duration_;
        settings.door_opener_duration = this->door_opener_duration_;
        settings.force_long_door_opener_protocol = this->force_long_door_opener_protocol_;
        settings.auto_answer_call = this->auto_answer_call_;

        if (!this->pref_.save(&settings))
        {
            ESP_LOGW(TAG, "Failed to save settings to flash memory.");
        }
    }

    void TCBusDeviceComponent::dump_config()
    {
        ESP_LOGCONFIG(TAG, this->virtual_device_ ? "Virtual TC:BUS Device:" : "TC:BUS Device:");
        ESP_LOGCONFIG(TAG, "  Group: %s", device_group_to_string(this->device_group_));
        ESP_LOGCONFIG(TAG, "  Model: %s", model_to_string(this->model_));
        ESP_LOGCONFIG(TAG, "  Serial Number: %i", this->serial_number_);

        if(this->virtual_device_ && this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
            ESP_LOGCONFIG(TAG, "  Door Readiness Duration: %i sec.", this->door_readiness_duration_ * 8);
            ESP_LOGCONFIG(TAG, "  Door Opener Duration: %i sec.", this->door_opener_duration_ * 8);
        }

        if(this->virtual_device_ && this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
        {
            ESP_LOGCONFIG(TAG, "  Address Divider: %i", this->address_divider_);
        }

        if(this->virtual_device_ && (this->device_group_ == DEVICE_GROUP_INDOOR_STATION || this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION))
        {
            ESP_LOGCONFIG(TAG, "  Call Time Duration: %i sec.", this->call_time_duration_ * 8);
        }

        if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
        {
            ESP_LOGCONFIG(TAG, "  Force long door opener protocol: %s", YESNO(this->force_long_door_opener_protocol_));
        }

        #ifdef USE_BUTTON
        ESP_LOGCONFIG(TAG, "  Buttons:");
        LOG_BUTTON("    ", "Identify", this->identify_device_button_);
        LOG_BUTTON("    ", "Read Memory", this->read_memory_button_);
        #endif

        #ifdef USE_SWITCH
        ESP_LOGCONFIG(TAG, "  Switches:");
        LOG_SWITCH("    ", "Force long door opener protocol", this->force_long_door_opener_protocol_switch_);
        LOG_SWITCH("    ", "Ringtone Mute", this->ringtone_mute_switch_);
        LOG_SWITCH("    ", "Auto answer call", this->auto_answer_call_switch_);
        #endif

        #ifdef USE_NUMBER
        ESP_LOGCONFIG(TAG, "  Number Inputs:");
        LOG_NUMBER("    ", "Serial Number", this->serial_number_number_);
        LOG_NUMBER("    ", "Address", this->address_number_);
        LOG_NUMBER("    ", "Volume Handset Door Call", this->volume_handset_door_call_number_);
        LOG_NUMBER("    ", "Volume Handset Internal Call", this->volume_handset_internal_call_number_);
        LOG_NUMBER("    ", "Volume Ringtone", this->volume_ringtone_number_);
        LOG_NUMBER("    ", "Address Divider", this->address_divider_number_);
        LOG_NUMBER("    ", "Door Readiness Duration", this->door_readiness_duration_number_);
        LOG_NUMBER("    ", "Call Time Duration", this->call_time_duration_number_);
        LOG_NUMBER("    ", "Door Opener Duration", this->door_opener_duration_number_);
        #endif

        #ifdef USE_SELECT
        ESP_LOGCONFIG(TAG, "  Select Inputs:");
        LOG_SELECT("    ", "Model", this->model_select_);
        LOG_SELECT("    ", "Ringtone Entrance Door Call", this->ringtone_entrance_door_call_select_);
        LOG_SELECT("    ", "Ringtone Second Entrance Door Call", this->ringtone_second_entrance_door_call_select_);
        LOG_SELECT("    ", "Ringtone Floor Call", this->ringtone_floor_call_select_);
        LOG_SELECT("    ", "Ringtone Internal Call", this->ringtone_internal_call_select_);
        #endif
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

        // Process flows for physical devices only
        if(!this->virtual_device_)
        {
            this->process_flow_queue();
        }
    }

    bool TCBusDeviceComponent::on_receive(tc_bus::TelegramData telegram_data, bool received)
    {
        if (!received || telegram_data.is_retransmission)
        {
            return false;
        }

        if (this->virtual_device_)
        {
            // Virtual Device

            // General
            if (telegram_data.type == TELEGRAM_TYPE_SEARCH_DEVICES)
            {
                if(this->tc_bus_->get_selected_device_group() == this->device_group_)
                {
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_FOUND_DEVICE, 0, 0, this->serial_number_);
                }
            }
            else if (telegram_data.type == TELEGRAM_TYPE_SELECT_DEVICE_GROUP)
            {
                if(telegram_data.payload == this->device_group_)
                {
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1); // workaround - remove later - or not
                }
                
                if(this->memory_mode_)
                {
                    this->memory_mode_ = false;
                    ESP_LOGW(TAG, "MEMORY MODE FALSE");
                }
            }
            else if(telegram_data.type == TELEGRAM_TYPE_SELECT_MEMORY_PAGE && telegram_data.serial_number == this->serial_number_)
            {
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                this->memory_mode_ = true;
                return true;
            }
            else if(telegram_data.type == TELEGRAM_TYPE_STOP_TALKING || telegram_data.type == TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL)
            {
                ESP_LOGD(TAG, "Stop talking received - disconnecting call");

                bool cancelled = this->reset_call();
                if(cancelled)
                {
                    this->call_ended_callback_.call(telegram_data);
                }
                return true;
            }
            else if(telegram_data.type == TELEGRAM_TYPE_READ_MEMORY_BLOCK && this->memory_mode_)
            {
                uint32_t block = this->memory_buffer_[telegram_data.address] << 24 | this->memory_buffer_[telegram_data.address + 1] << 16 | this->memory_buffer_[telegram_data.address + 2] << 8 | this->memory_buffer_[telegram_data.address + 3];
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, block);
                return true;
            }
            else if(telegram_data.type == TELEGRAM_TYPE_WRITE_MEMORY && this->memory_mode_)
            {
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);

                memory_buffer_[telegram_data.address/4] = (telegram_data.payload >> 8) & 0xFF;
                memory_buffer_[(telegram_data.address/4) + 1] = telegram_data.payload & 0xFF;

                return true;
            }
            
            // Device specific
            if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
            {
                if(telegram_data.type == TELEGRAM_TYPE_DOOR_CALL && telegram_data.serial_number == this->serial_number_)
                {
                    // 1. receive door call from outdoor station
                    // 2. send ACK STATUS

                    // 3. send start talking to outdoor station with answer_call()
                    // 4. receive acknowledge from outdoor station to initiate call

                    // Door call from outdoor station
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                    this->call_internal_ = false;
                    this->call_address_ = telegram_data.address;
                    this->call_state_ = CallState::IN_RINGING;
                    this->incoming_call_callback_.call(telegram_data);
                }
                else if(telegram_data.type == TELEGRAM_TYPE_INTERNAL_CALL && telegram_data.serial_number == this->serial_number_)
                {
                    // 1. receive internal call from indoor station (address 63)
                    // 2. send ACK STATUS

                    // 3. send start talking to indoor station with answer_call()
                    // 4. receive acknowledge from outdoor station to initiate call

                    // Internal call from another indoor station
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                    this->call_internal_ = true;
                    this->call_address_ = telegram_data.address;
                    this->call_state_ = CallState::IN_RINGING;
                    this->incoming_call_callback_.call(telegram_data);
                }
                else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
                {
                    this->call_state_ = CallState::LINE_BUSY;
                }
                else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING)
                {
                    // Outgoing internal call accepted by other indoor station
                    if(this->call_state_ == CallState::OUT_RINGING && this->call_address_ == telegram_data.serial_number)
                    {
                        // out: acknowledge talk
                        this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 3);
                        this->call_state_ = CallState::CONNECTED;

                        ESP_LOGD(TAG, "Connected to another indoor station");

                        // reset ack timeouts
                        this->cancel_timeout("wait_for_call_ack");
                        this->cancel_timeout("wait_for_talking_ack");

                        this->call_started_callback_.call(telegram_data);

                        if(this->call_time_duration_ != 0)
                        {
                            this->set_timeout("call_time_limit", (this->call_time_duration_ * 8 * 1000), [this]() {
                                this->reset_call();

                                ESP_LOGD(TAG, "Maximum call time reached - disconnecting call");

                                TelegramData out_telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING, 63);
                                this->call_ended_callback_.call(out_telegram_data);
                            });
                        }
                    }
                    else
                    {
                        this->call_state_ = CallState::LINE_BUSY;
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS)
                {
                    // Other device accepted
                    if(this->call_state_ == CallState::OUT_CHECK_DST)
                    {
                        this->call_state_ = CallState::OUT_RINGING;

                        // reset call ack timeout
                        this->cancel_timeout("wait_for_call_ack");

                        ESP_LOGD(TAG, "Indoor Station acknowledged, waiting for start talking");

                        // wait for start talking telegram
                        this->set_timeout("wait_for_talking_ack", CALL_TIMEOUT_MS, [this]() {
                            this->reset_call();
                            ESP_LOGE(TAG, "START TALKING TIMEOUT - No response from indoor station");
                            this->call_failed_callback_.call();
                        });
                    }
                    else if(this->call_state_ == CallState::IN_WAIT_FOR_INIT)
                    {
                        this->call_state_ = CallState::CONNECTED;
                        this->call_started_callback_.call(telegram_data);

                        // reset ack timeouts
                        this->cancel_timeout("wait_for_call_ack");
                        this->cancel_timeout("wait_for_talking_ack");
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_RESET)
                {
                    this->memory_mode_ = false;
                    this->reset_call();
                }
                else if (telegram_data.type == TELEGRAM_TYPE_REQUEST_VERSION && telegram_data.serial_number == this->serial_number_)
                {
                    // TODO: use actual model identifier selected

                    // TASTA PRO AUDIO
                    // Version 2.9.12
                    // + Smart Stick
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, 0x007ED29C);
                }
            }
            else if(this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
            {
                // 1. call() - outgoing door call from this outdoor station to indoor station
                // 2. receive ACK STATUS from indoor station

                // 3. receive start talking from indoor station (address is this->address)
                // 4. send acknowledge to indoor station to initiate call


                // in: start talking from indoor station sn to this address
                if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
                {
                    // Outgoing door call accepted by indoor station
                    // or call initiated by indoor station directly to outdoor station address
                    if((this->call_state_ == CallState::IDLE || this->call_state_ == CallState::OUT_RINGING) && telegram_data.address == this->address_)
                    {
                        // out: acknowledge talk
                        this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 3);
                        this->call_state_ = CallState::CONNECTED;
                        this->call_started_callback_.call(telegram_data);

                        // reset ack timeouts
                        this->cancel_timeout("wait_for_call_ack");
                        this->cancel_timeout("wait_for_talking_ack");

                        // call time limit
                        if(this->call_time_duration_ != 0)
                        {
                            this->set_timeout("call_time_limit", (this->call_time_duration_ * 8 * 1000), [this]() {
                                this->reset_call();
                                
                                ESP_LOGD(TAG, "Maximum call time reached - disconnecting call from indoor station");

                                TelegramData out_telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, this->address_);
                                this->call_ended_callback_.call(out_telegram_data);
                            });
                        }
                    }
                    else
                    {
                        this->call_state_ = CallState::LINE_BUSY;
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING)
                {
                    this->call_state_ = CallState::LINE_BUSY;
                }
                else if (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS)
                {
                    // Other device accepted
                    if(this->call_state_ == CallState::OUT_CHECK_DST)
                    {
                        this->call_state_ = CallState::OUT_RINGING;

                        // reset call ack timeout
                        this->cancel_timeout("wait_for_call_ack");

                        ESP_LOGD(TAG, "Indoor Station acknowledged, waiting for start talking");

                        // wait for start talking telegram
                        this->set_timeout("wait_for_talking_ack", CALL_TIMEOUT_MS, [this]() {
                            this->reset_call();
                            ESP_LOGE(TAG, "Call timeout - No answer from indoor station");
                            this->call_failed_callback_.call();
                        });

                        // Door readiness timeout
                        if(this->door_readiness_duration_ != 0)
                        {
                            this->set_timeout("door_readiness_timeout", (this->door_readiness_duration_ * 8 * 1000), [this]() {
                                ESP_LOGD(TAG, "Maximum door readiness time reached");
                                this->tc_bus_->send_telegram(TELEGRAM_TYPE_END_OF_DOOR_READINESS, this->address_);
                            });
                        }
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_RESET || (telegram_data.type == TELEGRAM_TYPE_CONTROL_FUNCTION && telegram_data.payload == 0xD7 && telegram_data.serial_number == this->serial_number_))
                {
                    this->memory_mode_ = false;
                    this->reset_call();
                    this->cancel_timeout("door_readiness_timeout");
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_INITIALIZE_DOOR_STATION, this->address_);
                }
                else if (telegram_data.type == TELEGRAM_TYPE_REQUEST_VERSION && telegram_data.serial_number == this->serial_number_)
                {
                    // TODO: use actual model identifier selected

                    // PUK
                    // Version 2.9.12
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, 0x0420D29C);
                }
            }
        }
        else
        {
            // Physical Device
            if(telegram_data.type == TELEGRAM_TYPE_ACK_DATA)
            {
                if (this->current_flow_ == FLOW_READ_MEMORY)
                {
                    uint8_t percent = ((reading_memory_count_ + 1) * 100) / reading_memory_max_;

                    ESP_LOGI(TAG,   "Received device memory (%i%%):\n"
                                    "  Start address: %i\n"
                                    "  End address: %i\n"
                                    "  Block Data: %s",
                                    percent, (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4, format_hex_pretty(telegram_data.raw, ' ', false).c_str());

                    this->cancel_timeout("wait_for_first_memory_block");

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
                        ESP_LOGI(TAG, "  Total Size: %i Bytes", memory_buffer_.size());
                        ESP_LOGD(TAG, "  Buffer: %s", format_hex_pretty(memory_buffer_, ' ', false).c_str());

                        this->read_memory_complete_callback_.call(memory_buffer_);

                        this->publish_settings();

                        // Complete this request and process next in queue
                        this->complete_current_flow();
                    }
                    else
                    {
                        send_telegram(TELEGRAM_TYPE_READ_MEMORY_BLOCK, reading_memory_count_, 0);
                    }

                    // Do not proceed
                    return true;
                }
                else if (current_flow_ == FLOW_READ_MEMORY_UPDATE)
                {
                    ESP_LOGI(TAG,   "Received requested device memory block:\n"
                                    "  Start address: %i\n"
                                    "  End address: %i\n"
                                    "  Block Data: %s", 
                                    (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4, format_hex_pretty(telegram_data.raw, ' ', false).c_str());

                    this->cancel_timeout("wait_for_first_memory_block");

                    // Save Data to memory Store
                    memory_buffer_[reading_memory_count_]     = (telegram_data.raw >> 24) & 0xFF;
                    memory_buffer_[reading_memory_count_ + 1] = (telegram_data.raw >> 16) & 0xFF;
                    memory_buffer_[reading_memory_count_ + 2] = (telegram_data.raw >> 8) & 0xFF;
                    memory_buffer_[reading_memory_count_ + 3] = telegram_data.raw & 0xFF;

                    this->publish_settings();

                    // Complete this request and process next in queue
                    this->complete_current_flow();

                    // Do not proceed
                    return true;
                }
                else if (current_flow_ == FLOW_IDENTIFY_DEVICE)
                {
                    this->cancel_timeout("wait_for_identification_group_0");
                    this->cancel_timeout("wait_for_identification_group_1");
                    this->cancel_timeout("wait_for_identification_other");

                    ModelData device;
                    device.device_group = this->tc_bus_->get_selected_device_group();
                    device.memory_size = 0;
                    
                    const char* hex = telegram_data.hex;

                    if (hex[4] == 'D')
                    {
                        // New models

                        // FW Version
                        char fw_buf[4] = { hex[5], hex[6], hex[7], '\0' };
                        device.firmware_version = strtol(fw_buf, nullptr, 16);

                        // Firmware major, minor, patch (1 char each)
                        char tmp[2] = {0};
                        tmp[0] = hex[5];
                        device.firmware_major = strtol(tmp, nullptr, 16);
                        tmp[0] = hex[6];
                        device.firmware_minor = strtol(tmp, nullptr, 16);
                        tmp[0] = hex[7];
                        device.firmware_patch = strtol(tmp, nullptr, 16);

                        // Hardware version (first char, decimal)
                        tmp[0] = hex[0];
                        tmp[1] = '\0';
                        device.hardware_version = strtol(tmp, nullptr, 10);

                        // Model string (substring 1-3)
                        char model_buf[4] = { hex[1], hex[2], hex[3], '\0' };
                        device.model = identifier_string_to_model(device.device_group, model_buf, device.hardware_version, device.firmware_version);
                    }
                    else
                    {
                        if(device.device_group == 0 || device.device_group == 1)
                        {
                            // Old indoor station models
                            switch(telegram_data.raw)
                            {
                                // TTC-XX
                                case 0x08000040:
                                    device.model = MODEL_IS_TTCXX;
                                    break;

                                // TTS-XX
                                case 0x02010040:
                                    device.model = MODEL_IS_TTSXX;
                                    break;

                                // ISH 1030
                                case 0x08000048:
                                case 0x08080048:
                                    device.model = MODEL_IS_ISH1030;
                                    break;

                                default:
                                    ESP_LOGW(TAG,   "Partial device identification:\n"
                                                    "  This device is not fully supported in the identification routine.\n"
                                                    "  Potential models: ISH1030 / VMH / VML / VMF / VME / TTN / TTS / TTF / TC20X\n"
                                                    "  Note: Please open an issue and provide your logs in order to implement support for this device model.");
                                    break;
                            }
                        }
                        else if(device.device_group == 2)
                        {
                            // Old outdoor station models
                            switch(telegram_data.raw)
                            {
                                default:
                                    ESP_LOGW(TAG,   "Partial device identification:\n"
                                                    "  This device is not fully supported in the identification routine.\n"
                                                    "  Potential models: PAK / PUK / PES / PDS / TCU2 / AVD / VPDS / AVE / VPES / AVU / VPUK / AD1 / AD2 / AD3\n"
                                                    "  Note: Please open an issue and provide your logs in order to implement support for this device model.");
                                    break;
                            }
                        }
                        else if(device.device_group == 4)
                        {
                            // Old controller models
                            switch(telegram_data.raw)
                            {
                                case 0x877F5804:
                                    device.model = MODEL_CTRL_BVS20;
                                    break;

                                default:
                                    ESP_LOGW(TAG,   "Partial device identification:\n"
                                                    "  This device is not fully supported in the identification routine.\n"
                                                    "  Potential models: BVS20 / BVS100 / NVV1000 / PSG / VBVS05 / BSG2\n"
                                                    "  Note: Please open an issue and provide your logs in order to implement support for this device model.");
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

                        ESP_LOGI(TAG,   "Device identified:\n"
                                        "  Model: %s (%s)\n"
                                        "  Version: %i\n"
                                        "  Firmware: %i.%i.%i",
                                        model_to_string(device.model),
                                        device_group_to_string(device.device_group),
                                        device.hardware_version,
                                        device.firmware_major,
                                        device.firmware_minor,
                                        device.firmware_patch);

                        // Update Model
                        this->set_model(device.model);

                        this->identify_complete_callback_.call(device);
                    }
                    else
                    {
                        ESP_LOGE(TAG,   "Unable to identify device:\n"
                                        "  Group: %s\n"
                                        "  Data received: %s\n"
                                        "  Note: Please open an issue and provide your logs in order to implement support for this device model.",
                                        device_group_to_string(device.device_group),
                                        telegram_data.hex);
                        this->identify_unknown_callback_.call();
                    }

                    // Complete this request and process next in queue
                    this->complete_current_flow();

                    // Do not proceed
                    return true;
                }
            }
            else
            {
                #ifdef USE_BINARY_SENSOR
                // Fire Binary Sensors
                for (auto &listener : listeners_)
                {
                    bool allow_publish = (telegram_data.type == (listener->type_.value_or(TELEGRAM_TYPE_UNKNOWN))) &&
                        (telegram_data.address == listener->address_.value_or(0) || listener->address_.value_or(0) == 255) &&
                        (telegram_data.payload == listener->payload_.value_or(0) || listener->payload_.value_or(0) == 255) &&
                        (telegram_data.serial_number == this->serial_number_);

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

    bool TCBusDeviceComponent::reset_call()
    {
        if(!this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for virtual devices");
            return false;
        }

        bool was_active = (this->call_state_ != CallState::IDLE && this->call_state_ != CallState::LINE_BUSY);
        
        this->call_internal_ = false;
        this->call_address_ = 0;
        this->call_state_ = CallState::IDLE;

        if(was_active)
        {
            this->cancel_timeout("call_time_limit");
            this->cancel_timeout("wait_for_call_ack");
            this->cancel_timeout("wait_for_talking_ack");
            return true;
        }

        return false;
    }

    void TCBusDeviceComponent::call(uint32_t destination, bool internal)
    {
        if(!this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for virtual devices");
            return;
        }

        if(this->call_state_ != CallState::IDLE)
        {
            ESP_LOGE(TAG, "Line is busy");
            return;
        }

        this->call_address_ = destination;

        if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
        {
            if(internal)
            {
                // Call indoor station serial number from address 63 (static)
                this->call_internal_ = true;
                this->call_state_ = CallState::OUT_CHECK_DST;
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_INTERNAL_CALL, 63, 0, destination);

                // ack timeout
                this->set_timeout("wait_for_call_ack", 20, [this]() {
                    this->reset_call();
                    ESP_LOGE(TAG, "ACK TIMEOUT - No response from indoor station");
                    this->call_failed_callback_.call();
                });
            }
            else
            {
                // Talk directly to outdoor station address (destination) from this serial number
                this->call_internal_ = false;
                this->call_state_ = CallState::OUT_CHECK_DST;
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_START_TALKING_DOOR_CALL, destination, this->tc_bus_->is_door_readiness_active() ? 1 : 0, this->serial_number_);
                
                // ack timeout
                this->set_timeout("wait_for_talking_ack", 20, [this]() {
                    this->reset_call();
                    ESP_LOGE(TAG, "ACK TIMEOUT - No response from outdoor station");
                    this->call_failed_callback_.call();
                });
            }
        }
        else if(this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            this->cancel_timeout("door_readiness_timeout");

            // Call indoor station serial number from this outdoor station
            this->call_internal_ = false;
            this->call_state_ = CallState::OUT_CHECK_DST;

            // send door call to indoor station from this outdoor station
            // destination serial number of indoor station
            // source address of this outdoor station
            this->tc_bus_->send_telegram(TELEGRAM_TYPE_DOOR_CALL, this->address_, 0, destination);
            
            // ack timeout
            this->set_timeout("wait_for_call_ack", 20, [this]() {
                this->reset_call();
                ESP_LOGE(TAG, "ACK TIMEOUT - No response from indoor station");
                this->call_failed_callback_.call();
            });
        }
    }

    void TCBusDeviceComponent::answer_call()
    {
        if(!this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for virtual devices");
            return;
        }

        if(this->call_state_ != CallState::IN_RINGING)
        {
            ESP_LOGE(TAG, "No call to answer!");
            return;
        }

        this->tc_bus_->send_telegram(this->call_internal_ ? TELEGRAM_TYPE_START_TALKING : TELEGRAM_TYPE_START_TALKING_DOOR_CALL, this->call_address_, 0, this->serial_number_);
        this->call_state_ = CallState::IN_WAIT_FOR_INIT;
        
        // ack timeout
        this->set_timeout("wait_for_talking_ack", 20, [this]() {
            this->reset_call();
            ESP_LOGE(TAG, "ACK TIMEOUT - No response from outdoor station");
            this->call_failed_callback_.call();
        });
    }

    void TCBusDeviceComponent::end_call()
    {
        if(!this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for virtual devices");
            return;
        }

        if(this->call_state_ != CallState::CONNECTED && this->call_state_ != CallState::OUT_CHECK_DST && this->call_state_ != CallState::OUT_RINGING)
        {
            ESP_LOGE(TAG, "No call to end!");
            return;
        }

        TelegramData telegram_data;

        if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
        {
            if(this->call_internal_)
            {
                ESP_LOGD(TAG, "End call - disconnecting call (IS -> IS)");

                telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING, 63);
            }
            else
            {
                ESP_LOGD(TAG, "End call - disconnecting call (IS -> AS)");
                telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, (uint8_t)this->call_address_);
            }
        }
        else if(this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGD(TAG, "End call - disconnecting call (AS -> IS)");
            telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, this->address_);
        }

        this->reset_call();

        this->call_ended_callback_.call(telegram_data);
    }

    void TCBusDeviceComponent::publish_settings()
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        if(this->memory_buffer_empty() || this->model_ == MODEL_NONE)
        {
            return;
        }

        ESP_LOGI(TAG,   "Device Settings:\n"
                        "  Model: %s (%s)\n"
                        "  Serial Number: %i",
                        model_to_string(this->model_), device_group_to_string(this->device_group_), this->serial_number_);

        if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
        {
            if(supports_setting(SETTING_RINGTONE_MUTE))
            {
                ESP_LOGI(TAG, "  Ringtone muted: %s", YESNO(get_setting(SETTING_RINGTONE_MUTE)));
            }
            
            if(supports_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL))
            {
                ESP_LOGI(TAG, "  Entrance Door Call Ringtone: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL)));
            }
            if(supports_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL))
            {
                ESP_LOGI(TAG, "  Second Entrance Door Call Ringtone: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL)));
            }
            if(supports_setting(SETTING_RINGTONE_INTERNAL_CALL))
            {
                ESP_LOGI(TAG, "  Internal Call Ringtone: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_INTERNAL_CALL)));
            }
            if(supports_setting(SETTING_RINGTONE_FLOOR_CALL))
            {
                ESP_LOGI(TAG, "  Floor Call Ringtone: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_FLOOR_CALL)));
            }
            if(supports_setting(SETTING_VOLUME_RINGTONE))
            {
                ESP_LOGI(TAG, "  Ringtone volume: %i", get_setting(SETTING_VOLUME_RINGTONE));
            }

            if(supports_setting(SETTING_VOLUME_HANDSET_DOOR_CALL))
            {
                ESP_LOGI(TAG, "  Handset volume (Door Call): %i", get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
            }
            if(supports_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL))
            {
                ESP_LOGI(TAG, "  Handset volume (Internal Call): %i", get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
            }

            if(supports_setting(SETTING_AS_ADDRESS_DIVIDER))
            {
                ESP_LOGI(TAG, "  Address Divider (AS): %i", get_setting(SETTING_AS_ADDRESS_DIVIDER));
            }
            if(supports_setting(SETTING_VAS_ADDRESS_DIVIDER))
            {
                ESP_LOGI(TAG, "  Address Divider (VAS): %i", get_setting(SETTING_VAS_ADDRESS_DIVIDER));
            }

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
            if (this->address_number_ != nullptr)
            {
                this->address_number_->publish_state(get_setting(SETTING_AS_ADDRESS));
            }
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

            #ifdef USE_SWITCH
            if (this->ringtone_mute_switch_)
            {
                this->ringtone_mute_switch_->publish_state(get_setting(SETTING_RINGTONE_MUTE) == 1);
            }
            #endif
        }
        else if(this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            if(supports_setting(SETTING_AS_ADDRESS))
            {
                ESP_LOGI(TAG, "  Address: %i", get_setting(SETTING_AS_ADDRESS));
            }
            if(supports_setting(SETTING_AS_ADDRESS_LOCK))
            {
                ESP_LOGI(TAG, "  Address Lock: %s", YESNO(get_setting(SETTING_AS_ADDRESS_LOCK)));
            }
            if(supports_setting(SETTING_TALKING_REQUIRES_DOOR_READINESS))
            {
                ESP_LOGI(TAG, "  Talking requires door readiness: %s", YESNO(get_setting(SETTING_TALKING_REQUIRES_DOOR_READINESS)));
            }
            if(supports_setting(SETTING_DOOR_OPENER_DURATION))
            {
                uint8_t door_opener_dur = get_setting(SETTING_DOOR_OPENER_DURATION);
                ESP_LOGI(TAG, "  Door Opener Duration: %i sec.", door_opener_dur);
            }
            if(supports_setting(SETTING_CALLING_DURATION))
            {
                uint8_t calling_dur = get_setting(SETTING_CALLING_DURATION);
                ESP_LOGI(TAG, "  Calling Duration: %f sec.", calling_dur * 0.5);
            }
            if(supports_setting(SETTING_DOOR_READINESS_DURATION))
            {
                uint8_t door_readiness_dur = get_setting(SETTING_DOOR_READINESS_DURATION);
                if(door_readiness_dur == 0)
                {
                    ESP_LOGI(TAG, "  Door Readiness Duration: Unlimited");
                }
                else
                {
                    ESP_LOGI(TAG, "  Door Readiness Duration: %i sec.", door_readiness_dur * 8);
                }
            }

            uint8_t button_rows = get_setting(SETTING_BUTTON_ROWS);
            uint8_t button_cols = 1;
            uint8_t col_offset = 0;

            if(this->model_ == MODEL_AS_TCU2)
            {
                button_rows = 4;
                button_cols = 4;
                col_offset = 4;
            }
            else if (this->model_ == MODEL_AS_PES)
            {
                button_cols = 2;
            }
            else if (this->model_ == MODEL_AS_PDS0X || this->model_ == MODEL_AS_PDS0X04)
            {
                if(get_setting(SETTING_HAS_CODE_LOCK) == 254)
                {
                    button_rows = 1;
                }
            }

            ESP_LOGI(TAG, "  Physical Buttons: %i", button_rows * button_cols);
            if (button_rows == 0 || button_rows == 255 || button_cols == 0)
            {
                ESP_LOGW(TAG, "    Invalid button configuration: rows=%i, cols=%i. Skipping button listing.", button_rows, button_cols);
            }
            else
            {
                for (uint8_t row = 1; row <= button_rows; row++) {
                    for (uint8_t col = 1; col <= button_cols; col++) {
                        // For non-matrix models, col is ignored (pass 0)
                        uint8_t col_param = (button_cols > 1) ? col + col_offset : 0;
                        DoorbellButtonConfig btn = get_doorbell_button(row, col_param);
                        
                        if (button_cols > 1) {
                            ESP_LOGI(TAG, "    Button [%i,%i]:", row, col);
                        } else {
                            ESP_LOGI(TAG, "    Button %i:", row);
                        }
                        
                        ESP_LOGI(TAG, "      Primary Action: %s", doorbell_button_action_to_string(btn.primary_action));
                        if(btn.primary_action != DOORBELL_BUTTON_ACTION_NONE)
                        {
                            ESP_LOGI(TAG, "        Payload: %i", btn.primary_payload);
                        }
                        ESP_LOGI(TAG, "      Secondary Action: %s", doorbell_button_action_to_string(btn.secondary_action));
                        if(btn.secondary_action != DOORBELL_BUTTON_ACTION_NONE)
                        {
                            ESP_LOGI(TAG, "        Payload: %i", btn.secondary_payload);
                        }
                    }
                }
            }
        }
        else
        {
            // Other devices
            ESP_LOGW(TAG, "  Note: This Device Group is not implemented yet!");
        }
    }

    #ifdef USE_BINARY_SENSOR
    void TCBusDeviceComponent::register_listener(TCBusDeviceListener *listener)
    {
        this->listeners_.push_back(listener);
    }
    #endif

    TelegramData TCBusDeviceComponent::send_telegram(TelegramType type, uint8_t address, uint32_t payload, uint32_t wait_duration)
    {
        // Use 32-bit protocol
        if(type == TELEGRAM_TYPE_OPEN_DOOR)
        {
            if(this->virtual_device_ == false && this->get_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL) == 1)
            {
                type = TELEGRAM_TYPE_OPEN_DOOR_LONG;
            }
            else if(this->force_long_door_opener_protocol_)
            {
                ESP_LOGV(TAG, "Detected 32-bit door protocol override, change telegram telegram to OPEN_DOOR_LONG.");
                type = TELEGRAM_TYPE_OPEN_DOOR_LONG;
            }
        }

        return this->tc_bus_->send_telegram(type, address, payload, this->serial_number_, wait_duration);
    }

    void TCBusDeviceComponent::identify_device()
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        if(this->serial_number_ == 0)
        {
            ESP_LOGE(TAG, "Device model cannot be identified without a serial number!");
            return;
        }

        enqueue_flow(FLOW_IDENTIFY_DEVICE, this);
    }

    void TCBusDeviceComponent::execute_identify_device()
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        this->cancel_timeout("wait_for_identification_group_0");
        this->cancel_timeout("wait_for_identification_group_1");
        this->cancel_timeout("wait_for_identification_other");

        if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION)
        {
            // Indoor Stations

            // First try with group 0
            ESP_LOGI(TAG,   "Identify device:\n"
                            "  Group: %s (%i)\n"
                            "  Serial Number: %i",
                            device_group_to_string(DEVICE_GROUP_INDOOR_STATION), 0, this->serial_number_);

            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, 0); // group 0
            send_telegram(TELEGRAM_TYPE_REQUEST_VERSION, 0, 0);

            this->set_timeout("wait_for_identification_group_0", 1000, [this]()
            {
                // Didn't receive identify result of group 0
                // Second try with group 1
                ESP_LOGI(TAG,   "Identify device:\n"
                                "  Group: %s (%i)\n"
                                "  Serial Number: %i",
                                device_group_to_string(DEVICE_GROUP_INDOOR_STATION), 1, this->serial_number_);
                send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, 1); // group 1
                send_telegram(TELEGRAM_TYPE_REQUEST_VERSION, 0, 0);

                this->set_timeout("wait_for_identification_group_1", 1000, [this]()
                {
                    // Didn't receive identify result of group 1
                    // Failed

                    this->identify_timeout_callback_.call();
                    ESP_LOGE(TAG, "Identification timeout. The device model may not support identification. Please select model manually.");

                    // Complete this request and process next in queue
                    this->complete_current_flow();
                });
            });
        }
        else
        {
            // Other Devices

            // Use device group if not 0 and 1
            ESP_LOGI(TAG,   "Identify device:\n"
                            "  Group: %s\n"
                            "  Serial Number: %i",
                            device_group_to_string(this->device_group_), this->serial_number_);
            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, (uint8_t)this->device_group_);
            send_telegram(TELEGRAM_TYPE_REQUEST_VERSION, 0, 0);

            this->set_timeout("wait_for_identification_other", 1000, [this]() {
                // Failed
                ESP_LOGE(TAG, "Identification timeout. The device model may not support identification. Please select model manually.");

                // Complete this request and process next in queue
                this->complete_current_flow();
            });
        }
    }

    // Flow Queue Management
    void TCBusDeviceComponent::enqueue_flow(FlowType type, TCBusDeviceComponent *component, uint8_t index)
    {
        TCBusDeviceFlowQueueItem flow;
        flow.type = type;
        flow.component = component;
        flow.index = index;
        
        if (!s_flow_queue.push(flow))
        {
            ESP_LOGE(component->TAG, "Flow queue is full! Flow dropped.");
            return;
        }

        ESP_LOGD(component->TAG, "Flow enqueued.");
    }

    void TCBusDeviceComponent::process_flow_queue()
    {
        if (s_flow_queue.empty())
        {
            ESP_LOGV(FLOW_QUEUE_TAG, "Queue is empty, nothing to process.");
            return;
        }
        
        if (s_running_flow)
        {
            ESP_LOGV(FLOW_QUEUE_TAG, "Flow already running, waiting...");
            return;
        }

        uint32_t now = millis();
        if (s_last_flow_completion_time > 0 && (now - s_last_flow_completion_time) < 1000)
        {
            ESP_LOGV(FLOW_QUEUE_TAG, "Waiting for cooldown period (%u ms remaining)...", 1000 - (now - s_last_flow_completion_time));
            return;
        }

        TCBusDeviceFlowQueueItem flow = s_flow_queue.front();
        s_flow_queue.pop();

        ESP_LOGD(flow.component->TAG, "Processing queued flow.");

        s_running_flow = true;
        flow.component->set_current_flow(flow.type);

        // Execute the queued flow
        switch (flow.type)
        {
            case FLOW_READ_MEMORY:
                flow.component->execute_read_memory();
                break;
            case FLOW_READ_MEMORY_UPDATE:
                flow.component->execute_read_memory_update(flow.index);
                break;
            case FLOW_IDENTIFY_DEVICE:
                flow.component->execute_identify_device();
                break;
            case FLOW_NONE:
            default:
                ESP_LOGW(flow.component->TAG, "Invalid flow type %d, skipping.", flow.type);
                flow.component->complete_current_flow();
                break;
        }
    }

    void TCBusDeviceComponent::complete_current_flow()
    {
        if (current_flow_ == FLOW_NONE)
        {
            ESP_LOGW(TAG, "complete_current_flow() called but no flow was running!");
            return;
        }

        s_running_flow = false;
        current_flow_ = FLOW_NONE;

        s_last_flow_completion_time = millis();
    }

    void TCBusDeviceComponent::read_memory()
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

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
            ESP_LOGE(TAG, "The model %s (Serial: %i) does not support reading memory!", model_to_string(this->model_), this->serial_number_);
            // Call timeout callback for unsupported models
            this->read_memory_timeout_callback_.call();
            return;
        }

        // Check if flow is already running
        enqueue_flow(FLOW_READ_MEMORY, this);
    }

    void TCBusDeviceComponent::execute_read_memory()
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        ESP_LOGI(TAG,   "Read device memory:\n"
                        "  Model: %s (%s)\n"
                        "  Serial Number: %i",
                        model_to_string(this->model_), device_group_to_string(this->device_group_), this->serial_number_);

        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group);
        send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0);

        memory_buffer_.clear();

        reading_memory_count_ = 0;
        reading_memory_max_ = (this->model_data_.memory_size / 4);

        this->set_timeout("wait_for_first_memory_block", 2000, [this]()
        {
            memory_buffer_.clear();
            reading_memory_count_ = 0;
            reading_memory_max_ = 0;

            this->read_memory_timeout_callback_.call();
            ESP_LOGE(TAG, "First memory block not received in time. Reading canceled!");

            // Complete this request and process next in queue
            this->complete_current_flow();
        });

        send_telegram(TELEGRAM_TYPE_READ_MEMORY_BLOCK, reading_memory_count_, 0);
    }

    void TCBusDeviceComponent::read_memory_update(uint8_t index)
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

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
            ESP_LOGE(TAG, "The model %s (Serial: %i) does not support reading memory!", model_to_string(this->model_), this->serial_number_);
            // Call timeout callback for unsupported models
            this->read_memory_timeout_callback_.call();
            return;
        }

        enqueue_flow(FLOW_READ_MEMORY_UPDATE, this, index);
    }

    void TCBusDeviceComponent::execute_read_memory_update(uint8_t index)
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        ESP_LOGI(TAG, "Read device memory after update:\n"
                        "  Model: %s (%s)\n"
                        "  Serial Number: %i",
                        model_to_string(this->model_), device_group_to_string(this->device_group_), this->serial_number_);

        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group);
        send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0);

        reading_memory_count_ = (index / 4);

        this->set_timeout("wait_for_first_memory_block", 2000, [this]()
        {
            reading_memory_count_ = 0;
            reading_memory_max_ = 0;

            ESP_LOGE(TAG, "Memory block not received in time. Reading canceled!");

            // Complete this request and process next in queue
            this->complete_current_flow();
        });

        send_telegram(TELEGRAM_TYPE_READ_MEMORY_BLOCK, reading_memory_count_, 0);
    }

    uint8_t TCBusDeviceComponent::get_doorbell_button_memory_index(uint8_t row, uint8_t col)
    {
        // Model-specific button index mappings
        static const uint8_t dsp_indices[][10] = {
            {}, {20}, {20, 26}, {14, 20, 26}, {8, 14, 20, 26, 32, 38, 44, 50, 56, 62}
        };

        static const uint8_t puk_indices[][10] = {
            {}, {20}, {20, 14}, {26, 20, 14}, {32, 26, 20, 14, 8, 38, 44, 50, 56, 62}
        };

        static const uint8_t pakv2_indices[][8] = {
            {}, {20}, {20, 26}, {14, 20, 26}, {8, 14, 20, 26, 32, 38, 44, 50}
        };

        static const uint8_t pakv3_indices[][8] = {
            {}, {14}, {14, 20}, {8, 14, 20}, {8, 14, 20, 26, 32, 38, 44, 50}
        };

        static const uint8_t pds0x_indices[][3] = {
            {}, {104}, {98, 104}, {98, 104, 110}
        };
        static const uint8_t pds_indices[][3] = {
            {}, {44}
        };
        static const uint8_t pds0x04_indices[][3] = {
            {}, {86}, {92, 86}, {92, 86, 80}
        };

        // Matrix [row 1-4][col 5-8] -> values 98 down to 8
        static const uint8_t tcu2_indices[][4] = {
            {98, 92, 86, 80},
            {74, 68, 62, 56},
            {50, 44, 38, 32},
            {26, 20, 14, 8}
        };

        static const uint8_t tcu34_indices[][4] = {
            {8, 14, 20, 26},
            {32, 38, 44, 50},
            {56, 62, 68, 74},
            {80, 86, 92, 98}
        };

        uint8_t button_rows = get_setting(SETTING_BUTTON_ROWS);

        if (this->model_ == MODEL_AS_TCU2)
        {
            if (row >= 1 && row <= 4 && col >= 5 && col <= 8)
            {
                return tcu2_indices[row - 1][col - 5];
            }
            return 0;
        }
        
        if (this->model_ == MODEL_AS_TCU3 || this->model_ == MODEL_AS_TCU4)
        {
            if (row >= 1 && row <= 4 && col >= 5 && col <= 8)
            {
                return tcu34_indices[row - 1][col - 5];
            }
            return 0;
        }
        
        if (this->model_ == MODEL_AS_PES)
        {
            if (col < 1 || col > 2 || row < 1)
            {
                return 0;
            }

            uint8_t offset = 0;
            
            // If button_rows <= 4, use offset to shift rows
            if (button_rows <= 4)
            {
                offset = 4;
            }
            else
            {
                // For button_rows > 4, manually map first 4 rows
                if (row <= 4)
                {
                    static const uint8_t pes_first_4[][2] = {
                        {92, 32}, {86, 26}, {80, 20}, {74, 14}
                    };
                    return pes_first_4[row - 1][col - 1];
                }
            }

            // Map remaining rows (5-10) or all rows (1-6) if offset=4
            static const uint8_t pes_dynamic[][2] = {
                {68, 8}, {98, 38}, {104, 44}, {110, 50}, {116, 56}, {122, 62}
            };

            uint8_t dynamic_row = row + offset - 5;  // Convert to 0-based index
            if (dynamic_row < 6)
            {
                return pes_dynamic[dynamic_row][col - 1];
            }

            return 0;
        }

        const uint8_t* indices = nullptr;
        uint8_t max_buttons = 0;

        switch(this->model_)
        {
            case MODEL_AS_PUK_DSP:
                indices = &dsp_indices[0][0];
                max_buttons = 10;
                break;
            case MODEL_AS_PUK:
                indices = &puk_indices[0][0];
                max_buttons = 10;
                break;
            case MODEL_AS_PAKV2:
                indices = &pakv2_indices[0][0];
                max_buttons = 8;
                break;
            case MODEL_AS_PAKV3:
                indices = &pakv3_indices[0][0];
                max_buttons = 8;
                break;
            case MODEL_AS_PDS0X:
                if(get_setting(SETTING_HAS_CODE_LOCK) == 254)
                {
                    indices = &pds_indices[0][0];
                    max_buttons = 1;
                }
                else
                {
                    indices = &pds0x04_indices[0][0];
                    max_buttons = 3;
                }
                break;
            case MODEL_AS_PDS0X04:
                if(get_setting(SETTING_HAS_CODE_LOCK) == 254)
                {
                    indices = &pds_indices[0][0];
                    max_buttons = 1;
                }
                else
                {
                    indices = &pds0x04_indices[0][0];
                    max_buttons = 3;
                }
                break;
            default:
                return 0;
        }

        if (indices && row > 0 && row <= max_buttons)
        {
            uint8_t config = (button_rows <= 3) ? button_rows : 4;
            return indices[config * max_buttons + row - 1];
        }

        return 0;
    }

    DoorbellButtonConfig TCBusDeviceComponent::get_doorbell_button(uint8_t row)
    {
        return get_doorbell_button(row, 1);
    }

    DoorbellButtonConfig TCBusDeviceComponent::get_doorbell_button(uint8_t row, uint8_t col)
    {
        DoorbellButtonConfig button{};

        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return button;
        }

        if (this->memory_buffer_empty())
        {
            ESP_LOGE(TAG, "Memory buffer is empty. Please read memory before proceeding!");
            return button;
        }

        if (this->model_ == MODEL_NONE || this->model_data_.device_group != 2)
        {
            ESP_LOGE(TAG, "This device is not an outdoor station and unsupported!");
            return button;
        }

        uint8_t base_index = get_doorbell_button_memory_index(row, col);
        if (base_index == 0)
        {
            ESP_LOGW(TAG, "No memory index for button found!");
            return button;
        }

        // First action
        uint8_t primary_action_value = (memory_buffer_[base_index] >> 4) & 0x0F;
        if (primary_action_value == 0xF || primary_action_value == 0x0 || primary_action_value == 0x1 || primary_action_value == 0x2)
        {
            button.primary_action = static_cast<DoorbellButtonAction>(primary_action_value);
        }
        else
        {
            button.primary_action = DOORBELL_BUTTON_ACTION_NONE;
        }
        button.primary_payload = ((memory_buffer_[base_index] & 0x0F) << 16) | (memory_buffer_[base_index + 1] << 8) | memory_buffer_[base_index + 2];

        // Second action
        uint8_t secondary_action_value = (memory_buffer_[base_index + 3] >> 4) & 0x0F;
        if (secondary_action_value == 0xF || secondary_action_value == 0x0 || secondary_action_value == 0x1 || secondary_action_value == 0x2)
        {
            button.secondary_action = static_cast<DoorbellButtonAction>(secondary_action_value);
        }
        else
        {
            button.secondary_action = DOORBELL_BUTTON_ACTION_NONE;
        }
        button.secondary_payload = ((memory_buffer_[base_index + 3] & 0x0F) << 16) | (memory_buffer_[base_index + 4] << 8) | memory_buffer_[base_index + 5];

        return button;
    }

    bool TCBusDeviceComponent::update_doorbell_button(uint8_t row, DoorbellButtonConfig button)
    {
        return update_doorbell_button(row, 1, button);
    }

    bool TCBusDeviceComponent::update_doorbell_button(uint8_t row, uint8_t col, DoorbellButtonConfig button)
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return false;
        }

        if (this->memory_buffer_empty())
        {
            ESP_LOGE(TAG, "Memory buffer is empty. Please read memory before proceeding!");
            return false;
        }

        if (this->model_ == MODEL_NONE || this->model_data_.device_group != 2)
        {
            ESP_LOGE(TAG, "This device is not an outdoor station and unsupported!");
            return false;
        }

        if (!(this->model_data_.capabilities & CAP_UPDATE_DOORBELL_BUTTON))
        {
            ESP_LOGE(TAG, "For your own safety, the feature is disabled on this device!");
            return false;
        }

        uint8_t base_index = get_doorbell_button_memory_index(row, col);
        if (base_index == 0)
        {
            ESP_LOGE(TAG, "No memory index for button found!");
            return false;
        }

        // First action (bytes 0-2)
        memory_buffer_[base_index] = (button.primary_action << 4) | ((button.primary_payload >> 16) & 0x0F);
        memory_buffer_[base_index + 1] = (button.primary_payload >> 8) & 0xFF;
        memory_buffer_[base_index + 2] = button.primary_payload & 0xFF;

        // Second action (bytes 3-5)
        memory_buffer_[base_index + 3] = (button.secondary_action << 4) | ((button.secondary_payload >> 16) & 0x0F);
        memory_buffer_[base_index + 4] = (button.secondary_payload >> 8) & 0xFF;
        memory_buffer_[base_index + 5] = button.secondary_payload & 0xFF;

        // Prepare Transmission
        // Select device group
        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group);

        // Select memory page %i of serial number %i
        send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0);

        // Transfer new button assignments (2 bytes per transmission)
        uint16_t value1 = (memory_buffer_[base_index] << 8) | memory_buffer_[base_index + 1];
        uint16_t value2 = (memory_buffer_[base_index + 2] << 8) | memory_buffer_[base_index + 3];
        uint16_t value3 = (memory_buffer_[base_index + 4] << 8) | memory_buffer_[base_index + 5];

        send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, base_index, value1);
        send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, base_index + 2, value2);
        send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, base_index + 4, value3);

        // Reset
        if(!(this->model_data_.capabilities & CAP_INDIVIDUAL_RESET))
        {
            send_telegram(TELEGRAM_TYPE_RESET);
        }
        else
        {
            send_telegram(TELEGRAM_TYPE_CONTROL_FUNCTION, 0, 0xD7);
        }
        
        return true;
    }

    uint8_t TCBusDeviceComponent::get_memory_byte(uint8_t index)
    {
        if (this->memory_buffer_empty() || index >= memory_buffer_.size())
        {
            return 0xFF;
        }
        return memory_buffer_[index];
    }

    bool TCBusDeviceComponent::supports_setting(SettingType type)
    {
        if(this->virtual_device_)
        {
            return true;
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
        // Get internal state for virtual device first,
        // because it can support all settings independent
        // of model and memory buffer state
        if(this->virtual_device_)
        {
            if(type == SETTING_AS_ADDRESS)
            {
                return this->address_;
            }
            else if(type == SETTING_AS_ADDRESS_DIVIDER)
            {
                return this->address_divider_;
            }
            else if(type == SETTING_DOOR_READINESS_DURATION)
            {
                return this->door_readiness_duration_;
            }
            else if(type == SETTING_CALLING_DURATION)
            {
                return this->call_time_duration_;
            }
            else if(type == SETTING_DOOR_OPENER_DURATION)
            {
                return this->door_opener_duration_;
            }
            else if(type == SETTING_USE_LONG_DOOR_OPENER_PROTOCOL)
            {
                return this->force_long_door_opener_protocol_ ? 1 : 0;
            }
            else if(type == SETTING_AUTO_ANSWER_CALL)
            {
                return this->auto_answer_call_ ? 1 : 0;
            }
        }

        if (this->memory_buffer_empty())
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
        if (cellData.index == 0)
        {
            ESP_LOGV(TAG, "The setting '%s' is not available for model '%s'.", setting_type_to_string(type), model_to_string(this->model_));
            return 0;
        }

        uint8_t shift = cellData.start_bit - cellData.length + 1;
        uint8_t mask = (1 << cellData.length) - 1;
        uint8_t value = (memory_buffer_[cellData.index] >> shift) & mask;
        return value;
    }

    bool TCBusDeviceComponent::update_setting(SettingType type, uint8_t new_value)
    {
        if(this->virtual_device_)
        {
            if(type == SETTING_AS_ADDRESS)
            {
                if(new_value > 0xFF)
                {
                    ESP_LOGW(TAG, "Invalid Address, reset to 0.");
                    new_value = 0;
                }
                this->address_ = new_value;

                // Update Entities
                #ifdef USE_NUMBER
                if (this->address_number_ != nullptr)
                {
                    this->address_number_->publish_state(new_value);
                }
                #endif
            }
            if(type == SETTING_AS_ADDRESS_DIVIDER)
            {
                this->address_divider_ = new_value;
            }
            else if(type == SETTING_DOOR_READINESS_DURATION)
            {
                this->door_readiness_duration_ = new_value;
            }
            else if(type == SETTING_CALLING_DURATION)
            {
                this->call_time_duration_ = new_value;
            }
            else if(type == SETTING_DOOR_OPENER_DURATION)
            {
                this->door_opener_duration_ = new_value;
            }
            else if(type == SETTING_USE_LONG_DOOR_OPENER_PROTOCOL)
            {
                this->force_long_door_opener_protocol_ = (new_value != 0);
            }
            else if(type == SETTING_AUTO_ANSWER_CALL)
            {
                this->auto_answer_call_ = (new_value != 0);
            }

            this->save_preferences();
        }


        if (this->memory_buffer_empty())
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
            if(!this->virtual_device_)
            {
                ESP_LOGE(TAG, "Cannot write because setting %s is not available for model %s!", setting_type_to_string(type), model_to_string(this->model_));
            }
            return false;
        }

        ESP_LOGI(TAG,   "Write setting to device:\n"
                        "  Model: %s (%s)\n"
                        "  Serial Number: %i\n"
                        "  Setting: %s\n"
                        "  Value: %X",
                        model_to_string(this->model_), device_group_to_string(this->device_group_), this->serial_number_, setting_type_to_string(type), new_value);
        
        // Apply new data
        uint8_t shift = cellData.start_bit - cellData.length + 1;
        uint8_t mask = (1 << cellData.length) - 1;
        uint8_t current_byte = memory_buffer_[cellData.index];
        current_byte &= ~(mask << shift);
        current_byte |= ((new_value & mask) << shift);
        memory_buffer_[cellData.index] = current_byte;

        // Update memory of physical device
        if(this->virtual_device_ == false)
        {
            // Prepare Transmission
            // Select device group
            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group);

            // Select memory page %i of serial number %i
            send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0);

            // Transfer new settings value to memory
            uint16_t new_values = (memory_buffer_[cellData.index] << 8) | memory_buffer_[cellData.index + 1];
            send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, cellData.index, new_values);

            // Reset
            if(!(this->model_data_.capabilities & CAP_INDIVIDUAL_RESET))
            {
                send_telegram(TELEGRAM_TYPE_RESET);
            }
            else
            {
                send_telegram(TELEGRAM_TYPE_CONTROL_FUNCTION, 0, 0xD7);
            }
        }
        else
        {
            ESP_LOGV(TAG, "Memory buffer updated without sending telegrams.");
        }

        return true;
    }

    bool TCBusDeviceComponent::write_memory()
    {
        if(this->virtual_device_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return false;
        }

        if (this->memory_buffer_empty())
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

        ESP_LOGI(TAG,   "Write memory buffer to device:\n"
                        "  Model: %s (%s)\n"
                        "  Serial Number: %i\n"
                        "  Size: %i Bytes",
                        model_to_string(this->model_), device_group_to_string(this->device_group_), this->serial_number_, this->memory_buffer_.size());
        ESP_LOGD(TAG,   "  Data: %s", format_hex_pretty(this->memory_buffer_, ' ', false).c_str());

        // Prepare Transmission
        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group);

        send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0);

        // Transmit Memory
        for (size_t address = 0; address < memory_buffer_.size(); address += 2)
        {
            uint16_t new_value = (memory_buffer_[address] << 8) | memory_buffer_[address + 1];
            send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, address, new_value);
        }

        // Reset
        if(!(this->model_data_.capabilities & CAP_INDIVIDUAL_RESET))
        {
            send_telegram(TELEGRAM_TYPE_RESET);
        }
        else
        {
            send_telegram(TELEGRAM_TYPE_CONTROL_FUNCTION, 0, 0xD7);
        }

        return true;
    }

}