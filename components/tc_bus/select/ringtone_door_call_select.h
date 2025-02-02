#pragma once

#include "esphome/components/select/select.h"
#include "../tc_bus.h"

namespace esphome {
namespace tc_bus {

class RingtoneEntranceDoorCallSelect : public select::Select, public Parented<TCBusComponent> {
 public:
  RingtoneEntranceDoorCallSelect() = default;

 protected:
  void control(const std::string &value) override;
};

class RingtoneSecondEntranceDoorCallSelect : public select::Select, public Parented<TCBusComponent> {
 public:
  RingtoneSecondEntranceDoorCallSelect() = default;

 protected:
  void control(const std::string &value) override;
};

}  // namespace tc_bus
}  // namespace esphome