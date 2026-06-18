#include "siedle_in_home_bus_button.h"
#include "esphome/core/log.h"

namespace esphome::siedle_in_home_bus {

static const char *const TAG = "siedle_in_home_bus.button";

void SiedleInHomeBusButton::dump_config() {
  LOG_BUTTON("", "Siedle In-Home Bus Button", this);
  SiedleInHomeBusMessage msg(this->raw_);
  ESP_LOGCONFIG(TAG, "  %s", msg.to_string().c_str());
}

void SiedleInHomeBusButton::press_action() {
  this->parent_->send_message(SiedleInHomeBusMessage(this->raw_));
}

}  // namespace esphome::siedle_in_home_bus
