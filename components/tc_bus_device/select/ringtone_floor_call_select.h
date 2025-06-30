#pragma once

#include "esphome/components/select/select.h"
#include "../tc_bus_device.h"

namespace esphome {
namespace tc_bus {

class RingtoneFloorCallSelect final : public select::Select, public Parented<TCBusDeviceComponent> {
 public:
  RingtoneFloorCallSelect() = default;

 protected:
  void control(const std::string &value) override;
};

}  // namespace tc_bus
}  // namespace esphome