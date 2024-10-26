#pragma once

#include "protocol.h"

#include <utility>
#include <vector>
#include "esphome/core/preferences.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/select/select.h"
#include "esphome/components/number/number.h"

namespace esphome
{
    namespace tc_bus
    {
        class TCBusListener
        {
            public:
                void set_command_lambda(std::function<optional<uint32_t>()> &&f) { this->command_lambda_ = f; }
                void set_command(uint32_t command) { this->command_ = command; }

                void set_address_lambda(std::function<optional<uint8_t>()> &&f) { this->address_lambda_ = f; }
                void set_address(uint8_t address) { this->address_ = address; }
                
                void set_payload_lambda(std::function<optional<uint32_t>()> &&f) { this->payload_lambda_ = f; }
                void set_payload(uint32_t payload) { this->payload_ = payload; }
                
                void set_command_type(CommandType type) { this->type_ = type; }
                
                void set_serial_number(uint32_t serial_number) { this->serial_number_ = serial_number; }
                
                void set_auto_off(uint16_t auto_off) { this->auto_off_ = auto_off; }

                virtual void turn_on(uint32_t *timer, uint16_t auto_off) {};
                virtual void turn_off(uint32_t *timer) {};

                uint32_t timer_;
                uint16_t auto_off_;
                
                optional<uint32_t> serial_number_;
                optional<CommandType> type_;

                optional<uint32_t> command_;
                optional<std::function<optional<uint32_t>()>> command_lambda_;

                optional<uint8_t> address_;
                optional<std::function<optional<uint8_t>()>> address_lambda_;

                optional<uint32_t> payload_;
                optional<std::function<optional<uint32_t>()>> payload_lambda_;
        };

        struct TCBusComponentStore
        {
            static void gpio_intr(TCBusComponentStore *arg);

            static volatile uint32_t s_cmd;
            static volatile uint8_t s_cmdLength;
            static volatile bool s_cmdReady;

            ISRInternalGPIOPin rx_pin;
        };

        class TCBusComponent : public Component
        {
            public:
                void set_rx_pin(InternalGPIOPin *pin) { this->rx_pin_ = pin; }
                void set_tx_pin(InternalGPIOPin *pin) { this->tx_pin_ = pin; }

                void set_event(const char *event) { this->event_ = event; }

                void setup() override;
                void dump_config() override;
                void loop() override;

                void register_listener(TCBusListener *listener);

                void set_model(Model model) { this->model_ = model; }

                void set_sn(uint32_t serial_number) { this->serial_number_ = serial_number; }
                void set_sn_lambda(std::function<optional<uint32_t>()> &&serial_number_lambda) { this->serial_number_lambda_ = serial_number_lambda; }

                void set_bus_command_text_sensor(text_sensor::TextSensor *bus_command) { this->bus_command_text_sensor_ = bus_command; }
                void set_hardware_version_text_sensor(text_sensor::TextSensor *hardware_version) { this->hardware_version_text_sensor_ = hardware_version; }
                void set_door_readiness_binary_sensor(binary_sensor::BinarySensor *door_readiness) { this->door_readiness_binary_sensor_ = door_readiness; }
                void set_intercom_model_select(select::Select *intercom_model) { this->intercom_model_select_ = intercom_model; }
                
                void set_intercom_ringtone_door_call_select(select::Select *intercom_ringtone_door_call) { this->intercom_ringtone_door_call_select_ = intercom_ringtone_door_call; }
                void set_intercom_ringtone_floor_call_select(select::Select *intercom_ringtone_floor_call) { this->intercom_ringtone_floor_call_select_ = intercom_ringtone_floor_call; }
                void set_intercom_ringtone_internal_call_select(select::Select *intercom_ringtone_internal_call) { this->intercom_ringtone_internal_call_select_ = intercom_ringtone_internal_call; }
                
                void set_intercom_volume_handset_number(number::Number *intercom_volume_handset) { this->intercom_volume_handset_number_ = intercom_volume_handset; }
                void set_intercom_volume_ringtone_number(number::Number *intercom_volume_ringtone) { this->intercom_volume_ringtone_number_ = intercom_volume_ringtone; }

