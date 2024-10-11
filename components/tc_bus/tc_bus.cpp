#include "protocol.h"
#include "tc_bus.h"

#include "esphome.h"
#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/api/custom_api_device.h"

#if defined(USE_ESP_IDF) || (defined(USE_ARDUINO) && defined(ESP32))
#include "soc/efuse_reg.h"
#include "soc/efuse_periph.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#endif

#ifdef USE_ARDUINO
#include "Arduino.h"
#endif

#include <stdint.h>
#include <optional>

using namespace esphome;

namespace esphome
{
    namespace tc_bus
    {
        static const char *const TAG = "tc_bus";

        static const uint8_t TCS_MSG_START_MS = 6; // a new message
        static const uint8_t TCS_ONE_BIT_MS = 4; // a 1-bit is 4ms long
        static const uint8_t TCS_ZERO_BIT_MS = 2; // a 0-bit is 2ms long

        TCBusComponent *global_tc_bus = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        void TCBusComponent::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up TC:BUS Intercom...");

            #if defined(USE_ESP_IDF) || (defined(USE_ARDUINO) && defined(ESP32))
            ESP_LOGD(TAG, "Check for Doorman Hardware");

            // Doorman Hardware Revision
            uint8_t ver[3];
            uint32_t value;
            esp_efuse_read_block(EFUSE_BLK3, &value, 0, 24);
            ver[0] = value >> 0;
            ver[1] = value >> 8;
            ver[2] = value >> 16;

            if(ver[0] > 0)
            {
                ESP_LOGI(TAG, "Doorman Hardware detected: V%i.%i.%i", ver[0], ver[1], ver[2]);
                this->hardware_version_str_ = "Doorman " + std::to_string(ver[0]) + "." + std::to_string(ver[1]) + "." + std::to_string(ver[2]);

                // Override GPIO
                if(ver[0] == 1 && (ver[1] == 3 || ver[1] == 4))
                {
                    esp32::ESP32InternalGPIOPin *gpio_pin_rx_;
                    gpio_pin_rx_ = new(esp32::ESP32InternalGPIOPin);
                    gpio_pin_rx_->set_pin(static_cast<gpio_num_t>(9));
                    gpio_pin_rx_->set_flags(gpio::Flags::FLAG_INPUT);
                    this->set_rx_pin(gpio_pin_rx_);

                    esp32::ESP32InternalGPIOPin *gpio_pin_tx_;
                    gpio_pin_tx_ = new(esp32::ESP32InternalGPIOPin);
                    gpio_pin_tx_->set_pin(static_cast<gpio_num_t>(8));
                    gpio_pin_tx_->set_flags(gpio::Flags::FLAG_OUTPUT);
                    this->set_tx_pin(gpio_pin_tx_);

                    ESP_LOGD(TAG, "Doorman Hardware GPIO Override: RX (%i), TX (%i)", this->rx_pin_->get_pin(), this->tx_pin_->get_pin());
                }
            }
            #endif

            if (this->hardware_version_ != nullptr)
            {
                this->hardware_version_->publish_state(this->hardware_version_str_);
            }

            this->rx_pin_->setup();
            this->tx_pin_->setup();
            this->tx_pin_->digital_write(false);
            
            auto &s = this->store_;

            s.rx_pin = this->rx_pin_->to_isr();
            this->rx_pin_->attach_interrupt(TCBusComponentStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);

            // Reset Sensors
            if (this->bus_command_ != nullptr)
            {
                this->bus_command_->publish_state("");
            }

            if (this->door_readiness_ != nullptr)
            {
                this->door_readiness_->publish_initial_state(false);
            }

            for (auto &listener : listeners_)
            {
                listener->turn_off(&listener->timer_);
            }
        }

