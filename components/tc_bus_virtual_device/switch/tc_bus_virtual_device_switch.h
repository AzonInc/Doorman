#pragma once

#include "esphome/components/switch/switch.h"
#include "../tc_bus_virtual_device.h"

namespace esphome::tc_bus
{
    class VForceLongDoorOpenerProtocolSwitch final : public switch_::Switch, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VForceLongDoorOpenerProtocolSwitch() = default;

      protected:
        void write_state(bool value) override;
    };

    class VAutoAnswerCallSwitch final : public switch_::Switch, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VAutoAnswerCallSwitch() = default;

      protected:
        void write_state(bool value) override;
    };
}