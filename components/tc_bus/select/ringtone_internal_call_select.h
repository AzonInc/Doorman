#pragma once

#include "esphome/components/select/select.h"
#include "../tc_bus.h"

namespace esphome {
namespace tc_bus {

class RingtoneInternalCallSelect final : public select::Select, public Parented<TCBusComponent> {
 public:
  RingtoneInternalCallSelect() = default;

 protected:
  void control(const std::string &value) override;
};

}  // namespace tc_bus
}  // namespace esphome