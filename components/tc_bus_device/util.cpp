#include "util.h"
#include "esphome.h"
#include "esphome/core/helpers.h"
#include <algorithm>
#include <cctype>

namespace esphome::tc_bus
{
    const SettingMapping setting_mappings[] = {
        {SETTING_RINGTONE_FLOOR_CALL, "RINGTONE_FLOOR_CALL"},
        {SETTING_RINGTONE_ENTRANCE_DOOR_CALL, "RINGTONE_ENTRANCE_DOOR_CALL"},
        {SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL, "RINGTONE_SECOND_ENTRANCE_DOOR_CALL"},
        {SETTING_RINGTONE_INTERNAL_CALL, "RINGTONE_INTERNAL_CALL"},
        {SETTING_VOLUME_RINGTONE, "VOLUME_RINGTONE"},
        {SETTING_VOLUME_HANDSET_DOOR_CALL, "VOLUME_HANDSET_DOOR_CALL"},
        {SETTING_VOLUME_HANDSET_INTERNAL_CALL, "VOLUME_HANDSET_INTERNAL_CALL"},
        {SETTING_AS_ADDRESS_DIVIDER, "AS_ADDRESS_DIVIDER"},
        {SETTING_VAS_ADDRESS_DIVIDER, "VAS_ADDRESS_DIVIDER"},
        {SETTING_USE_LONG_DOOR_OPENER_PROTOCOL, "USE_LONG_DOOR_OPENER_PROTOCOL"},
        {SETTING_AMBIENT_LIGHT_IN_STANDBY, "AMBIENT_LIGHT_IN_STANDBY"},
        {SETTING_RINGTONE_MUTE, "RINGTONE_MUTE"},
        {SETTING_DOOR_OPENER_DURATION, "DOOR_OPENER_DURATION"},
        {SETTING_ADDRESS, "ADDRESS"},
        {SETTING_ADDRESS_LOCK, "ADDRESS_LOCK"},
        {SETTING_CALLING_REQUIRES_DOOR_READINESS, "CALLING_REQUIRES_DOOR_READINESS"},
        {SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS, "DOOR_OPENER_REQUIRES_DOOR_READINESS"},
        {SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL, "DOOR_OPENER_REQUIRES_ACTIVE_CALL"},
        {SETTING_DOOR_READINESS_DURATION, "DOOR_READINESS_DURATION"},
        {SETTING_CALL_TIME_DURATION, "CALL_TIME_DURATION"},
        {SETTING_BUTTON_ROWS, "BUTTON_ROWS"},
        {SETTING_HAS_CODE_LOCK, "HAS_CODE_LOCK"},
        {SETTING_AUTO_ANSWER_CALL, "AUTO_ANSWER_CALL"},
        {SETTING_CALL_TIME_UNLIMITED, "CALL_TIME_UNLIMITED"},
        {SETTING_PARALLEL_SERIAL_NUMBER, "PARALLEL_SERIAL_NUMBER"},
    };

    SettingType string_to_setting_type(const char* str)
    {
        if (!str) return SETTING_UNKNOWN;

        for (const auto& mapping : setting_mappings)
        {
            const char* a = str;
            const char* b = mapping.name;
            while (*a && *b)
            {
                if (toupper(*a) != toupper(*b))
                {
                    break;
                }
                ++a; ++b;
            }
            if (*a == '\0' && *b == '\0')
            {
                return mapping.type;
            }
        }
        return SETTING_UNKNOWN;
    }

    const char* setting_type_to_string(SettingType type)
    {
        for (const auto& mapping : setting_mappings)
        {
            if (mapping.type == type)
            {
                return mapping.name;
            }
        }
        return "UNKNOWN";
    }

    const DoorbellButtonActionMapping doorbell_button_action_mappings[] = {
        {DOORBELL_BUTTON_ACTION_NONE, "NONE"},
        {DOORBELL_BUTTON_ACTION_DOOR_CALL, "DOOR_CALL"},
        {DOORBELL_BUTTON_ACTION_LIGHT, "LIGHT"},
        {DOORBELL_BUTTON_ACTION_CONTROL_FUNCTION, "CONTROL_FUNCTION"},
    };

    DoorbellButtonAction string_to_doorbell_button_action(const char* str)
    {
        if (!str)
        {
            return DOORBELL_BUTTON_ACTION_NONE;
        }

        for (const auto& mapping : doorbell_button_action_mappings)
        {
            const char* a = str;
            const char* b = mapping.name;
            while (*a && *b)
            {
                if (toupper(*a) != toupper(*b))
                {
                    break;
                }
                ++a; ++b;
            }
            if (*a == '\0' && *b == '\0')
            {
                return mapping.action;
            }
        }
        return DOORBELL_BUTTON_ACTION_NONE;
    }

    const char* doorbell_button_action_to_string(DoorbellButtonAction action)
    {
        for (const auto& mapping : doorbell_button_action_mappings)
        {
            if (mapping.action == action)
            {
                return mapping.name;
            }
        }
        return "NONE";
    }

