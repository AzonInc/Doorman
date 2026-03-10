#pragma once

#include "esphome/core/hal.h"

namespace esphome::tc_bus
{
    enum VirtualDeviceGroup {
        VIRTUAL_DEVICE_GROUP_INDOOR_STATION = 1,
        VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION = 2,
    };

    const char* virtual_device_group_to_string(uint8_t device_group);
}