        void TCBusComponent::dump_config()
        {
            ESP_LOGCONFIG(TAG, "TC:BUS:");
            LOG_PIN("  Pin RX: ", this->rx_pin_);
            LOG_PIN("  Pin TX: ", this->tx_pin_);

            if (strcmp(this->event_, "esphome.none") != 0)
            {
                ESP_LOGCONFIG(TAG, "  Event: %s", this->event_);
            }
            else
            {
                ESP_LOGCONFIG(TAG, "  Event: disabled");
            }

            ESP_LOGCONFIG(TAG, "  Hardware: %s", this->hardware_version_str_.c_str());

            #ifdef USE_BINARY_SENSOR
            ESP_LOGCONFIG(TAG, "Binary Sensors:");
            LOG_BINARY_SENSOR("  ", "Door readiness", this->door_readiness_);
            #endif

            #ifdef USE_TEXT_SENSOR
            ESP_LOGCONFIG(TAG, "Text Sensors:");
            LOG_TEXT_SENSOR("  ", "Last Bus Command", this->bus_command_);
            LOG_TEXT_SENSOR("  ", "Hardware Version", this->hardware_version_);
            #endif
        }

        void TCBusComponent::loop()
        {
            // Turn off binary sensor after ... milliseconds
            uint32_t now_millis = millis();
            for (auto &listener : listeners_)
            { 
                if (listener->timer_ && now_millis > listener->timer_)
                {
                    listener->turn_off(&listener->timer_);
                }
            }

            // Cancel Reading memory
            if(reading_memory_)
            {
                uint32_t now_millis = millis();

                if(now_millis - reading_memory_timer_ >= 5000)
                {
                    reading_memory_ = false;
                    reading_memory_count_ = 0;
                    reading_memory_timer_ = 0;
                        
                    ESP_LOGE(TAG, "Reading memory timed out!");
                    this->read_memory_timeout_callback_.call();
                }
            }

            auto &s = this->store_;

            if(s.s_cmdReady)
            {
                if(reading_memory_)
                {
                    ESP_LOGD(TAG, "Received 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);

                    // Save Data to memory Store
                    memory_buffer_.push_back((s.s_cmd >> 24) & 0xFF);
                    memory_buffer_.push_back((s.s_cmd >> 16) & 0xFF);
                    memory_buffer_.push_back((s.s_cmd >> 8) & 0xFF);
                    memory_buffer_.push_back(s.s_cmd & 0xFF);

                    // Next 4 Data Blocks
                    reading_memory_count_++;

                    if(reading_memory_count_ == 6)
                    {
                        // Turn off
                        reading_memory_ = false;
                        reading_memory_timer_ = 0;

                        ESP_LOGD(TAG, "Handset volume %i", get_setting(SETTING_VOLUME_HANDSET));
                        ESP_LOGD(TAG, "Ringtone volume %i", get_setting(SETTING_VOLUME_RINGTONE));
                        ESP_LOGD(TAG, "Door Call Ringtone %i", get_setting(SETTING_RINGTONE_DOOR_CALL));
                        ESP_LOGD(TAG, "Floor Call Ringtone %i", get_setting(SETTING_RINGTONE_FLOOR_CALL));
                        ESP_LOGD(TAG, "Internal Call Ringtone %i", get_setting(SETTING_RINGTONE_INTERNAL_CALL));

                        this->read_memory_complete_callback_.call(memory_buffer_);
                    }
                    else
                    {
                        delay(20);

                        // Request Data Blocks
                        ESP_LOGD(TAG, "Read 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);
                        send_command(COMMAND_TYPE_READ_MEMORY_BLOCK, reading_memory_count_);
                    }
                }
                else
                {
                    ESP_LOGD(TAG, "Received command %08X", s.s_cmd);
                    this->publish_command(s.s_cmd, true);
                }

                s.s_cmdReady = false;
                s.s_cmd = 0;
            }
        }

        void TCBusComponent::register_listener(TCBusListener *listener)
        {
            this->listeners_.push_back(listener); 
        }

        volatile uint32_t TCBusComponentStore::s_cmd = 0;
        volatile uint8_t TCBusComponentStore::s_cmdLength = 0;
        volatile bool TCBusComponentStore::s_cmdReady = false;

        void bitSetIDF(uint32_t *variable, int bitPosition) {
            *variable |= (1UL << bitPosition);
        }

        uint8_t bitReadIDF(uint32_t variable, int bitPosition) {
            return (variable >> bitPosition) & 0x01;
        }

        void IRAM_ATTR HOT TCBusComponentStore::gpio_intr(TCBusComponentStore *arg)
        {
            // Made by https://github.com/atc1441/TCSintercomArduino
            static uint32_t curCMD;
            static uint32_t usLast;

            static uint8_t curCRC;
            static uint8_t calCRC;
            static uint8_t curLength;
            static uint8_t cmdIntReady;
            static uint8_t curPos;

            uint32_t usNow = micros();
            uint32_t timeInUS = usNow - usLast;
            usLast = usNow;

            uint8_t curBit = 4;

            if (timeInUS >= 1000 && timeInUS <= 2999)
            {
                curBit = 0;
            }
            else if (timeInUS >= 3000 && timeInUS <= 4999)
            {
                curBit = 1;
            }
            else if (timeInUS >= 5000 && timeInUS <= 6999)
            {
                curBit = 2;
            }
            else if (timeInUS >= 7000 && timeInUS <= 24000)
            {
                curBit = 3;
                curPos = 0;
            }

            if (curPos == 0)
            {
                if (curBit == 2)
                {
                    curPos++;
                }

                curCMD = 0;
                curCRC = 0;
                calCRC = 1;
                curLength = 0;
            }
            else if (curBit == 0 || curBit == 1)
            {
                if (curPos == 1)
                {
                    curLength = curBit;
                    curPos++;
                }
                else if (curPos >= 2 && curPos <= 17)
                {
                    if (curBit)
                    {
                        #if defined(USE_ESP_IDF)
                        bitSetIDF(&curCMD, (curLength ? 33 : 17) - curPos);
                        #else
                        bitSet(curCMD, (curLength ? 33 : 17) - curPos);
                        #endif
                    }

                    calCRC ^= curBit;
                    curPos++;
                }
                else if (curPos == 18)
                {
                    if (curLength)
                    {
                        if (curBit)
                        {
                            #if defined(USE_ESP_IDF)
                            bitSetIDF(&curCMD, 33 - curPos);
                            #else
                            bitSet(curCMD, 33 - curPos);
                            #endif
                        }

                        calCRC ^= curBit;
                        curPos++;
                    }
                    else
                    {
                        curCRC = curBit;
                        cmdIntReady = 1;
                    }
                }
                else if (curPos >= 19 && curPos <= 33)
                {
                    if (curBit)
                    {
                        #if defined(USE_ESP_IDF)
                        bitSetIDF(&curCMD, 33 - curPos);
                        #else
                        bitSet(curCMD, 33 - curPos);
                        #endif
                    }
                    
                    calCRC ^= curBit;
                    curPos++;
                }
                else if (curPos == 34)
                {
                    curCRC = curBit;
                    cmdIntReady = 1;
                }
            }
            else
            {
                curPos = 0;
            }

            if (cmdIntReady)
            {
                cmdIntReady = 0;

                if (curCRC == calCRC)
                {
                    arg->s_cmdReady = true;
                    arg->s_cmd = curCMD;
                }

                curCMD = 0;
                curPos = 0;
            }
        }

        void TCBusComponent::publish_command(uint32_t command, bool received)
        {
            // Get current TCS Serial Number
            uint32_t tcs_serial = this->serial_number_;
            if (serial_number_lambda_.has_value()) {
                auto optional_value = (*serial_number_lambda_)();
                if (optional_value.has_value()) {
                    tcs_serial = optional_value.value();
                }
            }

            // Parse Command
            CommandData cmd_data = parseCommand(command);
            ESP_LOGD(TAG, "[Parsed] Type: %s, Address: %i, Payload: %x, Serial: %i", command_type_to_string(cmd_data.type), cmd_data.address, cmd_data.payload, cmd_data.serial_number);

            // Update Door Readiness Status
            if (cmd_data.type == COMMAND_TYPE_START_TALKING_DOOR_STATION) {
                bool door_readiness_state = cmd_data.payload == 1;
                ESP_LOGD(TAG, "Door readiness: %s", YESNO(door_readiness_state));
                if (this->door_readiness_ != nullptr) {
                    this->door_readiness_->publish_state(door_readiness_state);
                }
            } else if (cmd_data.type == COMMAND_TYPE_END_OF_DOOR_READINESS) {
                ESP_LOGD(TAG, "Door readiness: %s", YESNO(false));
                if (this->door_readiness_ != nullptr) {
                    this->door_readiness_->publish_state(false);
                }
            } else if (cmd_data.type == COMMAND_TYPE_PROGRAMMING_MODE) {
                ESP_LOGD(TAG, "Programming Mode: %s", YESNO(cmd_data.payload == 1));
                this->programming_mode_ = cmd_data.payload == 1;
            }

            // Publish Command to Last Bus Command Sensor
            if (this->bus_command_ != nullptr) {
                this->bus_command_->publish_state(cmd_data.command_hex);
            }

            // If the command was received, notify the listeners
            if (received)
            {
                // Fire Callback
                this->received_command_callback_.call(cmd_data);

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
            }
        }

        void TCBusComponent::send_command(CommandType type, uint8_t address, uint32_t payload, uint32_t serial_number)
        {
            ESP_LOGV(TAG, "Generating command: Type: %s, Address: %i, Payload: %X, Serial number: %i", command_type_to_string(type), address, payload, serial_number);

            // Get current TCS Serial Number
            uint32_t tcs_serial = this->serial_number_;
            if (serial_number_lambda_.has_value()) {
                auto optional_value = (*serial_number_lambda_)();
                if (optional_value.has_value()) {
                    tcs_serial = optional_value.value();
                }
            }

            if(serial_number == 0)
            {
                ESP_LOGV(TAG, "Serial Number is 0, use intercom serial number: %i", tcs_serial);
                serial_number = tcs_serial;
            }

            uint32_t command = buildCommand(type, address, payload, serial_number);
            if(command == 0)
            {
                ESP_LOGW(TAG, "Sending commands of type %s is not supported!", command_type_to_string(type));
            }
            else
            {
                send_command(command);
            }
        }

        void TCBusComponent::send_command(uint32_t command)
        {
            ESP_LOGD(TAG, "Sending command %08X", command);

            if (this->sending)
            {
                ESP_LOGD(TAG, "Sending of command %i cancelled, another sending is in progress", command);
            }
            else
            {
                // Pause reading
                ESP_LOGV(TAG, "Pause reading");
                this->rx_pin_->detach_interrupt();

                // Source: https://github.com/atc1441/TCSintercomArduino
                this->sending = true;

                // Determine message length and whether it's a long message
                bool isLongMessage = (command > 0xFFFF);
                int length = isLongMessage ? 32 : 16;

                uint8_t checksm = 1;
                bool output_state = false;

                // Start transmission
                this->tx_pin_->digital_write(true);
                delay(TCS_MSG_START_MS);

                this->tx_pin_->digital_write(false);
                delay(isLongMessage ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);

                int curBit = 0;
                for (uint8_t i = length; i > 0; i--)
                {
                    #if defined(USE_ESP_IDF)
                    curBit = bitReadIDF(command, i - 1);
                    #else
                    curBit = bitRead(command, i - 1);
                    #endif
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
                this->publish_command(command, false);
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

        void TCBusComponent::set_programming_mode(bool enabled)
        {
            send_command(COMMAND_TYPE_PROGRAMMING_MODE, 0, enabled ? 1 : 0);
        }

        void TCBusComponent::read_memory(uint32_t serial_number)
        {
            if(serial_number == 0)
            {
                serial_number = this->serial_number_;
                if (serial_number_lambda_.has_value()) {
                    auto optional_value = (*serial_number_lambda_)();
                    if (optional_value.has_value()) {
                        serial_number = optional_value.value();
                    }
                }
            }

            memory_buffer_.clear();
            reading_memory_count_ = 0;
            reading_memory_ = false;

            ESP_LOGD(TAG, "Select Indoor Stations");
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, 0); // payload 0 = indoor stations
            delay(50);

            ESP_LOGD(TAG, "Select Memory Page %i of Serial Number %i", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
            delay(50);

            reading_memory_ = true;
            reading_memory_count_ = 0;
            reading_memory_timer_ = millis();
                
            ESP_LOGD(TAG, "Read 4 memory addresses %i to %i", (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4);
            send_command(COMMAND_TYPE_READ_MEMORY_BLOCK, reading_memory_count_);
        }

        uint8_t TCBusComponent::get_setting(SettingType type)
        {
            if(memory_buffer_.size() == 0)
            {
                return 0;
            }

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type);

            if(cellData.index != 0)
            {
                return cellData.left_nibble ? ((memory_buffer_[cellData.index] >> 4) & 0xF) : (memory_buffer_[cellData.index] & 0xF);
            }
            else
            {
                ESP_LOGV(TAG, "The setting %s is not available for model %s", setting_type_to_string(type), model_to_string(model_));
                return 0;
            }
        }

        bool TCBusComponent::update_setting(SettingType type, uint8_t new_value, uint32_t serial_number)
        {
            if(memory_buffer_.size() == 0)
            {
                ESP_LOGW(TAG, "Memory Buffer is empty! Please read memory first!");
                return false;
            }

            if(serial_number == 0)
            {
                serial_number = this->serial_number_;
                if (serial_number_lambda_.has_value()) {
                    auto optional_value = (*serial_number_lambda_)();
                    if (optional_value.has_value()) {
                        serial_number = optional_value.value();
                    }
                }
            }

            if(serial_number == 0)
            {
                ESP_LOGW(TAG, "Serial Number is not set!");
                return false;
            }

            uint8_t saved_nibble = 0;

            // Get Setting Cell Data by Model
            SettingCellData cellData = getSettingCellData(type);

            if(cellData.index != 0)
            {
                // Apply new nibble and keep other nibble
                saved_nibble = (cellData.left_nibble ? memory_buffer_[cellData.index] : (memory_buffer_[cellData.index] >> 4)) & 0xF;
                memory_buffer_[cellData.index] = cellData.left_nibble ? ((new_value << 4) | (saved_nibble & 0xF)) : ((saved_nibble << 4) | (new_value & 0xF));

                // Prepare Transmission
                ESP_LOGD(TAG, "Select Indoor Stations");
                send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, 0); // payload 0 = indoor stations
                delay(50);

                ESP_LOGD(TAG, "Select Memory Page %i of Serial Number %i", 0, serial_number);
                send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
                delay(50);

                // Transfer new settings value to memory
                uint16_t new_values = (memory_buffer_[cellData.index] << 8) | memory_buffer_[cellData.index + 1];
                send_command(COMMAND_TYPE_WRITE_MEMORY, cellData.index, new_values, serial_number);

                return true;
            }
            else
            {
                ESP_LOGW(TAG, "The setting %s is not available for model %s", setting_type_to_string(type), model_to_string(model_));
                return false;
            }
        }

        SettingCellData TCBusComponent::getSettingCellData(SettingType setting)
        {
            SettingCellData data;

            switch (model_)
            {
                case MODEL_ISH_3030:
                case MODEL_ISW_3030:
                    switch (setting)
                    {
                        case SETTING_RINGTONE_DOOR_CALL:
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

                        case SETTING_VOLUME_HANDSET:
                            data.index = 21;
                            data.left_nibble = false;
                            break;

                        default: break;
                    }
                    break;

                default: break;
            }

            return data;
        }

        void TCBusComponent::write_memory(uint32_t serial_number)
        {
            if(memory_buffer_.size() == 0)
            {
                ESP_LOGW(TAG, "Memory Buffer is empty! Please read memory first!");
                return;
            }

            if(serial_number == 0)
            {
                serial_number = this->serial_number_;
                if (serial_number_lambda_.has_value()) {
                    auto optional_value = (*serial_number_lambda_)();
                    if (optional_value.has_value()) {
                        serial_number = optional_value.value();
                    }
                }
            }

            if(serial_number == 0)
            {
                ESP_LOGW(TAG, "Serial Number is not set!");
                return;
            }

            // Prepare Transmission
            ESP_LOGD(TAG, "Select Indoor Stations");
            send_command(COMMAND_TYPE_SELECT_DEVICE_GROUP, 0, 0); // payload 0 = indoor stations
            delay(50);

            ESP_LOGD(TAG, "Select Memory Page %i of Serial Number %i", 0, serial_number);
            send_command(COMMAND_TYPE_SELECT_MEMORY_PAGE, 0, 0, serial_number);
            delay(50);

            // Transmit Memory
            uint8_t address = 0;
            for (size_t i = 0; i < memory_buffer_.size(); i += 2)
            {
                uint16_t new_value = (memory_buffer_[i] << 8) | memory_buffer_[i + 1];
                send_command(COMMAND_TYPE_WRITE_MEMORY, address, new_value, serial_number);
                address = address + 2;
                delay(50);
            }
        }

    }  // namespace tc_bus
}  // namespace esphome