                void send_command(uint32_t command);
                void send_command(CommandType type, uint8_t address = 0, uint32_t payload = 0, uint32_t serial_number = 0);
                void set_programming_mode(bool enabled);

                void read_memory(uint32_t serial_number);
                void request_memory_blocks(uint8_t start_address);
                void write_memory(uint32_t serial_number = 0);

                uint8_t get_setting(SettingType type);
                bool update_setting(SettingType type, uint8_t new_value, uint32_t serial_number = 0);
                SettingCellData getSettingCellData(SettingType setting);

                void publish_command(uint32_t command, bool fire_events);

                void publish_settings();

                void add_received_command_callback(std::function<void(CommandData)> &&callback);
                CallbackManager<void(CommandData)> received_command_callback_{};
                void add_read_memory_complete_callback(std::function<void(std::vector<uint8_t>)> &&callback);
                CallbackManager<void(std::vector<uint8_t>)> read_memory_complete_callback_{};
                void add_read_memory_timeout_callback(std::function<void()> &&callback);
                CallbackManager<void()> read_memory_timeout_callback_{};

                bool sending;

                ESPPreferenceObject &get_pref() {
                    return this->pref_;
                }

            protected:
                InternalGPIOPin *rx_pin_;
                InternalGPIOPin *tx_pin_;
                const char* event_;

                Model model_;

                TCBusComponentStore store_;
                std::vector<TCBusListener *> listeners_{};

                uint32_t serial_number_;
                optional<std::function<optional<uint32_t>()>> serial_number_lambda_;

                std::string hardware_version_str_ = "Generic";

                text_sensor::TextSensor *bus_command_text_sensor_{nullptr};
                text_sensor::TextSensor *hardware_version_text_sensor_{nullptr};
                binary_sensor::BinarySensor *door_readiness_binary_sensor_{nullptr};
                
                select::Select *intercom_model_select_{nullptr};
                select::Select *intercom_ringtone_door_call_select_{nullptr};
                select::Select *intercom_ringtone_floor_call_select_{nullptr};
                select::Select *intercom_ringtone_internal_call_select_{nullptr};
                number::Number *intercom_volume_handset_number_{nullptr};
                number::Number *intercom_volume_ringtone_number_{nullptr};

                bool programming_mode_ = false;

                bool reading_memory_ = false;
                uint8_t reading_memory_count_ = 0;
                uint32_t reading_memory_timer_ = 0;
                std::vector<uint8_t> memory_buffer_;

                ESPPreferenceObject pref_;
        };

        class IntercomModelSelect : public select::Select, public Parented<TCBusComponent> {
            public:
                IntercomModelSelect() = default;
            protected:
                void control(const std::string &value) override;
        };

        class IntercomRingtoneDoorCallSelect : public select::Select, public Parented<TCBusComponent> {
            public:
                IntercomRingtoneDoorCallSelect() = default;
            protected:
                void control(const std::string &value) override;
        };

        class IntercomRingtoneFloorCallSelect : public select::Select, public Parented<TCBusComponent> {
            public:
                IntercomRingtoneFloorCallSelect() = default;
            protected:
                void control(const std::string &value) override;
        };

        class IntercomRingtoneInternalCallSelect : public select::Select, public Parented<TCBusComponent> {
            public:
                IntercomRingtoneInternalCallSelect() = default;
            protected:
                void control(const std::string &value) override;
        };

        class IntercomVolumeHandsetNumber : public number::Number, public Parented<TCBusComponent> {
            public:
                IntercomVolumeHandsetNumber() = default;

            protected:
                void control(float value) override;
        };

        class IntercomVolumeRingtoneNumber : public number::Number, public Parented<TCBusComponent> {
            public:
                IntercomVolumeRingtoneNumber() = default;

            protected:
                void control(float value) override;
        };

    }  // namespace tc_bus

}  // namespace esphome