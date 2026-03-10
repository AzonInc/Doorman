#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus_virtual_device.h"

namespace esphome::tc_bus
{
    class VSerialNumberNumber final : public number::Number, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VSerialNumberNumber() = default;

      protected:
        void control(float value) override;
    };

    class VAddressNumber final : public number::Number, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VAddressNumber() = default;

      protected:
        void control(float value) override;
    };

    class VAddressDividerNumber final : public number::Number, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VAddressDividerNumber() = default;

      protected:
        void control(float value) override;
    };

    class VDoorReadinessDurationNumber final : public number::Number, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VDoorReadinessDurationNumber() = default;

      protected:
        void control(float value) override;
    };

    class VCallTimeDurationNumber final : public number::Number, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VCallTimeDurationNumber() = default;

      protected:
        void control(float value) override;
    };

    class VDoorOpenerDurationNumber final : public number::Number, public Parented<TCBusVirtualDeviceComponent> {
      public:
        VDoorOpenerDurationNumber() = default;

      protected:
        void control(float value) override;
    };
}