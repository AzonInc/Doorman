#include "protocol.h"
#include "tc_bus.h"

#include "esphome.h"
#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#ifdef USE_API
#include "esphome/components/api/custom_api_device.h"
#endif

#if defined(USE_ESP_IDF) || (defined(USE_ARDUINO) && defined(ESP32))
#include "soc/efuse_reg.h"
#include "soc/efuse_periph.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#endif

#ifdef USE_ARDUINO
#include "Arduino.h"
#endif

#include <optional>
#include <utility>
#include <vector>
#include <cinttypes>

using namespace esphome;

namespace esphome
{
    namespace tc_bus
    {
        static const char *const TAG = "tc_bus";

        uint32_t global_tc_bus_id = 1911044085ULL;
        TCBusComponent *global_tc_bus = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        void TCBusComponent::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up TC:BUS Intercom...");

            this->pref_ = global_preferences->make_preference<TCBusSettings>(global_tc_bus_id);
            
            TCBusSettings recovered;

            if (!this->pref_.load(&recovered)) {
                recovered = {MODEL_NONE, 0};
            }

            this->model_ = recovered.model;
            this->serial_number_ = recovered.serial_number;

            #ifdef USE_SELECT
            if (this->model_select_ != nullptr) {
                this->model_select_->publish_state(model_to_string(this->model_));
            }
            #endif
            #ifdef USE_NUMBER
            if (this->serial_number_number_ != nullptr) {
                this->serial_number_number_->publish_state(this->serial_number_);
            }
            #endif
            
            #if defined(USE_ESP_IDF) || (defined(USE_ARDUINO) && defined(ESP32))
            ESP_LOGD(TAG, "Checking for Doorman hardware");

            // Doorman Hardware Revision
            uint8_t ver[3];
            uint32_t value;
            esp_efuse_read_block(EFUSE_BLK3, &value, 0, 24);
            ver[0] = value >> 0;
            ver[1] = value >> 8;
            ver[2] = value >> 16;

            if(ver[0] > 0) {
                ESP_LOGI(TAG, "Doorman hardware detected: Revision %i.%i.%i.", ver[0], ver[1], ver[2]);
                this->hardware_version_str_ = "Doorman-S3 " + std::to_string(ver[0]) + "." + std::to_string(ver[1]) + "." + std::to_string(ver[2]);
            }
            #endif

            #ifdef USE_TEXT_SENSOR
            if (this->hardware_version_text_sensor_ != nullptr) {
                this->hardware_version_text_sensor_->publish_state(this->hardware_version_str_);
            }
            #endif

            // Reset Sensors
            #ifdef USE_TEXT_SENSOR
            if (this->bus_command_text_sensor_ != nullptr) {
                this->bus_command_text_sensor_->publish_state("");
            }
            #endif

            #ifdef USE_BINARY_SENSOR
            for (auto &listener : listeners_) {
                listener->turn_off(&listener->timer_);
            }
            #endif

            // Register remote receiver listener
            this->rx_->register_listener(this);
        }

        void TCBusComponent::save_settings()
        {
            TCBusSettings settings {
                this->model_,
                this->serial_number_
            };

            if (!this->pref_.save(&settings)) {
                ESP_LOGW(TAG, "Failed to save settings to flash memory.");
            }
        }

        void TCBusComponent::dump_config()
        {
            ESP_LOGCONFIG(TAG, "TC:BUS:");

            if (strcmp(this->event_, "esphome.none") != 0) {
                ESP_LOGCONFIG(TAG, "  Event: %s", this->event_);
            } else {
                ESP_LOGCONFIG(TAG, "  Event: Disabled");
            }

            ESP_LOGCONFIG(TAG, "  Hardware: %s", this->hardware_version_str_.c_str());

            #ifdef USE_TEXT_SENSOR
            ESP_LOGCONFIG(TAG, "Text Sensors:");
            LOG_TEXT_SENSOR("  ", "Last Bus Command", this->bus_command_text_sensor_);
            LOG_TEXT_SENSOR("  ", "Hardware Version", this->hardware_version_text_sensor_);
            #endif
        }

        void TCBusComponent::loop()
        {
            #ifdef USE_BINARY_SENSOR
            // Turn off binary sensor after ... milliseconds
            uint32_t now_millis = millis();
            for (auto &listener : listeners_) { 
                if (listener->timer_ && now_millis > listener->timer_) {
                    listener->turn_off(&listener->timer_);
                }
            }
            #endif

            // Process command queue
            this->process_command_queue();
        }

