#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus_device.h"

namespace esphome::tc_bus
{
    class SerialNumberNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        SerialNumberNumber() = default;

      protected:
        void control(float value) override;
    };

    class ParallelSerialNumberNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        ParallelSerialNumberNumber() = default;

      protected:
        void control(float value) override;
    };

    class AddressNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        AddressNumber() = default;

      protected:
        void control(float value) override;
    };

    class VolumeHandsetDoorCallNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        VolumeHandsetDoorCallNumber() = default;
    
      protected:
        void control(float value) override;
    };

    class VolumeHandsetInternalCallNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        VolumeHandsetInternalCallNumber() = default;
      
      protected:
        void control(float value) override;
    };

    class VolumeRingtoneNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        VolumeRingtoneNumber() = default;
      
      protected:
        void control(float value) override;
    };

    class AddressDividerNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        AddressDividerNumber() = default;

      protected:
        void control(float value) override;
    };

    class DoorReadinessDurationNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        DoorReadinessDurationNumber() = default;

      protected:
        void control(float value) override;
    };

    class CallTimeDurationNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        CallTimeDurationNumber() = default;

      protected:
        void control(float value) override;
    };

    class DoorOpenerDurationNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        DoorOpenerDurationNumber() = default;

      protected:
        void control(float value) override;
    };

}