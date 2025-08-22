#pragma once

#include "esphome/components/button/button.h"
#include "../homekit_bridge.h"

namespace esphome {
namespace homekit {

class ResetButton : public button::Button, public Parented<HomeKitBridgeComponent> {
 public:
  ResetButton() = default;

 protected:
  void press_action() override;
};

}  // namespace homekit
}  // namespace esphome
