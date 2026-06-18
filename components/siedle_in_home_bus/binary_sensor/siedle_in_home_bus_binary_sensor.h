#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "../siedle_in_home_bus.h"

namespace esphome::siedle_in_home_bus {

class SiedleInHomeBusBinarySensor : public binary_sensor::BinarySensor, public SiedleInHomeBusListener {
 public:
  void on_message(const SiedleInHomeBusMessage &msg) override;
  void on_loop() override;
  void dump_config() override;
};

}  // namespace esphome::siedle_in_home_bus
