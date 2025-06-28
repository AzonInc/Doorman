#include "volume_handset_number.h"

namespace esphome {
namespace tc_bus {

void VolumeHandsetDoorCallNumber::control(float value)
{
    this->publish_state(value);
    this->parent_->update_setting(SETTING_VOLUME_HANDSET_DOOR_CALL, value, 0);
}

void VolumeHandsetInternalCallNumber::control(float value)
{
    this->publish_state(value);
    this->parent_->update_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL, value, 0);
}


}  // namespace tc_bus
}  // namespace esphome