#pragma once

#include "protocol.h"

#include <utility>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

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

                void set_sn(uint32_t serial_number) { this->serial_number_ = serial_number; }
                void set_sn_lambda(std::function<optional<uint32_t>()> &&serial_number_lambda) { this->serial_number_lambda_ = serial_number_lambda; }

                void set_bus_command_sensor(text_sensor::TextSensor *bus_command) { this->bus_command_ = bus_command; }
                void set_hardware_version_sensor(text_sensor::TextSensor *hardware_version) { this->hardware_version_ = hardware_version; }
                void set_door_readiness_sensor(binary_sensor::BinarySensor *door_readiness) { this->door_readiness_ = door_readiness; }

                void send_command(uint32_t command);
                void send_command_generate(CommandType type, uint8_t address, uint32_t serial_number);

                void publish_command(uint32_t command, bool fire_events);

                void add_received_command_callback(std::function<void(CommandData)> &&callback);
                CallbackManager<void(CommandData)> received_command_callback_{};

                bool sending;

            protected:
                InternalGPIOPin *rx_pin_;
                InternalGPIOPin *tx_pin_;
                const char* event_;

                TCBusComponentStore store_;
                std::vector<TCBusListener *> listeners_{};

                uint32_t serial_number_;
                optional<std::function<optional<uint32_t>()>> serial_number_lambda_;

                text_sensor::TextSensor *bus_command_{nullptr};
                text_sensor::TextSensor *hardware_version_{nullptr};
                binary_sensor::BinarySensor *door_readiness_{nullptr};
                std::string hardware_version_str_ = "Generic";
        };

    }  // namespace tc_bus

}  // namespace esphome