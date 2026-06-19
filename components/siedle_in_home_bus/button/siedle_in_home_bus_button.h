#pragma once

#include "esphome/components/button/button.h"
#include "../siedle_in_home_bus.h"

namespace esphome::siedle_in_home_bus {

class SiedleInHomeBusButton : public button::Button, public Parented<SiedleInHomeBusComponent> {
 public:
  void set_command(uint8_t command) { this->command_ = command; }
  void set_destination(uint8_t destination) { this->destination_ = destination; }
  void set_destination_bus(uint8_t destination_bus) { this->destination_bus_ = destination_bus; }
  void set_source(uint8_t source) { this->source_ = source; }
  void set_source_bus(uint8_t source_bus) { this->source_bus_ = source_bus; }

  void dump_config() override;

 protected:
  void press_action() override;

  uint8_t command_{0};
  uint8_t destination_{0};
  uint8_t destination_bus_{0x08};
  uint8_t source_{0};
  uint8_t source_bus_{0x08};
};

}  // namespace esphome::siedle_in_home_bus
