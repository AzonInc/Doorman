#pragma once

#include "esphome/components/select/select.h"
#include "../tc_bus_device.h"

namespace esphome
{
  namespace tc_bus
  {
    class ModelSelect final : public select::Select, public Parented<TCBusDeviceComponent> {
      public:
        ModelSelect() = default;
    
      protected:
        void control(const std::string &value) override;
    };
    
    class RingtoneEntranceDoorCallSelect final : public select::Select, public Parented<TCBusDeviceComponent> {
      public:
        RingtoneEntranceDoorCallSelect() = default;

      protected:
        void control(const std::string &value) override;
    };

    class RingtoneSecondEntranceDoorCallSelect final : public select::Select, public Parented<TCBusDeviceComponent> {
      public:
        RingtoneSecondEntranceDoorCallSelect() = default;

      protected:
        void control(const std::string &value) override;
    };

    class RingtoneFloorCallSelect final : public select::Select, public Parented<TCBusDeviceComponent> {
      public:
        RingtoneFloorCallSelect() = default;
    
      protected:
        void control(const std::string &value) override;
    };

    class RingtoneInternalCallSelect final : public select::Select, public Parented<TCBusDeviceComponent> {
      public:
        RingtoneInternalCallSelect() = default;
    
      protected:
        void control(const std::string &value) override;
    };

  }  // namespace tc_bus
}  // namespace esphome