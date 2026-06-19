#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/automation.h"
#include "../siedle_in_home_bus.h"

namespace esphome::siedle_in_home_bus
{

    class SiedleInHomeBusBinarySensor : public binary_sensor::BinarySensor, public SiedleInHomeBusListener
    {
        public:
            template<typename T> void set_command(T v) { this->command_ = v; }
            template<typename T> void set_destination(T v) { this->destination_ = v; }
            template<typename T> void set_destination_bus(T v) { this->destination_bus_ = v; }
            template<typename T> void set_source(T v) { this->source_ = v; }
            template<typename T> void set_source_bus(T v) { this->source_bus_ = v; }

            void on_message(const SiedleInHomeBusMessage &msg) override;
            void on_loop() override;
            void dump_config() override;

        protected:
            TemplatableValue<uint8_t> command_{};
            TemplatableValue<uint8_t> destination_{};
            TemplatableValue<uint8_t> destination_bus_{};
            TemplatableValue<uint8_t> source_{};
            TemplatableValue<uint8_t> source_bus_{};
    };

}  // namespace esphome::siedle_in_home_bus
