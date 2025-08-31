#include "../util.h"
#include "tc_bus_device_number.h"

namespace esphome
{
    namespace tc_bus
    {
        void SerialNumberNumber::control(float value)
        {
            this->publish_state(value);
            this->parent_->set_serial_number(value);
            this->parent_->save_preferences();
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

    }  // namespace tc_bus
}  // namespace esphome