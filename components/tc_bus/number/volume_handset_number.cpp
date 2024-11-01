#include "volume_handset_number.h"

namespace esphome {
namespace tc_bus {

void VolumeHandsetNumber::control(float value)
{
    this->publish_state(value);
    this->parent_->update_setting(SETTING_VOLUME_HANDSET, value, 0);
}

}  // namespace tc_bus
}  // namespace esphome