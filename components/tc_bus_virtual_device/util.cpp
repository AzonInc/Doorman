#include "util.h"
#include "esphome.h"
#include "esphome/core/helpers.h"
#include <algorithm>
#include <cctype>

namespace esphome::tc_bus
{
    const char* virtual_device_group_to_string(uint8_t device_group)
    {
        if(device_group == 0 || device_group == VIRTUAL_DEVICE_GROUP_INDOOR_STATION) {
            return "Indoor Station";
        } else if(device_group == VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION) {
            return "Outdoor Station";
        } else {
            return "Unsupported";
        }
    }
}