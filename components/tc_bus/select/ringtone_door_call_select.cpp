#include "../protocol.h"
#include "ringtone_door_call_select.h"

namespace esphome {
namespace tc_bus {

void RingtoneDoorCallSelect::control(const std::string &value)
{
    this->publish_state(value);
    uint8_t ringtone = ringtone_to_int(value);
    this->parent_->update_setting(SETTING_RINGTONE_DOOR_CALL, ringtone, 0);
}

}  // namespace tc_bus
}  // namespace esphome