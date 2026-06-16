#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus.h"

namespace esphome::tc_bus
{
    class EntranceAddressNumber final : public number::Number, public Parented<TCBusComponent> {
      public:
        EntranceAddressNumber() = default;

      protected:
        void control(float value) override;
    };

    class SecondEntranceAddressNumber final : public number::Number, public Parented<TCBusComponent> {
      public:
        SecondEntranceAddressNumber() = default;

      protected:
        void control(float value) override;
    };
}