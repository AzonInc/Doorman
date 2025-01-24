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

using namespace esphome;

namespace esphome
{
    namespace tc_bus
    {
        static const char *const TAG = "tc_bus";

        uint32_t global_tcs_id = 1911044085ULL;

        static const uint8_t TCS_MSG_START_MS = 6; // a new message
        static const uint8_t TCS_ONE_BIT_MS = 4; // a 1-bit is 4ms long
        static const uint8_t TCS_ZERO_BIT_MS = 2; // a 0-bit is 2ms long
        static const uint8_t TCS_SEND_WAIT_DURATION = 50;

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
                if(ver[0] == 1 && (ver[1] == 3 || ver[1] == 4 || ver[1] == 5)) {
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
                }
            }
            #endif

            #ifdef USE_TEXT_SENSOR
            if (this->hardware_version_text_sensor_ != nullptr) {
                this->hardware_version_text_sensor_->publish_state(this->hardware_version_str_);
            }
            #endif

            this->rx_pin_->setup();
            this->tx_pin_->setup();
            this->tx_pin_->digital_write(false);
            
            auto &s = this->store_;

            s.rx_pin = this->rx_pin_->to_isr();
            this->rx_pin_->attach_interrupt(TCBusComponentStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);

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
            LOG_PIN("  Pin RX: ", this->rx_pin_);
            LOG_PIN("  Pin TX: ", this->tx_pin_);

