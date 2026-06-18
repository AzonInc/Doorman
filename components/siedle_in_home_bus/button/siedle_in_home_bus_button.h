#pragma once

#include "esphome/components/button/button.h"
#include "../siedle_in_home_bus.h"

namespace esphome::siedle_in_home_bus {

class SiedleInHomeBusButton : public button::Button {
 public:
  explicit SiedleInHomeBusButton(SiedleInHomeBusComponent *parent, uint32_t raw)
      : parent_(parent), raw_(raw) {}

  void dump_config() override;

 protected:
  void press_action() override;

  SiedleInHomeBusComponent *parent_;
  uint32_t raw_;
};

}  // namespace esphome::siedle_in_home_bus
