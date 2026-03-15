#include "../util.h"
#include "tc_bus_device_number.h"

namespace esphome::tc_bus
{
    void SerialNumberNumber::control(float value)
    {
        this->parent_->set_serial_number(value, true);
    }

    void ParallelSerialNumberNumber::control(float value)
    {
        // entity state is updated in update_setting() after validation
        this->parent_->update_setting(SETTING_PARALLEL_SERIAL_NUMBER, value);
    }

    void AddressNumber::control(float value)
    {
        // entity state is updated in update_setting() after validation
        this->parent_->update_setting(SETTING_ADDRESS, value);
    }

    void VolumeHandsetDoorCallNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_VOLUME_HANDSET_DOOR_CALL, value);
    }

    void VolumeHandsetInternalCallNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL, value);
    }

    void VolumeRingtoneNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_VOLUME_RINGTONE, value);
    }

    void AddressDividerNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_AS_ADDRESS_DIVIDER, value);
    }

    void DoorReadinessDurationNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_DOOR_READINESS_DURATION, value/8);
    }

    void CallTimeDurationNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_CALL_TIME_DURATION, value/8);
    }

    void DoorOpenerDurationNumber::control(float value)
    {
        this->publish_state(value);
        this->parent_->update_setting(SETTING_DOOR_OPENER_DURATION, value);
    }

}