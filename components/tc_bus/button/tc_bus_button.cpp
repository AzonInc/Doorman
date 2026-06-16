#include "tc_bus_button.h"

namespace esphome::tc_bus
{
    void SystemDiscoveryButton::press_action()
    {
        this->parent_->discover_system_devices();
    }
}