#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus.h"

namespace esphome {
namespace tc_bus {

class VolumeHandsetNumber : public number::Number, public Parented<TCBusComponent> {
 public:
  VolumeHandsetNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace tc_bus
}  // namespace esphome