        void TCBusComponent::process_command_queue() {
            uint32_t currentTime = millis();

            if (!this->command_queue_.empty()) {
                TCBusCommandQueueItem queue_item = this->command_queue_.front();
        
                if (currentTime - this->last_command_time_ >= queue_item.wait_duration) {
                    // Send command
                    this->transmit_command(queue_item.command_data);

                    // Remove command from the queue
                    this->command_queue_.pop();
        
                    // Update the time of the last command
                    this->last_command_time_ = currentTime;
                }
            }
        }

        void TCBusComponent::received_command(CommandData cmd_data, bool received)
        {
            if(received) {
                // Incoming command

                if(reading_memory_) {
                    ESP_LOGD(TAG, "Received 4 memory blocks from %i to %i | Data: 0x%08X", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4, cmd_data.command);

                    // Save Data to memory Store
                    memory_buffer_.push_back((cmd_data.command >> 24) & 0xFF);
                    memory_buffer_.push_back((cmd_data.command >> 16) & 0xFF);
                    memory_buffer_.push_back((cmd_data.command >> 8) & 0xFF);
                    memory_buffer_.push_back(cmd_data.command & 0xFF);

                    // Next 4 Data Blocks
                    reading_memory_count_++;

                    // Memory reading complete
                    if(reading_memory_count_ == reading_memory_max_) {
                        // Turn off
                        this->cancel_timeout("wait_for_memory_reading");
                        reading_memory_ = false;

                        this->publish_settings();
                        this->read_memory_complete_callback_.call(memory_buffer_);
                    } else {
                        read_memory_block();
                    }
                } else if(identify_model_) {
                    identify_model_ = false;
                    this->cancel_timeout("wait_for_identification_category_0");
                    this->cancel_timeout("wait_for_identification_category_1");

                    ModelData device;
                    device.category = 0;
                    device.memory_size = 0;

                    if(cmd_data.command_hex.substr(4, 1) == "D") {
                        // New models

                        // FW Version
                        device.firmware_version = std::stoi(cmd_data.command_hex.substr(5, 3));
                        device.firmware_major = std::stoi(cmd_data.command_hex.substr(5, 1), nullptr, 16);
                        device.firmware_minor = std::stoi(cmd_data.command_hex.substr(6, 1), nullptr, 16);
                        device.firmware_patch = std::stoi(cmd_data.command_hex.substr(7, 1), nullptr, 16);

                        // HW Version
                        device.hardware_version = std::stoi(cmd_data.command_hex.substr(0, 1));
                        device.model = identifier_string_to_model(cmd_data.command_hex.substr(1, 3), device.hardware_version, device.firmware_version);
                    } else {
                        // Old models
                        if(cmd_data.command == 0x08000040) {
                            // TTC-XX
                            device.model = MODEL_ISH1030;
                        } else if(cmd_data.command == 0x08000048) {
                            // ISH1030
                            device.model = MODEL_ISH1030;
                        }
                    }

                    const char* hw_model = model_to_string(device.model);

                    if(device.model != MODEL_NONE) {
                        ESP_LOGD(TAG, "Identified Hardware: %s (v%i) | Firmware: %i.%i.%i",
                            hw_model, device.hardware_version, device.firmware_major, device.firmware_minor, device.firmware_patch);
                    } else {
                        ESP_LOGE(TAG, "Unable to identify Hardware! Unknown model. Data received: %s", cmd_data.command_hex.c_str());
                    }

                    // Update Model
                    if(device.model != MODEL_NONE && device.model != this->model_) {
                        this->model_ = device.model;
                        #ifdef USE_SELECT
                        if (this->model_select_ != nullptr) {
                            this->model_select_->publish_state(hw_model);
                        }
                        #endif
                        this->save_settings();
                    }

                    if(device.model != MODEL_NONE) {
                        this->identify_complete_callback_.call(device);
                    } else {
                        this->identify_unknown_callback_.call();
                    }
                } else {
                    if(cmd_data.type == COMMAND_TYPE_ACK) {
                        ESP_LOGD(TAG, "Received Acknowledge - Type: %i", cmd_data.payload);
                    } else {
                        ESP_LOGD(TAG, "Received Command - Type: %s | Address: %i | Payload: 0x%X | Serial-Number: %i | Length: %i-bit | Raw Data: 0x%08X", command_type_to_string(cmd_data.type), cmd_data.address, cmd_data.payload, cmd_data.serial_number, (cmd_data.is_long ? 32 : 16), cmd_data.command);
                    }

                    // Fire Callback
                    this->received_command_callback_.call(cmd_data);

                    #ifdef USE_BINARY_SENSOR
                    // Fire Binary Sensors
                    for (auto &listener : listeners_) {
                        // Listener Command lambda or command property when not available
                        uint32_t listener_command = listener->command_.has_value() ? listener->command_.value() : 0;
                        if (listener->command_lambda_.has_value()) {
                            auto optional_value = (*listener->command_lambda_)();
                            if (optional_value.has_value()) {
                                listener_command = optional_value.value();
                            }
                        }

                        // Listener Serial Number or TCS Serial Number when empty
                        uint32_t listener_serial_number = listener->serial_number_.has_value() ? listener->serial_number_.value() : this->serial_number_;
                        
                        // Listener Address lambda or address property when not available
                        uint8_t listener_address = listener->address_.has_value() ? listener->address_.value() : 0;
                        if (listener->address_lambda_.has_value()) {
                            auto optional_value = (*listener->address_lambda_)();
                            if (optional_value.has_value()) {
                                listener_address = optional_value.value();
                            }
                        }

                        // Listener payload lambda or payload property when not available
                        uint32_t listener_payload = listener->payload_.has_value() ? listener->payload_.value() : 0;
                        if (listener->payload_lambda_.has_value()) {
                            auto optional_value = (*listener->payload_lambda_)();
                            if (optional_value.has_value()) {
                                listener_payload = optional_value.value();
                            }
                        }

                        // Listener Command Type
                        CommandType listener_type = listener->type_.has_value() ? listener->type_.value() : COMMAND_TYPE_UNKNOWN;

                        bool allow_publish = false;

                        // Check if listener matches the command
                        if (listener_command != 0) {
                            if (cmd_data.command == listener_command) {
                                allow_publish = true;
                            }
                        } else if (cmd_data.type == listener_type && (cmd_data.address == listener_address || listener_address == 255) && (cmd_data.payload == listener_payload || listener_payload == 255)) {
                            if (listener_serial_number != 0) {
                                if (cmd_data.serial_number == listener_serial_number || listener_serial_number == 255) {
                                    allow_publish = true;
                                }
                            } else {
                                allow_publish = true;  // Accept any serial number
                            }
                        }

                        // Trigger listener binary sensor if match found
                        if (allow_publish) {
                            listener->turn_on(&listener->timer_, listener->auto_off_);
                        }
                    }
                    #endif

                    #ifdef USE_API
                    // Fire Home Assistant Event if event name is specified
                    if (strcmp(event_, "esphome.none") != 0 && cmd_data.type != COMMAND_TYPE_ACK) {
                        auto capi = new esphome::api::CustomAPIDevice();
                        ESP_LOGV(TAG, "Send event to Home Assistant on %s", event_);

                        // Convert type to lowercase
                        std::string type_str = command_type_to_string(cmd_data.type);
                        std::transform(type_str.begin(), type_str.end(), type_str.begin(), ::tolower);

                        capi->fire_homeassistant_event(event_, {
                            {"command", cmd_data.command_hex},
                            {"type", type_str},
                            {"address", std::to_string(cmd_data.address)},
                            {"payload", std::to_string(cmd_data.payload)},
                            {"serial_number", std::to_string(cmd_data.serial_number)}
                        });
                    }
                    #endif
                }

            } else {
                // Outgoing command
                if(cmd_data.type == COMMAND_TYPE_ACK) {
                    ESP_LOGD(TAG, "Sending Acknowledge - Type: %i", cmd_data.payload);
                } else {
                    ESP_LOGD(TAG, "Sending Command - Type: %s | Address: %i | Payload: 0x%X | Serial-Number: %i | Length: %i-bit | Raw Data: 0x%08X", command_type_to_string(cmd_data.type), cmd_data.address, cmd_data.payload, cmd_data.serial_number, (cmd_data.is_long ? 32 : 16), cmd_data.command);
                }
            }

            // No response data
            if(!this-identify_model_ && !this->reading_memory_) {
                // Update Door Readiness Status
                if (cmd_data.type == COMMAND_TYPE_START_TALKING_DOOR_CALL) {
                    bool door_readiness_state = cmd_data.payload == 1;
                    ESP_LOGD(TAG, "Door readiness: %s", YESNO(door_readiness_state));
                } else if (cmd_data.type == COMMAND_TYPE_END_OF_DOOR_READINESS) {
                    ESP_LOGD(TAG, "Door readiness: %s", YESNO(false));
                } else if (cmd_data.type == COMMAND_TYPE_PROGRAMMING_MODE) {
                    ESP_LOGD(TAG, "Programming Mode: %s", YESNO(cmd_data.payload == 1));
                    this->programming_mode_ = cmd_data.payload == 1;
                } else if (cmd_data.type == COMMAND_TYPE_SEARCH_DOORMAN_DEVICES) {
                    ESP_LOGD(TAG, "Responding to Doorman search request.");

                    uint8_t mac[6];
                    get_mac_address_raw(mac);

                    uint32_t mac_addr = 0;
                    mac_addr |= (mac[3] << 16);
                    mac_addr |= (mac[4] << 8);
                    mac_addr |= (mac[5] << 0);

                    send_command(COMMAND_TYPE_FOUND_DOORMAN_DEVICE, 0, mac_addr, 0);
                } else if (cmd_data.type == COMMAND_TYPE_FOUND_DOORMAN_DEVICE) {
                    uint8_t mac[3];
                    mac[0] = (cmd_data.payload >> 16) & 0xFF;
                    mac[1] = (cmd_data.payload >> 8) & 0xFF;
                    mac[2] = cmd_data.payload & 0xFF;

                    ESP_LOGD(TAG, "Discovered Doorman with MAC: %02X:%02X:%02X", mac[0], mac[1], mac[2]);
                }

                #ifdef USE_TEXT_SENSOR
                if(cmd_data.type != COMMAND_TYPE_ACK) {
                    // Publish Command to Last Bus Command Sensor
                    if (this->bus_command_text_sensor_ != nullptr) {
                        this->bus_command_text_sensor_->publish_state(cmd_data.command_hex);
                    }
                }
                #endif
            }
        }

