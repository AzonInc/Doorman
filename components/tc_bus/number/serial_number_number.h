#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus.h"

namespace esphome {
namespace tc_bus {

class SerialNumberNumber final : public number::Number, public Parented<TCBusComponent> {
 public:
  SerialNumberNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace tc_bus
}  // namespace esphome