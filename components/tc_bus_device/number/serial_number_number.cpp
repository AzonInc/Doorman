#include "../util.h"
#include "serial_number_number.h"

namespace esphome {
namespace tc_bus {

void SerialNumberNumber::control(float value)
{
    this->publish_state(value);
    this->parent_->set_serial_number(value);
    this->parent_->save_preferences();
}

}  // namespace tc_bus
}  // namespace esphome