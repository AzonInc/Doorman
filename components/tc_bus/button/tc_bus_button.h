#pragma once

#include "esphome/components/button/button.h"
#include "../tc_bus.h"

namespace esphome::tc_bus
{
    class SystemDiscoveryButton final : public button::Button, public Parented<TCBusComponent>
    {
      public:
        SystemDiscoveryButton() = default;

      protected:
        void press_action() override;
    };
}