        bool TCBusComponent::on_receive(remote_base::RemoteReceiveData data)
        {
            ESP_LOGV(TAG, "Received raw data (Length: %" PRIi32 ")", data.size());

            bool is_long = false;
            bool command_ready = false;

            uint32_t command = 0;
            uint8_t cmd_pos = 0;
            uint8_t cmd_crc = 0;
            uint8_t cmd_cal_crc = 1;

            uint8_t ack_command = 0;
            uint8_t ack_pos = 0;
            uint8_t ack_crc = 0;
            uint8_t ack_cal_crc = 1;

            // Process each pulse duration in the received data
            for (auto raw_pulse_duration : data.get_raw_data()) {
                uint32_t pulse_duration = std::abs(raw_pulse_duration);
                uint8_t pulse_type = 4;

                // Determine the type of pulse based on its duration
                if (pulse_duration >= BIT_0_MIN && pulse_duration <= BIT_0_MAX) {
                    pulse_type = 0;
                    ESP_LOGV(TAG, "Command Bit (%i), %i", 0, cmd_pos);
                } else if (pulse_duration >= BIT_1_MIN && pulse_duration <= BIT_1_MAX) {
                    pulse_type = 1;
                    ESP_LOGV(TAG, "Command Bit (%i), %i", 1, cmd_pos);
                } else if (pulse_duration >= START_MIN && pulse_duration <= START_MAX) {
                    pulse_type = 2;
                    ESP_LOGV(TAG, "Begin (%i)", pulse_duration, cmd_pos);
                } else {
                    // If the pulse duration does not match any known type, reset the state
                    // Check for ACK
                    if (ack_pos == 6) {
                        if (ack_crc == ack_cal_crc && !reading_memory_ && !identify_model_) {
                            CommandData cmd_data = parseCommand(ack_command, false);
                            received_command(cmd_data);
                        }

                        ack_pos = 0;
                        ack_command = 0;
                        ack_crc = 0;
                        ack_cal_crc = 1;
                    }

                    // Invalid timing, reset state
                    cmd_pos = 0;
                    ESP_LOGV(TAG, "Reset (%i), %i", pulse_duration, cmd_pos);
                    continue;
                }

                // Process acknowledgment bits
                if (pulse_type == 0 || pulse_type == 1) {
                    if (ack_pos == 0) {
                        ack_pos++;
                    } else if (ack_pos >= 1 && ack_pos <= 4) {
                        if (pulse_type) {
                            ack_command |= (1 << (4 - ack_pos));  
                        }
                        ack_cal_crc ^= pulse_type;
                        ack_pos++;
                    } else if (ack_pos == 5) {
                        ack_crc = pulse_type;
                        ack_pos++;
                    }
                } else if (pulse_type == 2) {
                    if (ack_pos == 6) { 
                        if (ack_crc == ack_cal_crc && !reading_memory_ && !identify_model_) {
                            CommandData cmd_data = parseCommand(ack_command, false);
                            received_command(cmd_data);
                        }
                    }

                    ack_pos = 0;
                    ack_command = 0;
                    ack_crc = 0;
                    ack_cal_crc = 1;
                }

                // Process command bits
                if (cmd_pos == 0) {
                    if (pulse_type == 2) {
                        cmd_pos++;
                        command = 0;
                        cmd_crc = 0;
                        cmd_cal_crc = 1;
                        is_long = false;
                    }
                } else if (pulse_type == 0 || pulse_type == 1) {
                    if (cmd_pos == 1) {
                        is_long = pulse_type;
                        cmd_pos++;
                    } else if (cmd_pos >= 2 && cmd_pos <= 17) {
                        if (pulse_type) {
                            command |= (1 << ((is_long ? 33 : 17) - cmd_pos));
                        }
                        cmd_cal_crc ^= pulse_type;
                        cmd_pos++;
                    } else if (cmd_pos == 18) {
                        if (is_long) {
                            if (pulse_type) {
                                command |= (1 << (33 - cmd_pos));
                            }
                            cmd_cal_crc ^= pulse_type;
                            cmd_pos++;
                        } else {
                            cmd_crc = pulse_type;
                            command_ready = true;
                        }
                    } else if (cmd_pos >= 19 && cmd_pos <= 33) {
                        if (pulse_type) {
                            command |= (1 << (33 - cmd_pos));
                        }
                        cmd_cal_crc ^= pulse_type;
                        cmd_pos++;
                    } else if (cmd_pos == 34) {
                        cmd_crc = pulse_type;
                        command_ready = true;
                    }
                } else if (pulse_type == 2) {  // Another START signal
                    // Only reset if we're not in the middle of a valid command
                    if (!command_ready) {
                        cmd_pos = 1;  // Set to 1 since we're starting a new command
                        command = 0;
                        cmd_crc = 0;
                        cmd_cal_crc = 1;
                        is_long = false;
                    }
                } else {
                    cmd_pos = 0;
                    command = 0;
                    cmd_crc = 0;
                    cmd_cal_crc = 1;
                    is_long = false;
                    
                    ack_pos = 0;
                    ack_command = 0;
                    ack_crc = 0;
                    ack_cal_crc = 1;
                }
    
                // If the command is ready, process it
                if (command_ready) {
                    command_ready = false;
    
                    if (cmd_crc == cmd_cal_crc) {
                        if(this->last_sent_command_ == -1) {
                            ESP_LOGV(TAG, "Received data %X, previously sent: NOTHING", command);
                        } else {
                            ESP_LOGV(TAG, "Received data %X, previously sent: %08X", command, this->last_sent_command_);
                        }

                        if(this->last_sent_command_ == -1 || (this->last_sent_command_ != -1 && static_cast<int32_t>(command) != this->last_sent_command_)) {
                            CommandData cmd_data = parseCommand(command, is_long);
                            received_command(cmd_data);
                        } else {
                            ESP_LOGD(TAG, "Received command 0x%08X, but ignoring it as it matches the last sent command.", command);
                        }
                        this->last_sent_command_ = -1;
                    } else {
                        ESP_LOGW(TAG, "CRC mismatch! Received: %d | Expected: %d", cmd_crc, cmd_cal_crc);
                    }
    
                    ack_pos = 0;
                    ack_command = 0;
                    ack_crc = 0;
                    ack_cal_crc = 1;
                    command = 0;
                    cmd_pos = 0;
                }
            }

            return true;
        }

