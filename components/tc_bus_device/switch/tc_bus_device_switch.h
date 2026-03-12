#pragma once

#include "esphome/components/switch/switch.h"
#include "../tc_bus_device.h"

namespace esphome::tc_bus
{
    class UseLongDoorOpenerProtocolSwitch final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        UseLongDoorOpenerProtocolSwitch() = default;

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

    class CallingRequiresDoorReadinessSwitch final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        CallingRequiresDoorReadinessSwitch() = default;

      protected:
        void write_state(bool value) override;
    };

    class DoorOpenerRequiresActiveCall final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        DoorOpenerRequiresActiveCall() = default;

      protected:
        void write_state(bool value) override;
    };

    class DoorOpenerRequiresDoorReadiness final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        DoorOpenerRequiresDoorReadiness() = default;

      protected:
        void write_state(bool value) override;
    };

    class AddressLockSwitch final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        AddressLockSwitch() = default;

      protected:
        void write_state(bool value) override;
    };

    class CallTimeUnlimitedSwitch final : public switch_::Switch, public Parented<TCBusDeviceComponent> {
      public:
        CallTimeUnlimitedSwitch() = default;

      protected:
        void write_state(bool value) override;
    };
}