#pragma once

#include "esphome/core/hal.h"

namespace esphome
{
    namespace tc_bus
    {
        enum SettingType {
            SETTING_RINGTONE_FLOOR_CALL,
            SETTING_RINGTONE_DOOR_CALL,
            SETTING_RINGTONE_INTERNAL_CALL,
            SETTING_RINGTONE_VOLUME,
            SETTING_HANDSET_VOLUME
        };

        struct Settings {
            uint8_t handset_volume = 4;
            uint8_t ringtone_volume = 4;
            uint8_t door_call_ringtone = 0;
            uint8_t internal_call_ringtone = 0;
            uint8_t floor_call_ringtone = 0;
        };
    }
}