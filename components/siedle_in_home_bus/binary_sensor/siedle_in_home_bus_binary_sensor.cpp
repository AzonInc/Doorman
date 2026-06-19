#include "siedle_in_home_bus_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome::siedle_in_home_bus {

static const char *const TAG = "siedle_in_home_bus.binary_sensor";

void SiedleInHomeBusBinarySensor::on_message(const SiedleInHomeBusMessage &msg) {
  if (msg.get_command()         != this->command_         ||
      msg.get_destination()     != this->destination_     ||
      msg.get_destination_bus() != this->destination_bus_ ||
      msg.get_source()          != this->source_          ||
      msg.get_source_bus()      != this->source_bus_)
    return;

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
  ESP_LOGCONFIG(TAG, "  Command:     0x%02X", this->command_);
  ESP_LOGCONFIG(TAG, "  Destination: 0x%X:0x%02X", this->destination_bus_, this->destination_);
  ESP_LOGCONFIG(TAG, "  Source:      0x%X:0x%02X", this->source_bus_, this->source_);
}

}  // namespace esphome::siedle_in_home_bus
