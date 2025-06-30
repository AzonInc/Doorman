#include "../util.h"
#include "volume_handset_number.h"

namespace esphome {
namespace tc_bus {

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


}  // namespace tc_bus
}  // namespace esphome