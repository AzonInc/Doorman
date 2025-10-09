#pragma once

#include "esphome/components/number/number.h"
#include "../tc_bus_device.h"

namespace esphome
{
  namespace tc_bus
  {
    class SerialNumberNumber final : public number::Number, public Parented<TCBusDeviceComponent> {
      public:
        SerialNumberNumber() = default;

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

  }  // namespace tc_bus
}  // namespace esphome