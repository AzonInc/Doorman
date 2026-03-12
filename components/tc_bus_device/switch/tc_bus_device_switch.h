#pragma once

#include "esphome/components/switch/switch.h"
#include "../tc_bus_device.h"

namespace esphome::tc_bus
{
    class ForceLongDoorOpenerProtocolSwitch final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        ForceLongDoorOpenerProtocolSwitch() = default;

      protected:
        void write_state(bool value) override;
    };

    class RingtoneMuteSwitch final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        RingtoneMuteSwitch() = default;

      protected:
        void write_state(bool value) override;
    };

    class AutoAnswerCallSwitch final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        AutoAnswerCallSwitch() = default;

      protected:
        void write_state(bool value) override;
    };
}