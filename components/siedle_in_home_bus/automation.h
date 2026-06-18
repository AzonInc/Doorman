#pragma once

#include "esphome/core/automation.h"
#include "siedle_in_home_bus.h"

namespace esphome::siedle_in_home_bus {

template<typename... Ts>
class SiedleInHomeBusSendAction : public Action<Ts...>, public Parented<SiedleInHomeBusComponent> {
 public:
  TEMPLATABLE_VALUE(uint32_t, raw)
  TEMPLATABLE_VALUE(uint8_t, command)
  TEMPLATABLE_VALUE(uint8_t, destination)
  TEMPLATABLE_VALUE(uint8_t, destination_bus)
  TEMPLATABLE_VALUE(uint8_t, source)
  TEMPLATABLE_VALUE(uint8_t, source_bus)

  void play(const Ts &...x) override {
    if (this->raw_.has_value()) {
      this->parent_->send_message(SiedleInHomeBusMessage(this->raw_.value(x...)));
    } else {
      this->parent_->send_message(SiedleInHomeBusMessage(
        this->command_.value(x...),
        this->destination_.value(x...),
        this->destination_bus_.value(x...),
        this->source_.value(x...),
        this->source_bus_.value(x...)
      ));
    }
  }
};

}  // namespace esphome::siedle_in_home_bus
