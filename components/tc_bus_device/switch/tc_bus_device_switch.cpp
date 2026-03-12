#include "tc_bus_device_switch.h"

namespace esphome::tc_bus
{
    void ForceLongDoorOpenerProtocolSwitch::write_state(bool value)
    {
        // for both
        this->publish_state(value);
        this->parent_->update_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL, value);
    }

    void RingtoneMuteSwitch::write_state(bool value)
    {
        // not for virtual
        this->publish_state(value);
        this->parent_->update_setting(SETTING_RINGTONE_MUTE, value);
    }

    void AutoAnswerCallSwitch::write_state(bool value)
    {
        // only for virtual
        this->publish_state(value);
        this->parent_->update_setting(SETTING_AUTO_ANSWER_CALL, value);
    }
}