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


namespace esphome
{
    namespace tc_bus
    {
        static const char *const TAG = "tc_bus";

        uint32_t global_tcs_id = 1911044085ULL;

        static const uint32_t TCS_MSG_START_MS = 6000; // a new message
        static const uint32_t TCS_ONE_BIT_MS = 4000; // a 1-bit is 4ms long
        static const uint32_t TCS_ZERO_BIT_MS = 2000; // a 0-bit is 2ms long

        static const uint8_t TCS_SEND_WAIT_DURATION = 50;
        static const uint8_t TCS_SEND_WAIT_TIMEOUT_MS = 100;
        static const uint8_t TCS_SEND_MIN_DELAY_MS = 20;
        static const uint8_t TCS_SEND_MAX_DELAY_MS = 50;

        TCBusComponent *global_tc_bus = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        void TCBusComponent::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up TC:BUS Intercom...");

            this->pref_ = global_preferences->make_preference<TCBusSettings>(global_tcs_id);
            
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
            ESP_LOGD(TAG, "Check for Doorman Hardware");

            // Doorman Hardware Revision
            uint8_t ver[3];
            uint32_t value;
            esp_efuse_read_block(EFUSE_BLK3, &value, 0, 24);
            ver[0] = value >> 0;
            ver[1] = value >> 8;
            ver[2] = value >> 16;

            if(ver[0] > 0) {
                ESP_LOGI(TAG, "Doorman Hardware detected: V%i.%i.%i", ver[0], ver[1], ver[2]);
                this->hardware_version_str_ = "Doorman-S3 " + std::to_string(ver[0]) + "." + std::to_string(ver[1]) + "." + std::to_string(ver[2]);

                // Override GPIO
                /*if(ver[0] == 1 && (ver[1] == 3 || ver[1] == 4 || ver[1] == 5)) {
                    esp32::ESP32InternalGPIOPin *gpio_pin_rx_;
                    gpio_pin_rx_ = new(esp32::ESP32InternalGPIOPin);
                    gpio_pin_rx_->set_pin(static_cast<gpio_num_t>(9));
                    gpio_pin_rx_->set_flags(gpio::Flags::FLAG_INPUT);
                    this->set_rx_pin(gpio_pin_rx_);

                    esp32::ESP32InternalGPIOPin *gpio_pin_tx_;
                    gpio_pin_tx_ = new(esp32::ESP32InternalGPIOPin);
                    gpio_pin_tx_->set_pin(static_cast<gpio_num_t>(8));
                    gpio_pin_tx_->set_inverted(false); 
                    gpio_pin_tx_->set_flags(gpio::Flags::FLAG_OUTPUT);
                    gpio_pin_tx_->set_drive_strength(GPIO_DRIVE_CAP_DEFAULT);
                    gpio_pin_tx_->digital_write(false);
                    this->set_tx_pin(gpio_pin_tx_);

                    ESP_LOGD(TAG, "Doorman Hardware GPIO Override: RX (%i), TX (%i)", this->rx_pin_->get_pin(), this->tx_pin_->get_pin());
                }*/
            }
            #endif

            #ifdef USE_TEXT_SENSOR
            if (this->hardware_version_text_sensor_ != nullptr) {
                this->hardware_version_text_sensor_->publish_state(this->hardware_version_str_);
            }
            #endif

            this->rx_->register_listener(this);

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
        }

