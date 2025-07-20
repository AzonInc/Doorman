#pragma once

#include "esphome/components/button/button.h"
#include "../tc_bus_device.h"

namespace esphome
{
  namespace tc_bus
  {
    class ReadMemoryButton final : public button::Button, public Parented<TCBusDeviceComponent>
    {
      public:
        ReadMemoryButton() = default;

      protected:
        void press_action() override;
    };

  }  // namespace tc_bus
}  // namespace esphome