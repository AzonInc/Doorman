#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus.h"

namespace esphome {
namespace tc_bus {

class VolumeRingtoneNumber final : public number::Number, public Parented<TCBusComponent> {
 public:
  VolumeRingtoneNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace tc_bus
}  // namespace esphome