#include "siedle_in_home_bus_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome::siedle_in_home_bus {

static const char *const TAG = "siedle_in_home_bus.binary_sensor";

void SiedleInHomeBusBinarySensor::on_message(const SiedleInHomeBusMessage &msg) {
  this->publish_state(true);
  if (this->auto_reset_ms_ > 0)
    this->auto_reset_timer_ = millis() + this->auto_reset_ms_;
}

void SiedleInHomeBusBinarySensor::on_loop() {
  if (!this->has_state() || !this->state || this->auto_reset_ms_ == 0)
    return;
  if (millis() >= this->auto_reset_timer_)
    this->publish_state(false);
}

void SiedleInHomeBusBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "Siedle In-Home Bus Binary Sensor", this);
  SiedleInHomeBusMessage msg(this->get_message_raw());
  ESP_LOGCONFIG(TAG, "  %s", msg.to_string().c_str());
}

}  // namespace esphome::siedle_in_home_bus
