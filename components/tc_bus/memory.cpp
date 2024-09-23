#include "memory.h"
#include "esphome.h"
#include "esphome/core/helpers.h"

namespace esphome
{
    namespace tc_bus
    {
        const char* setting_type_to_string(SettingType type)
        {
            switch (type)
            {
                case SETTING_RINGTONE_FLOOR_CALL: return "ringtone_floor_call";
                case SETTING_RINGTONE_DOOR_CALL: return "ringtone_door_call";
                case SETTING_RINGTONE_INTERNAL_CALL: return "ringtone_internal_call";
                case SETTING_RINGTONE_VOLUME: return "volume_ringtone";
                case SETTING_HANDSET_VOLUME: return "volume_handset";
                default: return "UNKNOWN";
            }
        }
    }  // namespace tc_bus
}  // namespace esphome