            if (strcmp(this->event_, "esphome.none") != 0) {
                ESP_LOGCONFIG(TAG, "  Event: %s", this->event_);
            } else {
                ESP_LOGCONFIG(TAG, "  Event: disabled");
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

            auto &s = this->store_;

            if(s.s_cmdReady) {
                if(reading_memory_) {
                    ESP_LOGD(TAG, "Received 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);

                    // Save Data to memory Store
                    memory_buffer_.push_back((s.s_cmd >> 24) & 0xFF);
                    memory_buffer_.push_back((s.s_cmd >> 16) & 0xFF);
                    memory_buffer_.push_back((s.s_cmd >> 8) & 0xFF);
                    memory_buffer_.push_back(s.s_cmd & 0xFF);

                    // Next 4 Data Blocks
                    reading_memory_count_++;

                    // Memory reading complete
                    if(reading_memory_count_ == 6) {
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
                    std::string hex_result = str_upper_case(format_hex(s.s_cmd));
                    if(hex_result.substr(4, 1) == "D") {
                        identify_model_ = false;
                        this->cancel_timeout("wait_for_identification");

                        DeviceData device;

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

                        this->identify_complete_callback_.call(device);

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
                    } else {
                        ESP_LOGE(TAG, "Invalid indentification response!");
                    }
                }
                else {
                    if(s.s_cmd_is_long) {
                        ESP_LOGD(TAG, "Received 32-Bit command %08X", s.s_cmd);
                    } else {
                        ESP_LOGD(TAG, "Received 16-Bit command %04X", s.s_cmd);
                    }
                    this->publish_command(s.s_cmd, s.s_cmd_is_long, true);
                }

                s.s_cmdReady = false;
                s.s_cmd_is_long = false;
                s.s_cmd = 0;
            }
        }

        void TCBusComponent::publish_settings()
        {
            ESP_LOGD(TAG, "Handset volume (Door Call) %i", get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
            ESP_LOGD(TAG, "Handset volume (Internal Call) %i", get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
            ESP_LOGD(TAG, "Ringtone volume %i", get_setting(SETTING_VOLUME_RINGTONE));

            ESP_LOGD(TAG, "Entrance Door Call Ringtone %i", get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL));
            ESP_LOGD(TAG, "Second Entrance Door Call Ringtone %i", get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL));
            ESP_LOGD(TAG, "Floor Call Ringtone %i", get_setting(SETTING_RINGTONE_FLOOR_CALL));
            ESP_LOGD(TAG, "Internal Call Ringtone %i", get_setting(SETTING_RINGTONE_INTERNAL_CALL));

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

        volatile uint32_t TCBusComponentStore::s_last_bit_change = 0;
        volatile uint32_t TCBusComponentStore::s_cmd = 0;
        volatile bool TCBusComponentStore::s_cmd_is_long = false;
        volatile bool TCBusComponentStore::s_cmdReady = false;

        void IRAM_ATTR HOT TCBusComponentStore::gpio_intr(TCBusComponentStore *arg)
        {
            // Made by https://github.com/atc1441/TCSintercomArduino

            // Timing thresholds (in microseconds)
            const uint32_t BIT_0_MIN = 1000, BIT_0_MAX = 2999;
            const uint32_t BIT_1_MIN = 3000, BIT_1_MAX = 4999;
            const uint32_t BIT_2_MIN = 5000, BIT_2_MAX = 6999;
            const uint32_t RESET_MIN = 7000, RESET_MAX = 24000;

            static uint32_t curCMD = 0;   // Current command being constructed
            static uint32_t usLast = 0;  // Last timestamp in microseconds
            static uint8_t curCRC = 0;   // CRC received in the data
            static uint8_t calCRC = 1;   // Calculated CRC (starts at 1)
            static uint8_t curPos = 0;   // Current position in the bit stream
            static bool curIsLong = false; // 32 or 16 Bit Command
            static bool cmdIntReady = false; // Command ready flag

            // Calculate time difference
            uint32_t usNow = micros();
            uint32_t timeInUS = usNow - usLast;
            usLast = usNow;

            // Determine current bit based on time interval
            uint8_t curBit = 4; // Default to undefined bit
            if (timeInUS >= BIT_0_MIN && timeInUS <= BIT_0_MAX) {
                curBit = 0;
            } else if (timeInUS >= BIT_1_MIN && timeInUS <= BIT_1_MAX) {
                curBit = 1;
            } else if (timeInUS >= BIT_2_MIN && timeInUS <= BIT_2_MAX) {
                curBit = 2;
            } else if (timeInUS >= RESET_MIN && timeInUS <= RESET_MAX) {
                curBit = 3; // Reset condition
            } else {
                // Invalid timing, reset the position
                curPos = 0;
                return;
            }

            // Reset if a reset signal is detected
            if (curBit == 3) {
                curPos = 0;
                return;
            }

            if (curPos == 0) {
                // First bit after reset: expect start signal (bit 2)
                if (curBit == 2)
                {
                    curPos++;
                }

                curCMD = 0;
                curCRC = 0;
                calCRC = 1;
                curIsLong = false;
            } else if (curBit == 0 || curBit == 1) {
                // Process bits based on position
                if (curPos == 1) {
                    // Second bit: command length (0 or 1)
                    curIsLong = curBit;
                    curPos++;
                } else if (curPos >= 2 && curPos <= 17) {
                    // Bits 2-17: Command data (low 16 bits)
                    if (curBit)
                    {
                        BIT_SET(curCMD, (curIsLong ? 33 : 17) - curPos);
                    }

                    calCRC ^= curBit; // Update CRC
                    curPos++;
                } else if (curPos == 18) {
                    // Bit 18: Either part of data (32-bit command) or CRC for 16-bit command
                    if (curIsLong)
                    {
                        if (curBit)
                        {
                            BIT_SET(curCMD, 33 - curPos);
                        }
                        calCRC ^= curBit; // Update CRC
                        curPos++;
                    }
                    else
                    {
                        curCRC = curBit; // Save CRC for 16-bit command
                        cmdIntReady = true;
                    }
                } else if (curPos >= 19 && curPos <= 33) {
                    // Bits 19-33: Remaining bits for 32-bit command
                    if (curBit)
                    {
                        BIT_SET(curCMD, 33 - curPos);
                    }
                    calCRC ^= curBit; // Update CRC
                    curPos++;
                } else if (curPos == 34) {
                    // Bit 34: CRC for 32-bit command
                    curCRC = curBit;
                    cmdIntReady = true;
                }
            } else {
                // Undefined bit, reset the position
                curPos = 0;
            }

            // Save last bit timestamp
            arg->s_last_bit_change = millis();

            // If the command is ready, validate CRC and save the command
            if (cmdIntReady) {
                cmdIntReady = false;

                if (curCRC == calCRC) {
                    arg->s_cmd_is_long = curIsLong ? true : false;
                    arg->s_cmd = curCMD; // Save the decoded command
                    arg->s_cmdReady = true; // Indicate that a command is ready
                }

                // Reset state
                curCMD = 0;
                curPos = 0;
            }
        }

        void TCBusComponent::publish_command(uint32_t command, bool is_long, bool received)
        {
            // Get current TCS Serial Number
            uint32_t tcs_serial = this->serial_number_;

            // Parse Command
            CommandData cmd_data = parseCommand(command, is_long);
            ESP_LOGD(TAG, "[Parsed] Type: %s, Address: %i, Payload: %x, Serial: %i, Length: %i-Bit", command_type_to_string(cmd_data.type), cmd_data.address, cmd_data.payload, cmd_data.serial_number, (is_long ? 32 : 16));

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
                    uint32_t listener_serial_number = listener->serial_number_.has_value() ? listener->serial_number_.value() : tcs_serial;
                    
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

                // Fire Home Assistant Event if event name is specified
                if (strcmp(event_, "esphome.none") != 0) {
                    #ifdef USE_API
                    auto capi = new esphome::api::CustomAPIDevice();
                    ESP_LOGD(TAG, "Send event to Home Assistant on %s", event_);
                    capi->fire_homeassistant_event(event_, {
                        {"command", cmd_data.command_hex},
                        {"type", command_type_to_string(cmd_data.type)},
                        {"address", std::to_string(cmd_data.address)},
                        {"payload", std::to_string(cmd_data.payload)},
                        {"serial_number", std::to_string(cmd_data.serial_number)}
                    });
                    #endif
                }
            }
        }

        void TCBusComponent::send_command(CommandType type, uint8_t address, uint32_t payload, uint32_t serial_number)
        {
            ESP_LOGV(TAG, "Generating command: Type: %s, Address: %i, Payload: %X, Serial number: %i", command_type_to_string(type), address, payload, serial_number);

            // Get current TCS Serial Number
            uint32_t tcs_serial = this->serial_number_;

            if(serial_number == 0) {
                ESP_LOGV(TAG, "Serial Number is 0, use intercom serial number: %i", tcs_serial);
                serial_number = tcs_serial;
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
                ESP_LOGD(TAG, "Sending 32-Bit command %08X", command);
            } else {
                ESP_LOGD(TAG, "Sending 16-Bit command %04X", command);
            }

            if (this->sending) {
                ESP_LOGD(TAG, "Sending of command %i cancelled, another sending is in progress", command);
            } else {
                // Prevent collisions
                /*auto &s = this->store_;

                uint32_t msNow = millis();
                std::srand(msNow);

                delay(std::rand() % 101 + 50); // 50-150
                while((msNow - s.s_last_bit_change) < TCS_SEND_WAIT_DURATION)
                {
                    delay(std::rand() % 101 + 50); // 50-150
                }*/

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
            }
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

        void TCBusComponent::add_identify_complete_callback(std::function<void(DeviceData)> &&callback)
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
            if(serial_number == 0) {
                serial_number = this->serial_number_;
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

        void TCBusComponent::read_memory(uint32_t serial_number)
        {
            if(serial_number == 0) {
                serial_number = this->serial_number_;
            }

            this->cancel_timeout("wait_for_memory_reading");
            reading_memory_ = false;

            uint8_t device_category = getDeviceCategory();

            ESP_LOGD(TAG, "Select Indoor Stations");
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, device_category);
            delay(50);

            ESP_LOGD(TAG, "Select Memory Page %i of Serial Number %i", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
            delay(50);

            memory_buffer_.clear();
            reading_memory_ = true;
            reading_memory_count_ = 0;

            this->set_timeout("wait_for_memory_reading", 5000, [this]() {
                memory_buffer_.clear();
                reading_memory_ = false;
                reading_memory_count_ = 0;

                this->read_memory_timeout_callback_.call();
                ESP_LOGE(TAG, "Reading memory timed out!");
            });
                
            ESP_LOGD(TAG, "Read 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);
            send_command(COMMAND_TYPE_READ_MEMORY_BLOCK, reading_memory_count_);
        }

        uint8_t TCBusComponent::get_setting(SettingType type)
        {
            if(memory_buffer_.size() == 0) {
                return 0;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type);

            if(cellData.index != 0) {
                return cellData.left_nibble ? ((memory_buffer_[cellData.index] >> 4) & 0xF) : (memory_buffer_[cellData.index] & 0xF);
            } else {
                ESP_LOGD(TAG, "The setting %s is not available for model %s", setting_type_to_string(type), model_to_string(model_));
                return 0;
            }
        }

        bool TCBusComponent::update_setting(SettingType type, uint8_t new_value, uint32_t serial_number)
        {
            if(memory_buffer_.size() == 0) {
                ESP_LOGW(TAG, "Memory Buffer is empty! Please read memory first!");
                return false;
            }

            if(serial_number == 0) {
                serial_number = this->serial_number_;
            }

            if(serial_number == 0) {
                ESP_LOGW(TAG, "Serial Number is not set!");
                return false;
            }

            uint8_t saved_nibble = 0;

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type);

            if(cellData.index != 0) {
                // Apply new nibble and keep other nibble
                saved_nibble = (cellData.left_nibble ? memory_buffer_[cellData.index] : (memory_buffer_[cellData.index] >> 4)) & 0xF;
                memory_buffer_[cellData.index] = cellData.left_nibble ? ((new_value << 4) | (saved_nibble & 0xF)) : ((saved_nibble << 4) | (new_value & 0xF));

                // Prepare Transmission
                ESP_LOGD(TAG, "Select Device Category");
                uint8_t device_category = getDeviceCategory();
                send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, device_category);
                delay(50);

                ESP_LOGD(TAG, "Select Memory Page %i of Serial Number %i", 0, serial_number);
                send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
                delay(50);

                // Transfer new settings value to memory
                uint16_t new_values = (memory_buffer_[cellData.index] << 8) | memory_buffer_[cellData.index + 1];
                send_command(COMMAND_TYPE_WRITE_MEMORY, cellData.index, new_values, serial_number);

                return true;
            } else {
                ESP_LOGW(TAG, "The setting %s is not available for model %s", setting_type_to_string(type), model_to_string(model_));
                return false;
            }
        }

        uint8_t TCBusComponent::getDeviceCategory()
        {
            switch (model_) {
                case MODEL_ISW3030: /* TC50 */
                case MODEL_ISW3130: /* TC50P */
                case MODEL_ISW3230: /* TC50 GFA */
                case MODEL_ISW3330: /* TC50 BW */
                case MODEL_ISW3340:
                case MODEL_ISW5010: /* TC60 */
                case MODEL_ISW5020:
                case MODEL_ISW5030:
                case MODEL_ISW5031:
                case MODEL_ISW5033:
                case MODEL_IVW511X: /* VTC60 */
                case MODEL_IVW521X: /* VTC60/2D */
                case MODEL_ISW6031:
                case MODEL_ISW7030: /* TC70 */
                case MODEL_IVW7510: /* VTC70 */
                case MODEL_ISH7030: /* TCH70 */
                case MODEL_IVH7510: /* VTCH70 */
                case MODEL_ISW6010:
                case MODEL_IVW6511:
                case MODEL_ISWM7000:
                case MODEL_IVWM7000:
                case MODEL_ISW4100: /* TC31 */
                case MODEL_IMM2100: /* TCE31 */
                case MODEL_IVW2210: /* Ecoos */
                case MODEL_IVW2211: /* Ecoos */
                case MODEL_IVW2212: /* Ecoos */
                case MODEL_VTC42V2:
                case MODEL_TC40V2:
                case MODEL_VTC40:
                case MODEL_TC40:
                case MODEL_TC2000:
                case MODEL_TC20P:
                case MODEL_TC20F:
                case MODEL_IVW2220:
                case MODEL_IVW2221:
                case MODEL_IVW3011:
                case MODEL_IVW3012:
                case MODEL_TKIS:
                case MODEL_TKISV:
                case MODEL_CAI2000:
                case MODEL_CAIXXXX:
                case MODEL_ISW42X0:
                    return 1;

                case MODEL_ISH3340:
                case MODEL_ISH3022: /* TCH50P */
                case MODEL_ISH3130: /* TCH50P */
                case MODEL_ISW3022:
                case MODEL_ISH3230: /* TCH50 GFA */
                case MODEL_ISH3030: /* TCH50 */
                case MODEL_ISH1030: /* TTS25 */
                case MODEL_IMM1000: /* TCH30 */
                case MODEL_IMM1100: /* TCHE30 */
                case MODEL_IMM1300: /* VTCH30 */
                case MODEL_IMM1500:
                case MODEL_IMM1310: /* VTCHE30 */
                case MODEL_IMM1110: /* TCHEE30 */
                case MODEL_IVH3222: /* VTCH50 */
                case MODEL_IVH4222: /* VTCH50/2D */
                case MODEL_VMH:
                case MODEL_VML:
                case MODEL_VMF:
                    return 0;

                default:
                    return 0;
            }
        }

        SettingCellData TCBusComponent::getSettingCellData(SettingType setting)
        {
            SettingCellData data;

            switch (model_) {
                case MODEL_ISH3030:
                case MODEL_ISH3230:
                case MODEL_ISH3340:
                case MODEL_ISW3030:
                case MODEL_ISW3230:
                case MODEL_ISW3340:
                case MODEL_ISW3130:
                case MODEL_ISW3330:
                case MODEL_IVH4222:
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;

                        case SETTING_VOLUME_HANDSET_DOOR_CALL:
                            data.index = 21;
                            data.left_nibble = false;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_ISH1030:
                case MODEL_IVH3222:
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_IVW511X:
                case MODEL_IVW521X:
                    // TASTA Video
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL:
                            data.index = 12;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        // Values: 0,2,4,6
                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_DOOR_CALL:
                            data.index = 21;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_INTERNAL_CALL:
                            data.index = 21;
                            data.left_nibble = true;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_ISW5010:
                case MODEL_ISW5020:
                case MODEL_ISW5030:
                case MODEL_ISW5031:
                case MODEL_ISW5033:
                    // TASTA Audio
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL:
                            data.index = 12;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        // Values: 0,2,4,6
                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_DOOR_CALL:
                            data.index = 21;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_INTERNAL_CALL:
                            data.index = 21;
                            data.left_nibble = true;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_IVW2210:
                case MODEL_IVW2211:
                case MODEL_IVW2212:
                    // ECOOS
                    switch (setting)
                    {
                        /*case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;*/

                        default: break;
                    }
                    break;
                default: break;
            }

            return data;
        }

        void TCBusComponent::write_memory(uint32_t serial_number)
        {
            if(memory_buffer_.size() == 0) {
                ESP_LOGW(TAG, "Memory buffer is empty! Please read memory first!");
                return;
            }

            if(serial_number == 0) {
                serial_number = this->serial_number_;
            }

            if(serial_number == 0) {
                ESP_LOGW(TAG, "Serial number is not set!");
                return;
            }

            // Prepare Transmission
            ESP_LOGD(TAG, "Select device category");
            uint8_t device_category = getDeviceCategory();
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, device_category);
            delay(50);

            ESP_LOGD(TAG, "Select memory page %i of serial number %i", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
            delay(50);

            // Transmit Memory
            for (size_t address = 0; address < memory_buffer_.size(); address += 2) {
                uint16_t new_value = (memory_buffer_[address] << 8) | memory_buffer_[address + 1];
                send_command(COMMAND_TYPE_WRITE_MEMORY, address, new_value, serial_number);
                delay(50);
            }
        }

    }  // namespace tc_bus
}  // namespace esphome