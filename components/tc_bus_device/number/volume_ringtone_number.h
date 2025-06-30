#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus_device.h"

namespace esphome {
namespace tc_bus {

class VolumeRingtoneNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
 public:
  VolumeRingtoneNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace tc_bus
}  // namespace esphome