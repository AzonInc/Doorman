#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus.h"

namespace esphome {
namespace tc_bus {

class VolumeHandsetDoorCallNumber final : public number::Number, public Parented<TCBusComponent> {
 public:
  VolumeHandsetDoorCallNumber() = default;

 protected:
  void control(float value) override;
};


class VolumeHandsetInternalCallNumber final : public number::Number, public Parented<TCBusComponent> {
  public:
   VolumeHandsetInternalCallNumber() = default;
 
  protected:
   void control(float value) override;
 };

}  // namespace tc_bus
}  // namespace esphome