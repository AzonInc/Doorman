#pragma once

#include "esphome/components/switch/switch.h"
#include "../tc_bus.h"

namespace esphome {
namespace tc_bus {

class ForceLongDoorOpenerSwitch final : public switch_::Switch, public Parented<TCBusComponent> {
 public:
  ForceLongDoorOpenerSwitch() = default;

 protected:
  void write_state(bool value) override;
};

}  // namespace tc_bus
}  // namespace esphome