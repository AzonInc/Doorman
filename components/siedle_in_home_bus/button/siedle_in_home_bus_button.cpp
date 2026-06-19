#include "siedle_in_home_bus_button.h"
#include "esphome/core/log.h"

namespace esphome::siedle_in_home_bus {

static const char *const TAG = "siedle_in_home_bus.button";

void SiedleInHomeBusButton::press_action() {
  this->parent_->send_message(SiedleInHomeBusMessage(
    this->command_, this->destination_, this->destination_bus_,
    this->source_, this->source_bus_));
}

void SiedleInHomeBusButton::dump_config() {
  LOG_BUTTON("", "Siedle In-Home Bus Button", this);
  ESP_LOGCONFIG(TAG, "  Command:     0x%02X", this->command_);
  ESP_LOGCONFIG(TAG, "  Destination: 0x%X:0x%02X", this->destination_bus_, this->destination_);
  ESP_LOGCONFIG(TAG, "  Source:      0x%X:0x%02X", this->source_bus_, this->source_);
}

}  // namespace esphome::siedle_in_home_bus