        void TCBusComponent::save_settings()
        {
            TCBusSettings settings {
                this->model_,
                this->serial_number_
            };

            if (!this->pref_.save(&settings)) {
                ESP_LOGW(TAG, "Failed to save settings");
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
        }

        void TCBusComponent::on_command(CommandData cmd_data)
        {
            ESP_LOGD(TAG, "[Received] Type: %s, Address: %i, Payload: %X, Serial: %i, Length: %i-bit", command_type_to_string(cmd_data.type), cmd_data.address, cmd_data.payload, cmd_data.serial_number, (cmd_data.is_long ? 32 : 16));
        
            if(reading_memory_) {
                ESP_LOGD(TAG, "Received 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);

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
                    delay(20);

                    // Request Data Blocks
                    ESP_LOGD(TAG, "Read 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);
                    send_command(COMMAND_TYPE_READ_MEMORY_BLOCK, reading_memory_count_);
                }
            }
            else if(identify_model_) {
                std::string hex_result = str_upper_case(cmd_data.command_hex);
                if(hex_result.substr(4, 1) == "D") {
                    identify_model_ = false;
                    this->cancel_timeout("wait_for_identification");

                    ModelData device;

                    device.category = 0;
                    device.memory_size = 0;

                    // FW Version
                    device.firmware_version = std::stoi(hex_result.substr(5, 3));
                    device.firmware_major = std::stoi(hex_result.substr(5, 1), nullptr, 16);
                    device.firmware_minor = std::stoi(hex_result.substr(6, 1), nullptr, 16);
                    device.firmware_patch = std::stoi(hex_result.substr(7, 1), nullptr, 16);

                    // HW Version
                    device.hardware_version = std::stoi(hex_result.substr(0, 1));
                    device.model = identifier_string_to_model(hex_result.substr(1, 3), device.hardware_version, device.firmware_version);
                    const char* hw_model = model_to_string(device.model);

                    ESP_LOGD(TAG, "Identified Hardware: %s (version %i), Firmware: %i.%i.%i - %i",
                        hw_model, device.hardware_version, device.firmware_major, device.firmware_minor, device.firmware_patch, device.firmware_version);

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

                    this->identify_complete_callback_.call(device);
                } else {
                    ESP_LOGE(TAG, "Invalid indentification response!");
                }
            } else {
                if(cmd_data.is_long) {
                    ESP_LOGD(TAG, "Received 32-Bit command %08X", cmd_data.command);
                } else {
                    ESP_LOGD(TAG, "Received 16-Bit command %04X", cmd_data.command);
                }
                this->publish_command(cmd_data.command, cmd_data.is_long, true);
            }
        }

        bool TCBusComponent::on_receive(remote_base::RemoteReceiveData data)
        {
            ESP_LOGD(TAG, "Received raw data with length %" PRIi32, data.size());

            uint32_t command = 0;
            uint8_t curPos = 0;
            uint8_t curCRC = 0;
            uint8_t calCRC = 1;

            uint8_t ackBits = 0;
            uint8_t ackCommand = 0;
            uint8_t ackCRC = 0;
            uint8_t ackCalCRC = 1;

            bool curIsLong = false;
            bool cmdIntReady = false;

            for (auto pulse_duration : data.get_raw_data()) {
                uint32_t abs_duration = std::abs(pulse_duration); // Normalize duration

                // Determine bit value based on absolute duration thresholds
                uint8_t curBit = 4;  // Undefined by default
                if (abs_duration >= BIT_0_MIN && abs_duration <= BIT_0_MAX) {
                    curBit = 0;
                    ESP_LOGD(TAG, "Command Bit %i", 0);
                } else if (abs_duration >= BIT_1_MIN && abs_duration <= BIT_1_MAX) {
                    curBit = 1;
                    ESP_LOGD(TAG, "Command Bit %i", 1);
                } else if (abs_duration >= START_MIN && abs_duration <= START_MAX) {
                    curBit = 2;
                    ESP_LOGD(TAG, "Begin %i", abs_duration);
                } else {
                    // Reset state for invalid bit and check for ACK
                    
                    //ESP_LOGD(TAG, "ackBits %i", ackBits);

                    if (ackBits == 6) {
                        if (ackCRC == ackCalCRC) {
                            ESP_LOGI(TAG, "ACK: %01X", ackCommand);
                        }
                        ackBits = 0;
                        ackCommand = 0;
                        ackCRC = 0;
                        ackCalCRC = 1;
                    }

                    curPos = 0;
                    ESP_LOGD(TAG, "Reset %i", abs_duration);
                    continue;  // Invalid timing, reset state
                }

                // Store bit for potential ACK command
                if (curBit == 0 || curBit == 1) {
                    if (ackBits == 0) {  // Length bit
                        ackBits++;
                        //ESP_LOGD(TAG, "ACK length bit: %d", curBit);
                    } else if (ackBits >= 1 && ackBits <= 4) {  // 4 data bits
                        if (curBit) {
                            ackCommand |= (1 << (4 - ackBits));  // Store bits from MSB to LSB
                        }
                        ackCalCRC ^= curBit;
                        ackBits++;
                        //ESP_LOGD(TAG, "ACK data bit %d: %d, command now: %02X", ackBits-1, curBit, ackCommand);
                    } else if (ackBits == 5) {  // CRC bit
                        ackCRC = curBit;
                        ackBits++;
                        //ESP_LOGD(TAG, "ACK CRC bit: %d, command: %02X", curBit, ackCommand);
                    }
                } else if (curBit == 2) {  // Start bit - check and reset ACK tracking
                    if (ackBits == 6) {  // Check if we have a complete ACK
                        if (ackCRC == ackCalCRC) {
                            ESP_LOGI(TAG, "ACK: %01X", ackCommand);
                        }
                    }
                    // Reset ACK tracking on new command
                    ackBits = 0;
                    ackCommand = 0;
                    ackCRC = 0;
                    ackCalCRC = 1;
                }

                if (curPos == 0) {
                    if (curBit == 2) {
                        curPos++;
                    }
                    command = 0;
                    curCRC = 0;
                    calCRC = 1;
                    curIsLong = false;
                } else if (curBit == 0 || curBit == 1) {
                    if (curPos == 1) {
                        curIsLong = curBit;
                        curPos++;
                    } else if (curPos >= 2 && curPos <= 17) {
                        if (curBit) {
                            command |= (1 << ((curIsLong ? 33 : 17) - curPos));
                        }
                        calCRC ^= curBit;
                        curPos++;
                    } else if (curPos == 18) {
                        if (curIsLong) {
                            if (curBit) {
                                command |= (1 << (33 - curPos));
                            }
                            calCRC ^= curBit;
                            curPos++;
                        } else {
                            curCRC = curBit;
                            cmdIntReady = true;
                        }
                    } else if (curPos >= 19 && curPos <= 33) {
                        if (curBit) {
                            command |= (1 << (33 - curPos));
                        }
                        calCRC ^= curBit;
                        curPos++;
                    } else if (curPos == 34) {
                        curCRC = curBit;
                        cmdIntReady = true;
                    }
                } else {
                    curPos = 0;
                    ackBits = 0;
                    ackCommand = 0;
                    ackCRC = 0;
                    ackCalCRC = 1;
                }
    
                if (cmdIntReady) {
                    cmdIntReady = false;
    
                    if (curCRC == calCRC) {
                        //ESP_LOGI(TAG, "CMD received: %08X", command);
                        CommandData cmd_data = parseCommand(command, curIsLong);
                        on_command(cmd_data);
                    } else {
                        ESP_LOGW(TAG, "CRC mismatch: Received %d, Calculated %d", curCRC, calCRC);
                    }
    
                    ackBits = 0;
                    ackCommand = 0;
                    ackCRC = 0;
                    ackCalCRC = 1;

                    command = 0;
                    curPos = 0;
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

        void TCBusComponent::publish_command(uint32_t command, bool is_long, bool received)
        {
            // Parse Command
            CommandData cmd_data = parseCommand(command, is_long);
            ESP_LOGD(TAG, "[Parsed] Type: %s, Address: %i, Payload: %X, Serial: %i, Length: %i-bit", command_type_to_string(cmd_data.type), cmd_data.address, cmd_data.payload, cmd_data.serial_number, (is_long ? 32 : 16));

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
                ESP_LOGD(TAG, "Replying to Doorman search request");

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

                ESP_LOGD(TAG, "Found a Doorman device (%02x:%02x:%02x)", mac[0], mac[1], mac[2]);
            }

            #ifdef USE_TEXT_SENSOR
            // Publish Command to Last Bus Command Sensor
            if (this->bus_command_text_sensor_ != nullptr) {
                this->bus_command_text_sensor_->publish_state(cmd_data.command_hex);
            }
            #endif

            // If the command was received, notify the listeners
            if (received) {
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
                if (strcmp(event_, "esphome.none") != 0) {
                    auto capi = new esphome::api::CustomAPIDevice();
                    ESP_LOGD(TAG, "Send event to Home Assistant on %s", event_);
                    capi->fire_homeassistant_event(event_, {
                        {"command", cmd_data.command_hex},
                        {"type", command_type_to_string(cmd_data.type)},
                        {"address", std::to_string(cmd_data.address)},
                        {"payload", std::to_string(cmd_data.payload)},
                        {"serial_number", std::to_string(cmd_data.serial_number)}
                    });
                }
                #endif
            }
        }

        void TCBusComponent::send_command(CommandType type, uint8_t address, uint32_t payload, uint32_t serial_number)
        {
            ESP_LOGV(TAG, "Generating command: Type: %s, Address: %i, Payload: %X, Serial number: %i", command_type_to_string(type), address, payload, serial_number);

            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
                ESP_LOGV(TAG, "Serial Number is 0, use intercom serial number: %i", serial_number);
            } else {
                ESP_LOGW(TAG, "Serial number is not set!");
            }

            CommandData command_data = buildCommand(type, address, payload, serial_number);
            if(command_data.command == 0) {
                ESP_LOGW(TAG, "Sending commands of type %s is not supported!", command_type_to_string(type));
            } else {
                send_command(command_data.command, command_data.is_long);
            }
        }

        void TCBusComponent::send_command(uint32_t command)
        {
            // Determine length of command
            // not so reliable as its based on the 32 bit integer itself
            send_command(command, (command > 0xFFFF));
        }

        void TCBusComponent::send_command(uint32_t command, bool is_long)
        {
            if(is_long) {
                ESP_LOGD(TAG, "Sending 32-bit command %08X", command);
            } else {
                ESP_LOGD(TAG, "Sending 16-bit command %04X", command);
            }

            if (this->sending) {
                ESP_LOGD(TAG, "Sending of command %08X cancelled, another sending is in progress", command);
                return;
            }

            auto call = id(this->tx_).transmit();
            remote_base::RemoteTransmitData *dst = call.get_data();

            // Initial space to bring line low
            dst->item(0, TCS_MSG_START_MS);
        
            // Start mark and length bit
            dst->item(TCS_MSG_START_MS, is_long ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);
            
            // Track checksum
            uint8_t checksm = 1;
            checksm ^= is_long ? 1 : 0;
            
            // Process all bits except the last one
            for (int i = length - 1; i > 0; i -= 2) {
                uint32_t mask1 = 1UL << i;
                uint32_t mask2 = 1UL << (i-1);
                
                bool bit1 = (command & mask1) != 0;
                bool bit2 = (command & mask2) != 0;
                
                checksm ^= bit1;
                checksm ^= bit2;

                dst->item(
                    bit1 ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS,
                    bit2 ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS
                );
            }

            // Handle last bit + checksum if length is odd
            if (length % 2 != 0) {
                bool last_bit = (command & 1) != 0;
                checksm ^= last_bit;
                
                dst->item(
                    last_bit ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS,
                    checksm ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS
                );
            }

            // End transmission
            dst->item(0, 0);
            call.perform();






            /*// Prevent collisions
            std::srand(millis());
            uint32_t delay_time = std::rand() % (TCS_SEND_MAX_DELAY_MS - TCS_SEND_MIN_DELAY_MS + 1) + TCS_SEND_MIN_DELAY_MS;
            uint32_t start_wait = millis();

            delay(delay_time);

            while((millis() - this->store_.last_bit_change) < TCS_SEND_WAIT_DURATION)
            {
                // Add timeout protection
                if((millis() - start_wait) > TCS_SEND_WAIT_TIMEOUT_MS) {
                    break;
                }
                delay(delay_time);
            }

            // Pause reading
            ESP_LOGV(TAG, "Pause reading");
            this->rx_pin_->detach_interrupt();

            // Source: https://github.com/atc1441/TCSintercomArduino
            this->sending = true;

            uint8_t checksm = 1;
            bool output_state = false;

            // Start transmission
            this->tx_pin_->digital_write(true);
            delay(TCS_MSG_START_MS);

            this->tx_pin_->digital_write(false);
            delay(is_long ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);

            int curBit = 0;
            uint8_t length = is_long ? 32 : 16;

            for (uint8_t i = length; i > 0; i--) {
                curBit = BIT_READ(command, i - 1);

                output_state = !output_state;
                this->tx_pin_->digital_write(output_state);
                delay(curBit ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);
                checksm ^= curBit;
            }

            this->tx_pin_->digital_write(!output_state);
            delay(checksm ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);
            this->tx_pin_->digital_write(false);

            this->sending = false;

            // Resume reading
            ESP_LOGV(TAG, "Resume reading");
            this->rx_pin_->attach_interrupt(TCBusComponentStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);

            // Publish received Command on Sensors, Events, etc.
            this->publish_command(command, is_long, false);
            */
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
            ESP_LOGD(TAG, "Identifying model of device with serial number: %i...", serial_number);

            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
            } else {
                ESP_LOGW(TAG, "Serial number is not set!");
                return;
            }

            this->cancel_timeout("wait_for_identification");

            identify_model_ = true;

            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, 0); // class 0
            delay(50);
            send_command(COMMAND_TYPE_REQUEST_VERSION, 0, 0, serial_number);
            delay(200);
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, 1); // class 1
            delay(50);
            send_command(COMMAND_TYPE_REQUEST_VERSION, 0, 0, serial_number);

            this->set_timeout("wait_for_identification", 600, [this]() {
                identify_model_ = false;
                this->identify_timeout_callback_.call();
                ESP_LOGE(TAG, "Reading version timed out!");
            });
        }

