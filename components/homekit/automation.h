#pragma once
#include "esphome/core/component.h"
#include "esphome/core/automation.h"

namespace esphome
{
  namespace homekit
  {
    class HKIdentifyTrigger : public Trigger<>
    {
    public:
      void process();
    };
  }
}