    const char* device_group_to_string(uint8_t device_group)
    {
        if(device_group == DEVICE_GROUP_INDOOR_STATION_CLASSIC)
        {
            return "Classic Indoor Station";
        }
        else if(device_group == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
        {
            return "Handsfree Indoor Station";
        }
        else if(device_group == DEVICE_GROUP_OUTDOOR_STATION)
        {
            return "Outdoor Station";
        }
        else if(device_group == DEVICE_GROUP_CONTROLLER)
        {
            return "Controller";
        }
        else if(device_group == DEVICE_GROUP_EXTENSION)
        {
            return "Extension";
        }
        else
        {
            return "Unknown";
        }
    }

    static const ModelEntry MODEL_TABLE[] = {
        // Group 0
        { MODEL_IS_ISH3030,         0x000, 0, UINT16_MAX, 0 },
        { MODEL_IS_ISH3230,         0x001, 0, UINT16_MAX, 0 },
        { MODEL_IS_ISH3022,         0x002, 0, UINT16_MAX, 0 },
        { MODEL_IS_ISH3130,         0x003, 0, UINT16_MAX, 0 },
        { MODEL_IS_IVH3222,         0x800, 0, UINT16_MAX, 0 },
        { MODEL_IS_IVH4222,         0x900, 0, UINT16_MAX, 0 },
        { MODEL_IS_IMM1000,         0xB00, 0, UINT16_MAX, 0 },
        { MODEL_IS_VMH,             0xC01, 0, UINT16_MAX, 0 },
        { MODEL_IS_VML,             0xC00, 0, UINT16_MAX, 0 },
        { MODEL_IS_VMF,             0xC02, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x180, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x181, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x182, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x183, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x184, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x185, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x186, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x187, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x188, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x189, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x18A, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x18B, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x18C, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x18D, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x18E, 0, UINT16_MAX, 0 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x18F, 0, UINT16_MAX, 0 },

        // Group 1
        { MODEL_IS_ISW3030,         0x010, 0, UINT16_MAX, 1 },
        { MODEL_IS_ISW3230,         0x011, 0, UINT16_MAX, 1 },
        { MODEL_IS_ISW3130,         0x013, 0, UINT16_MAX, 1 },
        { MODEL_IS_ISW3330,         0x015, 0, UINT16_MAX, 1 },
        { MODEL_IS_ISW3340,         0x017, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW9030,         0x194, 0, UINT16_MAX, 1 },
        { MODEL_IS_ISW42X0,         0x400, 0, UINT16_MAX, 1 },
        { MODEL_IS_TKM_IS,          0x410, 0, UINT16_MAX, 1 },
        { MODEL_IS_TKM_ISV,         0x420, 0, UINT16_MAX, 1 },
        { MODEL_IS_ISW4100,         0x200, 0, UINT16_MAX, 1 },
        { MODEL_IS_IMM2100,         0x201, 0, UINT16_MAX, 1 },
        { MODEL_IS_CAIXXXX,         0x208, 0, UINT16_MAX, 1 },
        { MODEL_IS_TC40V2,          0x280, 512, UINT16_MAX, 1 },
        { MODEL_IS_TC40,            0x280, 0,  511,         1 },
        { MODEL_IS_VTC42V2,         0x281, 512, UINT16_MAX, 1 },
        { MODEL_IS_VTC40,           0x281, 0,  511,         1 },
        { MODEL_IS_ECOOS,           0x800, 0, UINT16_MAX, 1 },
        { MODEL_IS_ECOOS,           0x805, 0, UINT16_MAX, 1 },
        { MODEL_IS_ECOOS,           0x807, 0, UINT16_MAX, 1 },
        { MODEL_IS_CAI2000,         0x809, 0, UINT16_MAX, 1 },
        { MODEL_IS_ECOOS,           0x80C, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW2220,         0x810, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW2221,         0x815, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW3011,         0x820, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW3012,         0x830, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW9010,         0x1E8, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW9110,         0x1EA, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVW9011,         0x1E9, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVE70,           0x1B3, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVE70,           0x1B4, 0, UINT16_MAX, 1 },
        { MODEL_IS_IVE70,           0x1B5, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x020, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x021, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x022, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x023, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x024, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x025, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x026, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x027, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x030, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x031, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x032, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x033, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x034, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x035, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x036, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x037, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x028, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x02B, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x02F, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x068, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x06F, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_AUDIO,     0x060, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x038, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x039, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x03A, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x03B, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x03C, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x03D, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x03E, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_VIDEO,     0x03F, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x070, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x071, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x072, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x073, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x074, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x075, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x076, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x077, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x078, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x079, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x07A, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x07B, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x07C, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x07D, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x07E, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_AUDIO, 0x07F, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x080, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x081, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x082, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x083, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x084, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x085, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x086, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0x087, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x088, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x089, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x08A, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x08B, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x08C, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x08D, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x08E, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0x08F, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x058, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x059, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x05A, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x05B, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x05C, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x05D, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x05E, 0, UINT16_MAX, 1 },
        { MODEL_IS_TASTA_PRO_VIDEO, 0x05F, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC70, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC71, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC72, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC73, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC74, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC75, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC76, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC77, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC90, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC91, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC92, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC93, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC94, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC95, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC96, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_AUDIO, 0xC97, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC80, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC81, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC82, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC83, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC84, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC85, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC86, 0, UINT16_MAX, 1 },
        { MODEL_IS_SENSO_PRO_VIDEO, 0xC87, 0, UINT16_MAX, 1 },

        // Group 2
        { MODEL_AS_PUK,            0x420,    0, 2623,       2 },
        { MODEL_AS_PDS0X,          0x420, 2624, UINT16_MAX, 2 },
        { MODEL_AS_PES,            0x430, 2656, UINT16_MAX, 2 },
        { MODEL_AS_PUK_DSP,        0x4E0,    0, UINT16_MAX, 2 },
        { MODEL_AS_PAKV2,          0x400,    0, UINT16_MAX, 2 },
        { MODEL_AS_PAKV3,          0x280,    0, UINT16_MAX, 2 },
        { MODEL_AS_PAKV3,          0x270,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3,           0xC00,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3_EX1,       0xC01,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3_EX2,       0xC02,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3_EX3,       0xC03,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3_EX4,       0xC04,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3_EX5,       0xC05,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3_EX6,       0xC06,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3,           0xC09,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU3,           0xD09,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU4,           0xC20,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU4_EX1,       0xC21,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU4_EX2,       0xC22,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU4_EX3,       0xC23,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU4_EX4,       0xC24,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU4_EX5,       0xC25,    0, UINT16_MAX, 2 },
        { MODEL_AS_TCU4_EX6,       0xC26,    0, UINT16_MAX, 2 },
        { MODEL_AS_TKM_AS,         0x480,    0, UINT16_MAX, 2 },
        { MODEL_AS_TKM_ASV,        0x4C0,    0, UINT16_MAX, 2 },

        // Group 4
        { MODEL_CTRL_BVS30,        0x008, 0, UINT16_MAX, 4 },
        { MODEL_CTRL_NBV3210,      0x010, 0, UINT16_MAX, 4 },
        { MODEL_CTRL_VBVS30,       0x009, 0, UINT16_MAX, 4 },
        { MODEL_CTRL_NBV2600,      0xD2D, 0, UINT16_MAX, 4 },
        { MODEL_CTRL_VBVS05,       0x000, 0, UINT16_MAX, 4 },

        // Group 6
        { MODEL_EXT_BRE2_SG,      0x300, 0, UINT16_MAX, 6 },
        { MODEL_EXT_BRE2_EB,      0x400, 0, UINT16_MAX, 6 },
        { MODEL_EXT_TRE2,         0x500, 0, UINT16_MAX, 6 },
        { MODEL_EXT_TOER2_EB,     0x510, 0, UINT16_MAX, 6 },
        { MODEL_EXT_FFL1000,      0x520, 0, UINT16_MAX, 6 },
        { MODEL_EXT_FAA1200,      0x530, 0, UINT16_MAX, 6 },
        
    };

    Model identifier_to_model(uint8_t device_group, uint16_t model_key, uint8_t hw_version, uint16_t fw_version)
    {
        for (const ModelEntry& e : MODEL_TABLE)
        {
            if (e.device_group == device_group && e.model_key == model_key && fw_version >= e.fw_min && fw_version <= e.fw_max)
            {
                return e.model;
            }
        }
        return MODEL_NONE;
    }

    uint16_t model_to_identifier(Model model)
    {
        for (const ModelEntry& e : MODEL_TABLE)
        {
            if (e.model == model)
            {
                return e.model_key;
            }
        }
        return 0x000;
    }

    const ModelMapping model_mappings[] = {
        {MODEL_IS_ISW3030, "TCS ISW3030 / Koch TC50 / Scantron Stilux"},
        {MODEL_IS_ISW3130, "TCS ISW3130 / Koch TC50P"},
        {MODEL_IS_ISW3230, "TCS ISW3230 / Koch TC50 GFA"},
        {MODEL_IS_ISW3330, "TCS ISW3330 / Koch TC50 BW"},
        {MODEL_IS_ISW3340, "TCS ISW3340"},
        {MODEL_IS_TASTA_AUDIO, "TCS TASTA Audio / Koch 60 Series Audio"},
        {MODEL_IS_TASTA_PRO_AUDIO, "TCS TASTA PRO Audio"},
        {MODEL_IS_TASTA_VIDEO, "TCS TASTA Video / Koch 60 Series Video / Scantron VIVO"},
        {MODEL_IS_TASTA_PRO_VIDEO, "TCS TASTA PRO Video"},
        {MODEL_IS_SENSO_PRO_AUDIO, "TCS SENSO PRO Audio / Koch 70 Series Audio"},
        {MODEL_IS_SENSO_PRO_VIDEO, "TCS SENSO PRO Video / Koch 70 Series Video"},
        {MODEL_IS_ECOOS, "TCS ECOOS / Koch ecoos / Scantron SLIM60T"},
        {MODEL_IS_ISW4100, "TCS ISW4100 / Koch TC31"},
        {MODEL_IS_IMM2100, "TCS IMM2100 / Koch TCE31"},
        {MODEL_IS_VTC42V2, "TCS VTC42V2"},
        {MODEL_IS_TC40V2, "TCS TC40V2"},
        {MODEL_IS_VTC40, "TCS VTC40"},
        {MODEL_IS_TC40, "TCS TC40"},
        {MODEL_IS_TC2000, "TCS TC2000"},
        {MODEL_IS_TC20P, "TCS TC20P"},
        {MODEL_IS_TC20F, "TCS TC20F"},
        {MODEL_IS_ISH3022, "TCS ISH3022"},
        {MODEL_IS_ISH3130, "TCS ISH3130 / Koch TCH50P / Scantron LuxPlus"},
        {MODEL_IS_ISH3230, "TCS ISH3230 / Koch TCH50 GFA"},
        {MODEL_IS_ISH3030, "TCS ISH3030 / Koch TCH50 / Scantron Lux2"},
        {MODEL_IS_ISH1030, "TCS ISH1030 / Koch TTS25"},
        {MODEL_IS_TTCXX, "TCS TTC-XX"},
        {MODEL_IS_TTSXX, "TCS TTS-XX"},
        {MODEL_IS_IMM1000, "TCS IMM1000 / Koch TCH30"},
        {MODEL_IS_IMM1100, "TCS IMM1100 / Koch TCHE30"},
        {MODEL_IS_IMM1300, "TCS IMM1300 / Koch VTCH30"},
        {MODEL_IS_IMM1500, "TCS IMM1500"},
        {MODEL_IS_IMM1310, "TCS IMM1310 / Koch VTCHE30"},
        {MODEL_IS_IMM1110, "TCS IMM1110 / Koch TCHEE30"},
        {MODEL_IS_IVH3222, "TCS IVH3222 / Koch VTCH50 / Scantron VLux"},
        {MODEL_IS_IVH4222, "TCS IVH4222 / Koch VTCH50/2D"},
        {MODEL_IS_IVW2220, "TCS IVW2220 / Koch Sky"},
        {MODEL_IS_IVW2221, "TCS IVW2221 / Koch Sky R1.00"},
        {MODEL_IS_IVW3011, "TCS IVW3011 / Koch Skyline Plus"},
        {MODEL_IS_IVW3012, "TCS IVW3012 / Koch Skyline/Aldup"},
        {MODEL_IS_VMH, "TCS VMH / Koch VMH"},
        {MODEL_IS_VML, "TCS VML / Koch VML"},
        {MODEL_IS_VMF, "TCS VMF / Koch VMF"},
        {MODEL_IS_TKM_IS, "Jung TKM IS"},
        {MODEL_IS_TKM_ISV, "Jung TKM ISV"},
        {MODEL_IS_CAIXXXX, "TCS CAIXXXX / Koch CAIXXXX"},
        {MODEL_IS_CAI2000, "TCS CAI2000 / Koch Carus"},
        {MODEL_IS_ISW42X0, "TCS ISW42X0"},
        {MODEL_IS_IVW9010, "TCS IVW9010"},
        {MODEL_IS_IVW9011, "TCS IVW9011 / Koch VTP10"},
        {MODEL_IS_IVW9110, "TCS IVW9110"},
        {MODEL_IS_IVW9030, "TCS IVW9030 / Scantron SLIM50T"},
        {MODEL_IS_IVE70, "TCS IVE70"},
        {MODEL_IS_DEBUG_0, "DEBUG IS Classic"},
        {MODEL_IS_DEBUG_1, "DEBUG IS Handsfree"},
        {MODEL_AS_DEBUG, "DEBUG AS"},
        {MODEL_AS_PUK, "TCS PUK"},
        {MODEL_AS_PUK_DSP, "TCS PUK-DSP"},
        {MODEL_AS_PAKV2, "TCS PAKV2"},
        {MODEL_AS_PAKV3, "TCS PAKV3"},
        {MODEL_AS_PDS0X, "TCS PDS0X"},
        {MODEL_AS_PDS0X04, "TCS PDS0X/04"},
        {MODEL_AS_PES, "TCS PES"},
        {MODEL_AS_TCU2, "TCS TCU2"},
        {MODEL_AS_TCU3, "TCS TCU3"},
        {MODEL_AS_TCU3_EX1, "TCS TCU3 + TCKE3 (1)"},
        {MODEL_AS_TCU3_EX2, "TCS TCU3 + TCKE3 (2)"},
        {MODEL_AS_TCU3_EX3, "TCS TCU3 + TCKE3 (3)"},
        {MODEL_AS_TCU3_EX4, "TCS TCU3 + TCKE3 (4)"},
        {MODEL_AS_TCU3_EX5, "TCS TCU3 + TCKE3 (5)"},
        {MODEL_AS_TCU3_EX6, "TCS TCU3 + TCKE3 (6)"},
        {MODEL_AS_TCU4, "TCS TCU4"},
        {MODEL_AS_TCU4_EX1, "TCS TCU4 + TCKE3 (1)"},
        {MODEL_AS_TCU4_EX2, "TCS TCU4 + TCKE3 (2)"},
        {MODEL_AS_TCU4_EX3, "TCS TCU4 + TCKE3 (3)"},
        {MODEL_AS_TCU4_EX4, "TCS TCU4 + TCKE3 (4)"},
        {MODEL_AS_TCU4_EX5, "TCS TCU4 + TCKE3 (5)"},
        {MODEL_AS_TCU4_EX6, "TCS TCU4 + TCKE3 (6)"},
        {MODEL_AS_TKM_AS, "Jung TKM AS"},
        {MODEL_AS_TKM_ASV, "Jung TKM ASV"},
        {MODEL_CTRL_BVS20, "TCS BVS20"},
        {MODEL_CTRL_BVS30, "TCS BVS30"},
        {MODEL_CTRL_NBV3210, "TCS NBV3210"},
        {MODEL_CTRL_VBVS30, "TCS VBVS30"},
        {MODEL_CTRL_NBV2600, "TCS NBV2600"},
        {MODEL_CTRL_VBVS05, "TCS VBVS05"},
        {MODEL_CTRL_DEBUG, "DEBUG CONTROLLER"},
        {MODEL_EXT_BRE2_SG, "TCS BRE2-SG"},
        {MODEL_EXT_BRE2_EB, "TCS BRE2-EB"},
        {MODEL_EXT_TRE2, "TCS TRE2"},
        {MODEL_EXT_TOER2_EB, "TCS TOER2-EB"},
        {MODEL_EXT_FFL1000, "TCS FFL1000"},
        {MODEL_EXT_FAA1200, "TCS FAA1200"},
        {MODEL_EXT_DEBUG, "DEBUG EXTENSION"}
    };

    Model string_to_model(const char* str)
    {
        if (!str)
        {
            return MODEL_NONE;
        }

        for (const auto& mapping : model_mappings)
        {
            if (strcmp(str, mapping.name) == 0)
            {
                return mapping.model;
            }
        }
        return MODEL_NONE;
    }

    const char* model_to_string(Model model)
    {
        for (const auto& mapping : model_mappings)
        {
            if (mapping.model == model)
            {
                return mapping.name;
            }
        }
        return "None";
    }

    ModelData getModelData(Model model)
    {
        ModelData modelData{};
        modelData.model = model;
        modelData.capabilities = 0;

        switch (model)
        {
            // Group 1
            case MODEL_IS_ISW3030: /* TC50 */
                modelData.device_group = 1;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_ISW3130: /* TC50P */
                modelData.device_group = 1;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_ISW3230: /* TC50 GFA */
                modelData.device_group = 1;
                modelData.memory_size = 40;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_ISW3330: /* TC50 BW */
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_ISW3340:
                modelData.device_group = 1;
                modelData.memory_size = 128;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_TASTA_AUDIO: /* Koch 60 Series Audio */
            case MODEL_IS_TASTA_PRO_AUDIO:
                modelData.device_group = 1;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL;
                modelData.capabilities |= CAP_SETTING_AMBIENT_LIGHT_IN_STANDBY;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_INDIVIDUAL_RESET;
                modelData.capabilities |= CAP_ALT_RINGTONE_ADDRESS;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_TASTA_VIDEO: /* Koch 60 Series Video */
            case MODEL_IS_TASTA_PRO_VIDEO:
                modelData.device_group = 1;
                modelData.memory_size = 48;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL;
                modelData.capabilities |= CAP_SETTING_AMBIENT_LIGHT_IN_STANDBY;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_INDIVIDUAL_RESET;
                modelData.capabilities |= CAP_ALT_RINGTONE_ADDRESS;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_SENSO_PRO_AUDIO:  /* Koch 70 Series Audio */
                modelData.device_group = 1;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_INDIVIDUAL_RESET;
                modelData.capabilities |= CAP_ALT_RINGTONE_ADDRESS;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_SENSO_PRO_VIDEO:  /* Koch 70 Series Video */
                modelData.device_group = 1;
                modelData.memory_size = 48;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_INDIVIDUAL_RESET;
                modelData.capabilities |= CAP_ALT_RINGTONE_ADDRESS;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_ECOOS:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                // Supported? Not documented
                //modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                break;
            case MODEL_IS_ISW4100: /* TC31 */
                modelData.device_group = 1;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_IMM2100: /* TCE31 */
                modelData.device_group = 1;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_VTC42V2:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_TC40V2:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_VTC40:
                modelData.device_group = 1;
                modelData.memory_size = 40;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                // not available in v1
                //modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_TC40:
                modelData.device_group = 1;
                modelData.memory_size = 40;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                // not available in v1
                //modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_TC2000:
                modelData.device_group = 1;
                modelData.memory_size = 16;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_TC20P:
                modelData.device_group = 1;
                modelData.memory_size = 16;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_TC20F:
                modelData.device_group = 1;
                modelData.memory_size = 16;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IVW2220:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_IVW2221:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_IVW3011:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_IVW3012:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_TKM_IS:
                modelData.device_group = 1;
                modelData.memory_size = 40;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                modelData.capabilities |= CAP_SETTING_AMBIENT_LIGHT_IN_STANDBY;
                break;
            case MODEL_IS_TKM_ISV:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                modelData.capabilities |= CAP_SETTING_AMBIENT_LIGHT_IN_STANDBY;
                break;
            case MODEL_IS_CAI2000:
                modelData.device_group = 1;
                modelData.memory_size = 64;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_CAIXXXX:
                modelData.device_group = 1;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;
            case MODEL_IS_ISW42X0:
                modelData.device_group = 1;
                modelData.memory_size = 40;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                modelData.capabilities |= CAP_SETTING_AUTO_ANSWER_CALL;
                break;

            // Group 0
            case MODEL_IS_ISH3022:
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_ISH3130: /* TCH50P */
                modelData.device_group = 0;
                modelData.memory_size = 40;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                break;
            case MODEL_IS_ISH3230: /* TCH50 GFA */
                modelData.device_group = 0;
                modelData.memory_size = 40;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                break;
            case MODEL_IS_ISH3030: /* TCH50 */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_UNLIMITED;
                break;
            case MODEL_IS_ISH1030: /* TTS25 */
                modelData.device_group = 0;
                modelData.memory_size = 16;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_TTCXX:
                modelData.device_group = 0;
                modelData.memory_size = 16;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_TTSXX:
                modelData.device_group = 0;
                modelData.memory_size = 16;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IMM1000: /* TCH30 */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IMM1100: /* TCHE30 */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IMM1300: /* VTCH30 */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IMM1500:
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IMM1310: /* VTCHE30 */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IMM1110: /* TCHEE30 */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IVH3222: /* VTCH50 */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_IVH4222: /* VTCH50/2D */
                modelData.device_group = 0;
                modelData.memory_size = 32;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_VMH:
                modelData.device_group = 0;
                modelData.memory_size = 24;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_VML:
                modelData.device_group = 0;
                modelData.memory_size = 24;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;
            case MODEL_IS_VMF:
                modelData.device_group = 0;
                modelData.memory_size = 24;
                modelData.capabilities |= CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_INTERNAL_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_FLOOR_CALL;
                modelData.capabilities |= CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_VOLUME_RINGTONE;
                modelData.capabilities |= CAP_SETTING_VOLUME_HANDSET_DOOR_CALL;
                modelData.capabilities |= CAP_SETTING_AS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_VAS_ADDRESS_DIVIDER;
                modelData.capabilities |= CAP_SETTING_RINGTONE_MUTE;
                modelData.capabilities |= CAP_SETTING_PARALLEL_SERIAL_NUMBER;
                break;

            case MODEL_IS_IVW9010:
            case MODEL_IS_IVW9011:
            case MODEL_IS_IVW9110:
            case MODEL_IS_IVW9030:
            case MODEL_IS_IVE70:
                modelData.device_group = 0;
                modelData.memory_size = 0;
                break;

            case MODEL_IS_DEBUG_0:
                modelData.device_group = 0;
                modelData.memory_size = 128;
                break;
            case MODEL_IS_DEBUG_1:
                modelData.device_group = 1;
                modelData.memory_size = 128;
                break;

            // Group 2
            case MODEL_AS_DEBUG:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                break;

            case MODEL_AS_PAKV2:
            case MODEL_AS_PAKV3:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_DURATION;
                modelData.capabilities |= CAP_SETTING_DOOR_READINESS_DURATION;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_DURATION;

                modelData.capabilities |= CAP_SETTING_ADDRESS;
                modelData.capabilities |= CAP_SETTING_ADDRESS_LOCK;

                modelData.capabilities |= CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL;
                
                modelData.capabilities |= CAP_SETTING_BUTTON_ROWS;
                modelData.capabilities |= CAP_UPDATE_DOORBELL_BUTTON;
                break;

            case MODEL_AS_PUK:
            case MODEL_AS_PUK_DSP:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_DURATION;
                modelData.capabilities |= CAP_SETTING_DOOR_READINESS_DURATION;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_DURATION;

                modelData.capabilities |= CAP_SETTING_ADDRESS;
                modelData.capabilities |= CAP_SETTING_ADDRESS_LOCK;

                modelData.capabilities |= CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL;
                
                modelData.capabilities |= CAP_SETTING_BUTTON_ROWS;
                modelData.capabilities |= CAP_UPDATE_DOORBELL_BUTTON;
                break;

            case MODEL_AS_PDS0X:
            case MODEL_AS_PDS0X04:
            case MODEL_AS_PES:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_DURATION;
                modelData.capabilities |= CAP_SETTING_DOOR_READINESS_DURATION;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_DURATION;

                modelData.capabilities |= CAP_SETTING_ADDRESS;
                modelData.capabilities |= CAP_SETTING_ADDRESS_LOCK;

                modelData.capabilities |= CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL;
                
                modelData.capabilities |= CAP_SETTING_BUTTON_ROWS;
                modelData.capabilities |= CAP_SETTING_HAS_CODE_LOCK;
                modelData.capabilities |= CAP_UPDATE_DOORBELL_BUTTON;
                break;

            case MODEL_AS_TCU2:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_DURATION;
                modelData.capabilities |= CAP_SETTING_DOOR_READINESS_DURATION;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_DURATION;

                modelData.capabilities |= CAP_SETTING_ADDRESS;
                modelData.capabilities |= CAP_SETTING_ADDRESS_LOCK;

                modelData.capabilities |= CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL;
                
                modelData.capabilities |= CAP_UPDATE_DOORBELL_BUTTON;
                break;

            case MODEL_AS_TCU3:
            case MODEL_AS_TCU3_EX1:
            case MODEL_AS_TCU3_EX2:
            case MODEL_AS_TCU3_EX3:
            case MODEL_AS_TCU3_EX4:
            case MODEL_AS_TCU3_EX5:
            case MODEL_AS_TCU3_EX6:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                modelData.memory_size_side = 192;
                modelData.sides = model_to_identifier(model) & 0xF;

                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_DURATION;
                modelData.capabilities |= CAP_SETTING_DOOR_READINESS_DURATION;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_DURATION;

                modelData.capabilities |= CAP_SETTING_ADDRESS;
                modelData.capabilities |= CAP_SETTING_ADDRESS_LOCK;

                modelData.capabilities |= CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL;
                
                modelData.capabilities |= CAP_UPDATE_DOORBELL_BUTTON;
                break;

            case MODEL_AS_TCU4:
            case MODEL_AS_TCU4_EX1:
            case MODEL_AS_TCU4_EX2:
            case MODEL_AS_TCU4_EX3:
            case MODEL_AS_TCU4_EX4:
            case MODEL_AS_TCU4_EX5:
            case MODEL_AS_TCU4_EX6:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                modelData.memory_size_side = 192;
                modelData.sides = model_to_identifier(model) & 0xF;

                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_DURATION;
                modelData.capabilities |= CAP_SETTING_DOOR_READINESS_DURATION;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_DURATION;

                modelData.capabilities |= CAP_SETTING_ADDRESS;
                modelData.capabilities |= CAP_SETTING_ADDRESS_LOCK;

                modelData.capabilities |= CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL;

                modelData.capabilities |= CAP_UPDATE_DOORBELL_BUTTON;
                break;

            case MODEL_AS_TKM_AS:
            case MODEL_AS_TKM_ASV:
                modelData.device_group = 2;
                modelData.memory_size = 128;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_DURATION;
                modelData.capabilities |= CAP_SETTING_DOOR_READINESS_DURATION;
                modelData.capabilities |= CAP_SETTING_CALL_TIME_DURATION;

                modelData.capabilities |= CAP_SETTING_ADDRESS;
                modelData.capabilities |= CAP_SETTING_ADDRESS_LOCK;

                modelData.capabilities |= CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS;
                modelData.capabilities |= CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL;
                
                modelData.capabilities |= CAP_UPDATE_DOORBELL_BUTTON;
                break;

            // Group 3
            // Not implemented

            // Group 4
            case MODEL_CTRL_BVS20:
            case MODEL_CTRL_BVS30:
            case MODEL_CTRL_NBV3210:
            case MODEL_CTRL_VBVS30:
            case MODEL_CTRL_NBV2600:
            case MODEL_CTRL_VBVS05:
                modelData.device_group = 4;
                modelData.memory_size = 0;
                break;
            case MODEL_CTRL_DEBUG:
                modelData.device_group = 4;
                modelData.memory_size = 128;
                break;

            // Group 5
            // Not implemented

            // Group 6
            case MODEL_EXT_TRE2:
            case MODEL_EXT_TOER2_EB:
            case MODEL_EXT_FFL1000:
            case MODEL_EXT_FAA1200:
                modelData.device_group = 6;
                modelData.memory_size = 8;
                break;

            case MODEL_EXT_BRE2_SG:
            case MODEL_EXT_BRE2_EB:
                modelData.device_group = 6;
                modelData.memory_size = 64;
                break;

            case MODEL_EXT_DEBUG:
                modelData.device_group = 6;
                modelData.memory_size = 128;
                break;

            default:
                break;
        }

        return modelData;
    }

    SettingCellData getSettingCellData(SettingType setting, Model model)
    {
        return getSettingCellData(setting, getModelData(model));
    }

    SettingCellData getSettingCellData(SettingType setting, const ModelData& model_data)
    {
        SettingCellData data{};

        if(model_data.device_group == 0 || model_data.device_group == 1)
        {
            // IS
            if (setting == SETTING_RINGTONE_ENTRANCE_DOOR_CALL && (model_data.capabilities & CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL))
            {
                if(model_data.capabilities & CAP_ALT_RINGTONE_ADDRESS)
                {
                    data.index = 24;
                    data.start_bit = 7;
                    data.length = 8;
                }
                else
                {
                    data.index = 3;
                    data.start_bit = 7;
                    data.length = 4;
                }

                data.valid = true;
            }
            else if (setting == SETTING_RINGTONE_INTERNAL_CALL && (model_data.capabilities & CAP_SETTING_RINGTONE_INTERNAL_CALL))
            {
                if(model_data.capabilities & CAP_ALT_RINGTONE_ADDRESS)
                {
                    data.index = 25;
                    data.start_bit = 7;
                    data.length = 8;
                }
                else
                {
                    data.index = 6;
                    data.start_bit = 7;
                    data.length = 4;
                }

                data.valid = true;
            }
            else if (setting == SETTING_RINGTONE_FLOOR_CALL && (model_data.capabilities & CAP_SETTING_RINGTONE_FLOOR_CALL))
            {
                if(model_data.capabilities & CAP_ALT_RINGTONE_ADDRESS)
                {
                    data.index = 26;
                    data.start_bit = 7;
                    data.length = 8;
                }
                else
                {
                    data.index = 9;
                    data.start_bit = 7;
                    data.length = 4;
                }

                data.valid = true;
            }
            else if (setting == SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL && (model_data.capabilities & CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL))
            {
                if(model_data.capabilities & CAP_ALT_RINGTONE_ADDRESS)
                {
                    data.index = 27;
                    data.start_bit = 7;
                    data.length = 8;
                }
                else
                {
                    data.index = 12;
                    data.start_bit = 7;
                    data.length = 4;
                }

                data.valid = true;
            }
            else if (setting == SETTING_VOLUME_RINGTONE && (model_data.capabilities & CAP_SETTING_VOLUME_RINGTONE))
            {
                if(model_data.model == MODEL_IS_TC40 || model_data.model == MODEL_IS_VTC40 || model_data.model == MODEL_IS_TC40V2 || model_data.model == MODEL_IS_VTC42V2)
                {
                    data.index = 28;
                    data.start_bit = 7;
                    data.length = 8;
                }
                else if(model_data.model == MODEL_IS_ISW42X0)
                {
                    data.index = 20;
                    data.start_bit = 1;
                    data.length = 2;
                }
                else
                {
                    data.index = 20;
                    data.start_bit = 7;
                    data.length = 8;
                }
                
                data.valid = true;
            }
            else if (setting == SETTING_VOLUME_HANDSET_DOOR_CALL && (model_data.capabilities & CAP_SETTING_VOLUME_HANDSET_DOOR_CALL))
            {
                data.index = 21;
                if(model_data.model == MODEL_IS_TC40V2 || model_data.model == MODEL_IS_VTC42V2)
                {
                    data.start_bit = 7;
                    data.length = 8;
                }
                else
                {
                    data.start_bit = 3;
                    data.length = 4;
                }
                data.valid = true;
            }
            else if (setting == SETTING_VOLUME_HANDSET_INTERNAL_CALL && (model_data.capabilities & CAP_SETTING_VOLUME_HANDSET_INTERNAL_CALL))
            {
                data.index = 21;
                data.start_bit = 7;
                data.length = 4;
                data.valid = true;
            }
            else if (setting == SETTING_AS_ADDRESS_DIVIDER && (model_data.capabilities & CAP_SETTING_AS_ADDRESS_DIVIDER))
            {
                if(model_data.model == MODEL_IS_TC40 || model_data.model == MODEL_IS_VTC40 || model_data.model == MODEL_IS_TC40V2 || model_data.model == MODEL_IS_VTC42V2)
                {
                    data.index = 16;
                    data.start_bit = 5;
                    data.length = 6;
                }
                else
                {
                    data.index = 22;
                    data.start_bit = 7;
                    data.length = 8;
                }
                data.valid = true;
            }
            else if (setting == SETTING_VAS_ADDRESS_DIVIDER && (model_data.capabilities & CAP_SETTING_VAS_ADDRESS_DIVIDER))
            {
                if(model_data.model == MODEL_IS_TC40 || model_data.model == MODEL_IS_VTC40 || model_data.model == MODEL_IS_TC40V2 || model_data.model == MODEL_IS_VTC42V2)
                {
                    data.index = 32;
                    data.start_bit = 5;
                    data.length = 6;
                }
                else
                {
                    data.index = 2;
                    data.start_bit = 7;
                    data.length = 8;
                }
                data.valid = true;
            }
            else if (setting == SETTING_USE_LONG_DOOR_OPENER_PROTOCOL && (model_data.capabilities & CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL))
            {
                if(model_data.model == MODEL_IS_TC40 || model_data.model == MODEL_IS_VTC40 || model_data.model == MODEL_IS_TC40V2 || model_data.model == MODEL_IS_VTC42V2)
                {
                    data.index = 15;
                    data.start_bit = 1;
                }
                else
                {
                    data.index = 23;
                    data.start_bit = 4;
                }
                data.valid = true;
            }
            else if (setting == SETTING_AMBIENT_LIGHT_IN_STANDBY && (model_data.capabilities & CAP_SETTING_AMBIENT_LIGHT_IN_STANDBY))
            {
                if(model_data.model == MODEL_IS_TKM_IS || model_data.model == MODEL_IS_TKM_ISV)
                {
                    data.index = 15;
                }
                else
                {
                    data.index = 23;
                }
                data.start_bit = 1;
                data.valid = true;
            }
            else if (setting == SETTING_RINGTONE_MUTE && (model_data.capabilities & CAP_SETTING_RINGTONE_MUTE))
            {
                if (model_data.model == MODEL_IS_TC40    || model_data.model == MODEL_IS_VTC40  || model_data.model == MODEL_IS_TC40V2 ||
                    model_data.model == MODEL_IS_VTC42V2 || model_data.model == MODEL_IS_TKM_IS || model_data.model == MODEL_IS_TKM_ISV)
                {
                    data.start_bit = 1;
                }
                else
                {
                    data.start_bit = 0;
                }
                data.index = 12;
                data.valid = true;
            }
            else if (setting == SETTING_AUTO_ANSWER_CALL && (model_data.capabilities & CAP_SETTING_AUTO_ANSWER_CALL))
            {
                data.index = 15;
                data.start_bit = 4;
                data.valid = true;
            }
            else if (setting == SETTING_CALL_TIME_UNLIMITED && (model_data.capabilities & CAP_SETTING_CALL_TIME_UNLIMITED))
            {
                data.index = 23;
                data.start_bit = 0;
                data.valid = true;
            }
            else if (setting == SETTING_PARALLEL_SERIAL_NUMBER && (model_data.capabilities & CAP_SETTING_PARALLEL_SERIAL_NUMBER))
            {
                data.index = 9;
                data.start_bit = 3;
                data.length = 20;
                data.valid = true;
            }
        }
        else if(model_data.device_group == 2)
        {
            // AS
            if (setting == SETTING_ADDRESS && (model_data.capabilities & CAP_SETTING_ADDRESS))
            {
                data.index = 0;
                data.start_bit = 6;
                data.length = 7;
                data.valid = true;
            }
            else if (setting == SETTING_ADDRESS_LOCK && (model_data.capabilities & CAP_SETTING_ADDRESS_LOCK))
            {
                data.index = 0;
                data.start_bit = 7;
                data.valid = true;
            }
            else if (setting == SETTING_CALLING_REQUIRES_DOOR_READINESS && (model_data.capabilities & CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS))
            {
                data.index = 4;
                data.start_bit = 5;
                data.valid = true;
            }
            else if (setting == SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS && (model_data.capabilities & CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS))
            {
                data.index = 4;
                data.start_bit = 4;
                data.valid = true;
            }
            else if (setting == SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL && (model_data.capabilities & CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL))
            {
                data.index = 4;
                data.start_bit = 3;
                data.valid = true;
            }
            else if (setting == SETTING_DOOR_OPENER_DURATION && (model_data.capabilities & CAP_SETTING_DOOR_OPENER_DURATION))
            {
                data.index = 1;
                data.start_bit = 7;
                data.length = 4;
                data.valid = true;
            }
            else if (setting == SETTING_CALL_TIME_DURATION && (model_data.capabilities & CAP_SETTING_CALL_TIME_DURATION))
            {
                data.index = 2;
                data.start_bit = 3;
                data.length = 4;
                data.valid = true;
            }
            else if (setting == SETTING_DOOR_READINESS_DURATION && (model_data.capabilities & CAP_SETTING_DOOR_READINESS_DURATION))
            {
                data.index = 2;
                data.start_bit = 7;
                data.length = 4;
                data.valid = true;
            }
            else if (setting == SETTING_BUTTON_ROWS && (model_data.capabilities & CAP_SETTING_BUTTON_ROWS))
            {
                data.index = 124;
                data.start_bit = 7;
                data.length = 8;
                data.valid = true;
            }
            else if (setting == SETTING_HAS_CODE_LOCK && (model_data.capabilities & CAP_SETTING_HAS_CODE_LOCK))
            {
                data.index = 125;
                data.start_bit = 7;
                data.length = 8;
                data.valid = true;
            }
        }

        return data;
    }

    uint8_t ringtone_to_int(const char* str)
    {
        for (uint8_t i = 0; i < sizeof(ringtones) / sizeof(ringtones[0]); ++i)
        {
            if (strcmp(str, ringtones[i]) == 0)
            {
                return i;
            }
        }
        return 0;
    }

    uint32_t translate_setting_to_memory(SettingType type, Model model, uint32_t value)
    {
        switch(type)
        {
            case SETTING_AS_ADDRESS_DIVIDER:
            case SETTING_VAS_ADDRESS_DIVIDER:
                if(value > 63)
                {
                    value = 63;
                }
                break;

            case SETTING_VOLUME_RINGTONE:
                if(model == MODEL_IS_TC40V2 || model == MODEL_IS_VTC42V2)
                {
                    // 0 -> 2, 1 -> 4, 2 -> 6, 3 -> 8, >3 -> 2
                    value = (value >= 0 && value <= 3) ? value * 2 + 2 : 2;
                }
                else if(model == MODEL_IS_ISW42X0)
                {
                    // 0 -> 0, 1 -> 1, 2 -> 2, 3 -> 3, >3 -> 0
                    value = (value >= 0 && value <= 3) ? value : 0;
                }
                break;

            case SETTING_VOLUME_HANDSET_DOOR_CALL:
                if(model == MODEL_IS_TC40 || model == MODEL_IS_VTC40 || model == MODEL_IS_TC40V2 || model == MODEL_IS_VTC42V2)
                {
                    // 0  -> 32, 1  -> 33, 2  -> 34, 3  -> 35, >3 -> 32
                    value = (value >= 0 && value <= 3) ? value + 32 : 32;
                }
                break;

            case SETTING_AMBIENT_LIGHT_IN_STANDBY:
                if(model == MODEL_IS_TKM_IS || model == MODEL_IS_TKM_ISV)
                {
                    value = value;
                }
                else
                {
                    // Invert the value
                    value = value ? 0 : 1;
                }
                break;

            default:
                break;
        }
        return value;
    }

    uint32_t translate_memory_to_setting(SettingType type, Model model, uint32_t value)
    {
        switch(type)
        {
            case SETTING_AS_ADDRESS_DIVIDER:
            case SETTING_VAS_ADDRESS_DIVIDER:
                if(value > 63)
                {
                    value = 63;
                }
                break;

            case SETTING_VOLUME_RINGTONE:
                if(model == MODEL_IS_TC40V2 || model == MODEL_IS_VTC42V2)
                {
                    // 2 -> 32, 4 -> 33, 6 -> 34, 8 -> 35, >8 -> 32, !%2 -> 32
                    value = (value >= 2 && value <= 8 && value % 2 == 0) ? value / 2 - 1 : 0;
                }
                else if(model == MODEL_IS_ISW42X0)
                {
                    // 0 -> 0, 1 -> 1, 2 -> 2, 3 -> 3, >3 -> 0
                    value = (value >= 0 && value <= 3) ? value : 0;
                }
                break;

            case SETTING_VOLUME_HANDSET_DOOR_CALL:
                if(model == MODEL_IS_TC40 || model == MODEL_IS_VTC40 || model == MODEL_IS_TC40V2 || model == MODEL_IS_VTC42V2)
                {
                    // <32 -> 0, 32  -> 0, 33 -> 1, 34 -> 2, 35 -> 3, >35 -> 0
                    value = (value >= 32 && value <= 35) ? value - 32 : 0;
                }
                break;

            case SETTING_AMBIENT_LIGHT_IN_STANDBY:
                if(model == MODEL_IS_TKM_IS || model == MODEL_IS_TKM_ISV)
                {
                    value = value;
                }
                else
                {
                    // Invert the value
                    value = value ? 0 : 1; 
                }
                break;

            default:
                break;
        }
        return value;
    }
}