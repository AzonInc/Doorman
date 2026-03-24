#include "tc_bus_number.h"

namespace esphome::tc_bus
{
    void EntranceAddressNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->set_entrance_address(value);
        this->parent_->save_preferences();
    }

    void SecondEntranceAddressNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->set_second_entrance_address(value);
        this->parent_->save_preferences();
    }
}