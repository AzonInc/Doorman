#include "automation.h"

namespace esphome
{
  namespace homekit
  {
    void HKIdentifyTrigger::process() {
      this->trigger();
    }
  }
}