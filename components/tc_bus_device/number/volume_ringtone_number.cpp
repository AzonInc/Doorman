#include "volume_ringtone_number.h"

namespace esphome {
namespace tc_bus {

void VolumeRingtoneNumber::control(float value)
{
    this->publish_state(value);
    this->parent_->update_setting(SETTING_VOLUME_RINGTONE, value, 0);
}

}  // namespace tc_bus
}  // namespace esphome