        void TCBusComponent::read_memory(uint32_t serial_number, Model model)
        {
            ESP_LOGD(TAG, "Reading EEPROM of %s (%i)...", model_to_string(model), serial_number);

            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
            } else {
                ESP_LOGW(TAG, "Serial number is not set!");
                return;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
            } else {
                ESP_LOGW(TAG, "Model is not set!");
                return;
            }

            this->cancel_timeout("wait_for_memory_reading");
            reading_memory_ = false;

            ModelData model_data = getModelData(model);

            ESP_LOGD(TAG, "Select device category");
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, model_data.category);
            delay(50);

            ESP_LOGD(TAG, "Select memory page %i of serial number %i", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
            delay(50);

            memory_buffer_.clear();
            reading_memory_ = true;
            reading_memory_serial_number_ = serial_number;
            reading_memory_count_ = 0;
            reading_memory_max_ = (model_data.memory_size / 4);

            this->set_timeout("wait_for_memory_reading", 5000, [this]() {
                memory_buffer_.clear();
                reading_memory_ = false;
                reading_memory_serial_number_ = 0;
                reading_memory_count_ = 0;
                reading_memory_max_ = 0;

                this->read_memory_timeout_callback_.call();
                ESP_LOGE(TAG, "Reading memory timed out!");
            });
                
            ESP_LOGD(TAG, "Read 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);
            send_command(COMMAND_TYPE_READ_MEMORY_BLOCK, reading_memory_count_);
        }

        uint8_t TCBusComponent::get_setting(SettingType type, Model model)
        {
            if(memory_buffer_.size() == 0) {
                return 0;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
            } else {
                ESP_LOGW(TAG, "Model is not set!");
                return 0;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, model);

            if(cellData.index != 0) {
                return cellData.left_nibble ? ((memory_buffer_[cellData.index] >> 4) & 0xF) : (memory_buffer_[cellData.index] & 0xF);
            } else {
                ESP_LOGV(TAG, "The setting %s is not available for model %s", setting_type_to_string(type), model_to_string(model));
                return 0;
            }
        }

        bool TCBusComponent::update_setting(SettingType type, uint8_t new_value, uint32_t serial_number, Model model)
        {
            ESP_LOGD(TAG, "Write setting %s (%X) to EEPROM of %s (%i)...", setting_type_to_string(type), new_value, model_to_string(model), serial_number);

            if(memory_buffer_.size() == 0) {
                ESP_LOGW(TAG, "Memory buffer is empty! Please read memory first!");
                return false;
            }

            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
            } else {
                ESP_LOGW(TAG, "Serial number is not set!");
                return false;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
            } else {
                ESP_LOGW(TAG, "Model is not set!");
                return false;
            }

            uint8_t saved_nibble = 0;

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type, model);

            if(cellData.index != 0) {
                // Apply new nibble and keep other nibble
                saved_nibble = (cellData.left_nibble ? memory_buffer_[cellData.index] : (memory_buffer_[cellData.index] >> 4)) & 0xF;
                memory_buffer_[cellData.index] = cellData.left_nibble ? ((new_value << 4) | (saved_nibble & 0xF)) : ((saved_nibble << 4) | (new_value & 0xF));

                // Prepare Transmission
                ESP_LOGD(TAG, "Select device category");
                ModelData model_data = getModelData(model);
                send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, model_data.category);
                delay(50);

                ESP_LOGD(TAG, "Select memory page %i of serial number %i", 0, serial_number);
                send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
                delay(50);

                // Transfer new settings value to memory
                uint16_t new_values = (memory_buffer_[cellData.index] << 8) | memory_buffer_[cellData.index + 1];
                send_command(COMMAND_TYPE_WRITE_MEMORY, cellData.index, new_values);
                delay(50);

                // Reset
                send_command(COMMAND_TYPE_RESET);

                return true;
            } else {
                ESP_LOGV(TAG, "The setting %s is not available for model %s", setting_type_to_string(type), model_to_string(model));
                return false;
            }
        }

        bool TCBusComponent::write_memory(uint32_t serial_number, Model model)
        {
            ESP_LOGD(TAG, "Write memory buffer to EEPROM of %s (%i)...", model_to_string(model), serial_number);

            if(memory_buffer_.size() == 0) {
                ESP_LOGW(TAG, "Memory buffer is empty! Please read memory first!");
                return false;
            }

            if(serial_number == 0 && this->serial_number_ != 0) {
                serial_number = this->serial_number_;
            } else {
                ESP_LOGW(TAG, "Serial number is not set!");
                return false;
            }

            if(model == MODEL_NONE && this->model_ != MODEL_NONE) {
                model = this->model_;
            } else {
                ESP_LOGW(TAG, "Model is not set!");
                return false;
            }

            // Prepare Transmission
            ESP_LOGD(TAG, "Select device category");
            ModelData model_data = getModelData(model);
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, model_data.category);
            delay(50);

            ESP_LOGD(TAG, "Select memory page %i of serial number %i", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
            delay(50);

            // Transmit Memory
            for (size_t address = 0; address < memory_buffer_.size(); address += 2) {
                uint16_t new_value = (memory_buffer_[address] << 8) | memory_buffer_[address + 1];
                send_command(COMMAND_TYPE_WRITE_MEMORY, address, new_value);
                delay(50);
            }

            // Reset
            send_command(COMMAND_TYPE_RESET);

            return true;
        }

    }  // namespace tc_bus
}  // namespace esphome