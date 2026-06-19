#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/automation.h"
#include "../siedle_in_home_bus.h"

namespace esphome::siedle_in_home_bus
{

    class SiedleInHomeBusButton : public button::Button, public Parented<SiedleInHomeBusComponent>
    {
        public:
            template<typename T> void set_command(T v) { this->command_ = v; }
            template<typename T> void set_destination(T v) { this->destination_ = v; }
            template<typename T> void set_destination_bus(T v) { this->destination_bus_ = v; }
            template<typename T> void set_source(T v) { this->source_ = v; }
            template<typename T> void set_source_bus(T v) { this->source_bus_ = v; }

            void dump_config() override;

        protected:
            void press_action() override;

            TemplatableValue<uint8_t> command_{};
            TemplatableValue<uint8_t> destination_{};
            TemplatableValue<uint8_t> destination_bus_{};
            TemplatableValue<uint8_t> source_{};
            TemplatableValue<uint8_t> source_bus_{};
    };

}  // namespace esphome::siedle_in_home_bus
