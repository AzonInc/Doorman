#pragma once

#include "esphome/core/hal.h"

namespace esphome
{
    namespace tc_bus
    {
        enum DeviceGroup {
            DEVICE_GROUP_INDOOR_STATION = 1,
            DEVICE_GROUP_OUTDOOR_STATION = 2,
            DEVICE_GROUP_CONTROLLER = 4,
            DEVICE_GROUP_EXTENSION = 6
        };

        enum Model {
            MODEL_NONE,

            MODEL_IS_ISW3030,
            MODEL_IS_ISW3130,
            MODEL_IS_ISW3230,
            MODEL_IS_ISW3330,
            MODEL_IS_ISW3340,
            MODEL_IS_ISW5010,
            MODEL_IS_ISW5020,
            MODEL_IS_ISW5030,
            MODEL_IS_ISW5031,
            MODEL_IS_ISW5033,
            MODEL_IS_IVW511X,
            MODEL_IS_IVW521X,
            MODEL_IS_ISW6031,
            MODEL_IS_ISW7030,
            MODEL_IS_IVW7510,
            MODEL_IS_ISH7030,
            MODEL_IS_IVH7510,
            MODEL_IS_ISW6010,
            MODEL_IS_IVW6511,
            MODEL_IS_ISWM7000,
            MODEL_IS_IVWM7000,
            MODEL_IS_ISW4100,
            MODEL_IS_IMM2100,
            MODEL_IS_IVW2210,
            MODEL_IS_IVW2211,
            MODEL_IS_IVW2212,
            MODEL_IS_VTC42V2,
            MODEL_IS_TC40V2,
            MODEL_IS_VTC40,
            MODEL_IS_TC40,
            MODEL_IS_TC2000,
            MODEL_IS_TC20P,
            MODEL_IS_TC20F,
            MODEL_IS_ISH3340,
            MODEL_IS_ISH3022,
            MODEL_IS_ISH3130,
            MODEL_IS_ISW3022,
            MODEL_IS_ISH3230,
            MODEL_IS_ISH3030,
            MODEL_IS_ISH1030,
            MODEL_IS_TTCXX,
            MODEL_IS_TTSXX,
            MODEL_IS_IMM1000,
            MODEL_IS_IMM1100,
            MODEL_IS_IMM1300,
            MODEL_IS_IMM1500,
            MODEL_IS_IMM1310,
            MODEL_IS_IMM1110,
            MODEL_IS_IVH3222,
            MODEL_IS_IVH4222,
            MODEL_IS_IVW2220,
            MODEL_IS_IVW2221,
            MODEL_IS_IVW3011,
            MODEL_IS_IVW3012,
            MODEL_IS_VMH,
            MODEL_IS_VML,
            MODEL_IS_VMF,
            MODEL_IS_TKIS,
            MODEL_IS_TKISV,
            MODEL_IS_CAIXXXX,
            MODEL_IS_CAI2000,
            MODEL_IS_ISW42X0,
            MODEL_IS_IVW9010,
            MODEL_IS_IVW9011,
            MODEL_IS_IVW9110,
            MODEL_IS_IVW9030,
            MODEL_IS_IVE70,
            MODEL_IS_DEBUG_0,
            MODEL_IS_DEBUG_1,

            MODEL_AS_DEBUG,

            MODEL_CTRL_BVS20,
            MODEL_CTRL_BVS30,
            MODEL_CTRL_NBV3210,
            MODEL_CTRL_VBVS30,
            MODEL_CTRL_NBV2600,
            MODEL_CTRL_DEBUG,

            MODEL_EXT_TRE2,
            MODEL_EXT_DEBUG
        };

        enum SettingType {
            SETTING_UNKNOWN,
            SETTING_RINGTONE_FLOOR_CALL,
            SETTING_RINGTONE_ENTRANCE_DOOR_CALL,
            SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL,
            SETTING_RINGTONE_INTERNAL_CALL,
            SETTING_VOLUME_RINGTONE,
            SETTING_VOLUME_HANDSET_DOOR_CALL,
            SETTING_VOLUME_HANDSET_INTERNAL_CALL
        };

        struct SettingCellData {
            uint8_t index = 0;
            bool left_nibble = false;
        };

        enum ModelCapabilities {
            CAP_RINGTONE_ENTRANCE_DOOR_CALL = (1 << 0),
            CAP_RINGTONE_INTERNAL_CALL = (1 << 1),
            CAP_RINGTONE_FLOOR_CALL = (1 << 2),
            CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL = (1 << 3),
            CAP_VOLUME_RINGTONE = (1 << 4),
            CAP_VOLUME_HANDSET_DOOR_CALL = (1 << 5),
            CAP_VOLUME_HANDSET_INTERNAL_CALL = (1 << 6)
        };

        struct ModelData {
            Model model = MODEL_NONE;
            uint32_t firmware_version = 0;
            uint8_t firmware_major = 0;
            uint8_t firmware_minor = 0;
            uint8_t firmware_patch = 0;
            uint8_t hardware_version = 0; 
            uint8_t device_group = 0;
            uint8_t memory_size = 0;
            uint32_t capabilities = 0;
        };

        const char* setting_type_to_string(SettingType type);
        SettingType string_to_setting_type(std::string str);

        SettingCellData getSettingCellData(SettingType setting, Model model);
        ModelData getModelData(Model model = MODEL_NONE);

        const char* model_to_string(Model model = MODEL_NONE);
        Model string_to_model(const std::string& str);
        Model identifier_string_to_model(const uint8_t& device_group, const std::string& model_key, const uint8_t& hw_version = 0, const uint32_t& fw_version = 0);

        uint8_t ringtone_to_int(const std::string& str);
        std::string int_to_ringtone(uint8_t ringtone);

    }  // namespace tc_bus
}  // namespace esphome