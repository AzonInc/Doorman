#include "tc_bus_device_switch.h"

namespace esphome::tc_bus
{
    void UseLongDoorOpenerProtocolSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL, value);
    }

    void RingtoneMuteSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_RINGTONE_MUTE, value);
    }

    void AutoAnswerCallSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_AUTO_ANSWER_CALL, value);
    }

    void CallingRequiresDoorReadinessSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_CALLING_REQUIRES_DOOR_READINESS, value);
    }

    void DoorOpenerRequiresActiveCallSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL, value);
    }

    void DoorOpenerRequiresDoorReadinessSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS, value);
    }

    void AddressLockSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_ADDRESS_LOCK, value);
    }

    void CallTimeUnlimitedSwitch::write_state(bool value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_CALL_TIME_UNLIMITED, value);
    }
}