#include "../util.h"
#include "tc_bus_virtual_device_number.h"

namespace esphome::tc_bus
{
    void VSerialNumberNumber::control(float value)
    {
        this->parent_->set_serial_number(value, true);
    }

    void VAddressNumber::control(float value)
    {
        this->parent_->set_address(value, true);
    }

    void VAddressDividerNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->set_address_divider(value);
        this->parent_->save_preferences();
    }

    void VDoorReadinessDurationNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->set_door_readiness_duration(value);
        this->parent_->save_preferences();
    }

    void VCallTimeDurationNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->set_call_time_duration(value);
        this->parent_->save_preferences();
    }

    void VDoorOpenerDurationNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->set_door_opener_duration(value);
        this->parent_->save_preferences();
    }
}