        void TCBusComponent::publish_settings()
        {
            ESP_LOGD(TAG, "Handset volume (Door Call): %i", get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
            ESP_LOGD(TAG, "Handset volume (Internal Call): %i", get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
            ESP_LOGD(TAG, "Ringtone volume: %i", get_setting(SETTING_VOLUME_RINGTONE));

            ESP_LOGD(TAG, "Entrance Door Call Ringtone: %i", get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL));
            ESP_LOGD(TAG, "Second Entrance Door Call Ringtone: %i", get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL));
            ESP_LOGD(TAG, "Floor Call Ringtone: %i", get_setting(SETTING_RINGTONE_FLOOR_CALL));
            ESP_LOGD(TAG, "Internal Call Ringtone: %i", get_setting(SETTING_RINGTONE_INTERNAL_CALL));

            #ifdef USE_SELECT
            if (this->ringtone_entrance_door_call_select_) {
                this->ringtone_entrance_door_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL)));
            }
            if (this->ringtone_second_entrance_door_call_select_) {
                this->ringtone_second_entrance_door_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL)));
            }
            if (this->ringtone_floor_call_select_) {
                this->ringtone_floor_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_FLOOR_CALL)));
            }
            if (this->ringtone_internal_call_select_) {
                this->ringtone_internal_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_INTERNAL_CALL)));
            }
            #endif

            #ifdef USE_NUMBER
            if (this->volume_handset_door_call_number_) {
                this->volume_handset_door_call_number_->publish_state(get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
            }
            if (this->volume_handset_internal_call_number_) {
                this->volume_handset_internal_call_number_->publish_state(get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
            }
            if (this->volume_ringtone_number_) {
                this->volume_ringtone_number_->publish_state(get_setting(SETTING_VOLUME_RINGTONE));
            }
            #endif
        }

        #ifdef USE_BINARY_SENSOR
        void TCBusComponent::register_listener(TCBusListener *listener)
        {
            this->listeners_.push_back(listener); 
        }
        #endif

        void TCBusComponent::send_command(uint32_t command, uint32_t wait_duration)
        {
            // Determine length of command
            // Not reliable as its based on the 32 bit integer itself
            bool is_long = (command > 0xFFFF);

            CommandData cmd_data = parseCommand(command, is_long);
            send_command(cmd_data, wait_duration);
        }

        void TCBusComponent::send_command(uint32_t command, bool is_long, uint32_t wait_duration)
        {
            CommandData cmd_data = parseCommand(command, is_long);
            send_command(cmd_data, wait_duration);
        }

        void TCBusComponent::send_command(CommandType type, uint8_t address, uint32_t payload, uint32_t serial_number, uint32_t wait_duration)
        {
            CommandData cmd_data = buildCommand(type, address, payload, serial_number);
            send_command(cmd_data, wait_duration);
        }

        void TCBusComponent::send_command(CommandData cmd_data, uint32_t wait_duration)
        {
            if(cmd_data.serial_number == 0 && this->serial_number_ != 0) {
                cmd_data.serial_number = this->serial_number_;
                ESP_LOGV(TAG, "Serial number is not specified. Using saved serial number: %i", cmd_data.serial_number);
            }

            if(cmd_data.command == 0) {
                ESP_LOGW(TAG, "Sending commands of type %s is not yet supported.", command_type_to_string(cmd_data.type));
            }

            this->command_queue_.push({cmd_data, wait_duration});
        }

        void TCBusComponent::transmit_command(CommandData cmd_data)
        {
            this->received_command(cmd_data, false);

            this->last_sent_command_ = cmd_data.command;

            auto call = id(this->tx_).transmit();
            remote_base::RemoteTransmitData *dst = call.get_data();

            // Start transmission with initial mark and space
            dst->mark(cmd_data.type == COMMAND_TYPE_ACK ? BUS_ACK_START_MS : BUS_CMD_START_MS);
            ESP_LOGV(TAG, "mark start %i", BUS_CMD_START_MS);
            dst->space(cmd_data.is_long ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);
            ESP_LOGV(TAG, "space lngth %i", cmd_data.is_long ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);

            // Calculate length based on command type
            uint8_t length = cmd_data.type == COMMAND_TYPE_ACK ? 4 : (cmd_data.is_long ? 32 : 16);
            
            // Track checksum
            uint8_t checksm = 1;

            // Process all bits
            for (int i = length - 1; i >= 0; i--) {
                // Extract single bit
                bool bit = (cmd_data.command & (1UL << i)) != 0;
                
                // Update checksum
                checksm ^= bit;

                // Send bit as mark/space sequence
                if(i % 2 == 0) {
                    dst->space(bit ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);
                    ESP_LOGV(TAG, "space bit %i - %i", bit, bit ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);
                } else {
                    dst->mark(bit ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);
                    ESP_LOGV(TAG, "mark bit %i - %i", bit, bit ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);
                }
            }
            
            dst->mark(checksm ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);
            ESP_LOGV(TAG, "mark chksm %i", checksm ? BUS_ONE_BIT_MS : BUS_ZERO_BIT_MS);
            
            call.perform();
            ESP_LOGV(TAG, "perform");
        }

        void TCBusComponent::add_received_command_callback(std::function<void(CommandData)> &&callback)
        {
            this->received_command_callback_.add(std::move(callback));
        }
        
        void TCBusComponent::add_read_memory_complete_callback(std::function<void(std::vector<uint8_t>)> &&callback)
        {
            this->read_memory_complete_callback_.add(std::move(callback));
        }

        void TCBusComponent::add_read_memory_timeout_callback(std::function<void()> &&callback)
        {
            this->read_memory_timeout_callback_.add(std::move(callback));
        }

        void TCBusComponent::add_identify_complete_callback(std::function<void(ModelData)> &&callback)
        {
            this->identify_complete_callback_.add(std::move(callback));
        }

        void TCBusComponent::add_identify_unknown_callback(std::function<void()> &&callback)
        {
            this->identify_unknown_callback_.add(std::move(callback));
        }

        void TCBusComponent::add_identify_timeout_callback(std::function<void()> &&callback)
        {
            this->identify_timeout_callback_.add(std::move(callback));
        }

        void TCBusComponent::set_programming_mode(bool enabled)
        {
            send_command(COMMAND_TYPE_PROGRAMMING_MODE, 0, enabled ? 1 : 0);
        }

        void TCBusComponent::request_version(uint32_t serial_number)
        {
            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
                ESP_LOGV(TAG, "Serial number is not specified. Using saved serial number: %i", serial_number);
            } else {
                ESP_LOGE(TAG, "Device model cannot be identified without a serial number!");
                return;
            }

            this->cancel_timeout("wait_for_identification_category_0");
            this->cancel_timeout("wait_for_identification_category_1");
            this->identify_model_ = true;

            // First try with category 0
            ESP_LOGD(TAG, "Identifying device model (Category %i) using serial number: %i...", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, 0, 0, 400); // class 0
            send_command(COMMAND_TYPE_REQUEST_VERSION, 0, 0, serial_number, 400);

            this->set_timeout("wait_for_identification_category_0", 1000, [this, serial_number]() {
                // Didn't receive identify result of category 0
                // Second try with category 1
                ESP_LOGD(TAG, "Identifying device model (Category %i) using serial number: %i...", 1, serial_number);
                send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, 1, 0, 400); // class 1
                send_command(COMMAND_TYPE_REQUEST_VERSION, 0, 0, serial_number, 400);

                this->set_timeout("wait_for_identification_category_1", 1000, [this]() {
                    // Didn't receive identify result of category 1
                    // Failed
                    this->identify_model_ = false;
                    this->identify_timeout_callback_.call();
                    ESP_LOGE(TAG, "Identification response not received in time. The device model may not support identification.");
                });
            });
        }

        void TCBusComponent::read_memory(uint32_t serial_number, Model model)
        {
            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
                ESP_LOGV(TAG, "Serial number is not specified. Using saved serial number: %i", serial_number);
            } else {
                ESP_LOGE(TAG, "Unable to read device memory without a serial number!");
                return;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
                ESP_LOGV(TAG, "Model is not specified (NONE). Using saved model: %s.", model_to_string(model));
            } else {
                ESP_LOGE(TAG, "Unable to read device memory without a model!");
                return;
            }

            ESP_LOGD(TAG, "Reading EEPROM for model %s (Serial: %i)...", model_to_string(model), serial_number);

            this->cancel_timeout("wait_for_memory_reading");
            reading_memory_ = false;

            ModelData model_data = getModelData(model);

            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, model_data.category);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);

            memory_buffer_.clear();
            reading_memory_ = true;
            reading_memory_serial_number_ = serial_number;
            reading_memory_count_ = 0;
            reading_memory_max_ = (model_data.memory_size / 4);

            this->set_timeout("wait_for_memory_reading", reading_memory_max_ * 600, [this]() {
                memory_buffer_.clear();
                reading_memory_ = false;
                reading_memory_serial_number_ = 0;
                reading_memory_count_ = 0;
                reading_memory_max_ = 0;

                this->read_memory_timeout_callback_.call();
                ESP_LOGE(TAG, "Memory block not received in time. Reading canceled!");
            });
                
            read_memory_block();
        }

        void TCBusComponent::read_memory_block()
        {
            ESP_LOGD(TAG, "Read 4 memory blocks, from %i to %i.", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);
            send_command(COMMAND_TYPE_READ_MEMORY_BLOCK, reading_memory_count_, 0, 0, 300);
        }

        bool TCBusComponent::supports_setting(SettingType type, Model model)
        {
            if(memory_buffer_.size() == 0) {
                return false;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
                ESP_LOGV(TAG, "Model is not specified (NONE). Using saved model: %s.", model_to_string(model));
            } else {
                return false;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, model);

            if(cellData.index != 0) {
                return true;
            } else {
                return false;
            }
        }

        uint8_t TCBusComponent::get_setting(SettingType type, Model model)
        {
            if(memory_buffer_.size() == 0) {
                return 0;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
                ESP_LOGV(TAG, "Model is not specified (NONE). Using saved model: %s.", model_to_string(model));
            } else {
                ESP_LOGV(TAG, "Model is not specified (NONE). Returning 0.");
                return 0;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, model);

            if(cellData.index != 0) {
                return cellData.left_nibble ? ((memory_buffer_[cellData.index] >> 4) & 0xF) : (memory_buffer_[cellData.index] & 0xF);
            } else {
                ESP_LOGV(TAG, "The setting '%s' is not available for model '%s'.", setting_type_to_string(type), model_to_string(model));
                return 0;
            }
        }

        bool TCBusComponent::update_setting(SettingType type, uint8_t new_value, uint32_t serial_number, Model model)
        {
            if(memory_buffer_.size() == 0) {
                ESP_LOGE(TAG, "Memory buffer is empty. Please read memory before proceeding!");
                return false;
            }

            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
                ESP_LOGV(TAG, "Serial number is not specified. Using saved serial number: %i", serial_number);
            } else {
                ESP_LOGE(TAG, "Unable to update device setting without a serial number!");
                return false;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
                ESP_LOGV(TAG, "Model is not specified (NONE). Using saved model: %s.", model_to_string(model));
            } else {
                ESP_LOGE(TAG, "Unable to update device setting without a model!");
                return false;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, model);
            if(cellData.index == 0) {
                ESP_LOGE(TAG, "Cannot write because setting %s is not available for model %s!", setting_type_to_string(type), model_to_string(model));
                return false;
            }

            ESP_LOGD(TAG, "Writing setting %s (%X) to EEPROM of %s (%i)...", setting_type_to_string(type), new_value, model_to_string(model), serial_number);
            // Apply new nibble and keep other nibble
            uint8_t saved_nibble = (cellData.left_nibble ? memory_buffer_[cellData.index] : (memory_buffer_[cellData.index] >> 4)) & 0xF;
            memory_buffer_[cellData.index] = cellData.left_nibble ? ((new_value << 4) | (saved_nibble & 0xF)) : ((saved_nibble << 4) | (new_value & 0xF));

            // Prepare Transmission
            // Select device category
            ModelData model_data = getModelData(model);
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, model_data.category);

            // Select memory page %i of serial number %i
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);

            // Transfer new settings value to memory
            uint16_t new_values = (memory_buffer_[cellData.index] << 8) | memory_buffer_[cellData.index + 1];
            send_command(COMMAND_TYPE_WRITE_MEMORY, cellData.index, new_values);

            // Reset
            send_command(COMMAND_TYPE_RESET);

            return true;
        }

        bool TCBusComponent::write_memory(uint32_t serial_number, Model model)
        {
            if(memory_buffer_.size() == 0) {
                ESP_LOGE(TAG, "Memory buffer is empty! Please read memory first before proceeding.");
                return false;
            }

            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
                ESP_LOGV(TAG, "Serial number is not specified. Using saved serial number: %i", serial_number);
            } else {
                ESP_LOGE(TAG, "Unable to write memory without a serial number!");
                return false;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
                ESP_LOGV(TAG, "Model is not specified (NONE). Using saved model: %s.", model_to_string(model));
            } else {
                ESP_LOGE(TAG, "Unable to write memory without a model!");
                return false;
            }

            ESP_LOGD(TAG, "Write memory buffer to EEPROM of %s (%i)...", model_to_string(model), serial_number);

            // Prepare Transmission
            //ESP_LOGD(TAG, "Select device category");
            ModelData model_data = getModelData(model);
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, model_data.category);

            //ESP_LOGD(TAG, "Select memory page %i of serial number %i", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);

            // Transmit Memory
            for (size_t address = 0; address < memory_buffer_.size(); address += 2) {
                uint16_t new_value = (memory_buffer_[address] << 8) | memory_buffer_[address + 1];
                send_command(COMMAND_TYPE_WRITE_MEMORY, address, new_value);
            }

            // Reset
            send_command(COMMAND_TYPE_RESET);

            return true;
        }

    }  // namespace tc_bus
}  // namespace esphome