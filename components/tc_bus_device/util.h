#pragma once

#include "esphome/core/hal.h"

namespace esphome::tc_bus
{
    enum DeviceGroup {
        DEVICE_GROUP_INDOOR_STATION_CLASSIC = 0,
        DEVICE_GROUP_INDOOR_STATION_HANDSFREE = 1,
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
        MODEL_IS_TASTA_AUDIO,
        MODEL_IS_TASTA_VIDEO,
        MODEL_IS_TASTA_PRO_AUDIO,
        MODEL_IS_TASTA_PRO_VIDEO,
        MODEL_IS_SENSO_PRO_AUDIO,
        MODEL_IS_SENSO_PRO_VIDEO,
        MODEL_IS_ECOOS,
        MODEL_IS_ISW4100,
        MODEL_IS_IMM2100,
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
        MODEL_IS_TKM_IS,
        MODEL_IS_TKM_ISV,
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
        MODEL_AS_PUK,
        MODEL_AS_PUK_DSP,
        MODEL_AS_PAKV2,
        MODEL_AS_PAKV3,
        MODEL_AS_PDS0X04,
        MODEL_AS_PDS0X,
        MODEL_AS_PES,
        MODEL_AS_TCU2,
        MODEL_AS_TCU3,
        MODEL_AS_TCU3_EX1,
        MODEL_AS_TCU3_EX2,
        MODEL_AS_TCU3_EX3,
        MODEL_AS_TCU3_EX4,
        MODEL_AS_TCU3_EX5,
        MODEL_AS_TCU3_EX6,
        MODEL_AS_TCU4,
        MODEL_AS_TCU4_EX1,
        MODEL_AS_TCU4_EX2,
        MODEL_AS_TCU4_EX3,
        MODEL_AS_TCU4_EX4,
        MODEL_AS_TCU4_EX5,
        MODEL_AS_TCU4_EX6,
        MODEL_AS_TKM_AS,
        MODEL_AS_TKM_ASV,
        MODEL_AS_AMI1010X,

        MODEL_CTRL_BVS20,
        MODEL_CTRL_BVS30,
        MODEL_CTRL_NBV3210,
        MODEL_CTRL_VBVS30,
        MODEL_CTRL_NBV2600,
        MODEL_CTRL_VBVS05,
        MODEL_CTRL_DEBUG,

        MODEL_EXT_BRE2_SG,
        MODEL_EXT_BRE2_EB,
        MODEL_EXT_TRE2,
        MODEL_EXT_TOER2_EB,
        MODEL_EXT_FFL1000,
        MODEL_EXT_FAA1200,
        MODEL_EXT_DEBUG
    };

    struct ModelMapping {
        Model model;
        const char* name;
    };

    struct ModelEntry {
        Model    model;
        uint16_t model_key;
        uint16_t fw_min;
        uint16_t fw_max;
        uint8_t  device_group;
    };

    enum SettingType {
        SETTING_UNKNOWN,
        SETTING_RINGTONE_FLOOR_CALL,
        SETTING_RINGTONE_ENTRANCE_DOOR_CALL,
        SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL,
        SETTING_RINGTONE_INTERNAL_CALL,
        SETTING_VOLUME_RINGTONE,
        SETTING_VOLUME_HANDSET_DOOR_CALL,
        SETTING_VOLUME_HANDSET_INTERNAL_CALL,
        SETTING_AS_ADDRESS_DIVIDER,
        SETTING_VAS_ADDRESS_DIVIDER,
        SETTING_USE_LONG_DOOR_OPENER_PROTOCOL,
        SETTING_AMBIENT_LIGHT_IN_STANDBY,
        SETTING_RINGTONE_MUTE,
        SETTING_DOOR_OPENER_DURATION,
        SETTING_ADDRESS,
        SETTING_ADDRESS_LOCK,
        SETTING_CALLING_REQUIRES_DOOR_READINESS,
        SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS,
        SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL,
        SETTING_DOOR_READINESS_DURATION,
        SETTING_CALL_TIME_DURATION,
        SETTING_BUTTON_ROWS,
        SETTING_HAS_CODE_LOCK,
        SETTING_AUTO_ANSWER_CALL,
        SETTING_CALL_TIME_UNLIMITED,
        SETTING_PARALLEL_SERIAL_NUMBER,
    };

    struct SettingMapping {
        SettingType type;
        const char* name;
    };

    struct SettingCellData {
        bool valid = false;
        uint8_t index = 0;
        uint8_t start_bit = 0;
        uint8_t length = 1;
        uint8_t page = 0;
    };

    enum DoorbellButtonAction {
        DOORBELL_BUTTON_ACTION_NONE = 0xF,
        DOORBELL_BUTTON_ACTION_DOOR_CALL = 0x0,
        DOORBELL_BUTTON_ACTION_LIGHT = 0x1,
        DOORBELL_BUTTON_ACTION_CONTROL_FUNCTION = 0x2,
    };

    struct DoorbellButtonActionMapping {
        DoorbellButtonAction action;
        const char* name;
    };

    struct DoorbellButtonConfig {
        static constexpr uint32_t DOORBELL_BUTTON_UNASSIGNED = 0xFFFFF;

        uint32_t primary_payload = DOORBELL_BUTTON_UNASSIGNED;
        uint32_t secondary_payload = DOORBELL_BUTTON_UNASSIGNED;
        
        DoorbellButtonAction primary_action = DOORBELL_BUTTON_ACTION_NONE;
        DoorbellButtonAction secondary_action = DOORBELL_BUTTON_ACTION_NONE;
        
    };

    enum ModelCapabilities {
        CAP_SETTING_RINGTONE_ENTRANCE_DOOR_CALL = (1 << 0),
        CAP_SETTING_RINGTONE_INTERNAL_CALL = (1 << 1),
        CAP_SETTING_RINGTONE_FLOOR_CALL = (1 << 2),
        CAP_SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL = (1 << 3),
        CAP_SETTING_VOLUME_RINGTONE = (1 << 4),
        CAP_SETTING_VOLUME_HANDSET_DOOR_CALL = (1 << 5),
        CAP_SETTING_VOLUME_HANDSET_INTERNAL_CALL = (1 << 6),
        CAP_SETTING_AS_ADDRESS_DIVIDER = (1 << 7),
        CAP_SETTING_VAS_ADDRESS_DIVIDER = (1 << 8),
        CAP_SETTING_USE_LONG_DOOR_OPENER_PROTOCOL = (1 << 9),
        CAP_SETTING_AMBIENT_LIGHT_IN_STANDBY = (1 << 10),
        CAP_SETTING_RINGTONE_MUTE = (1 << 11),
        CAP_SETTING_DOOR_OPENER_DURATION = (1 << 12),
        CAP_SETTING_DOOR_READINESS_DURATION = (1 << 13),
        CAP_SETTING_CALL_TIME_DURATION = (1 << 14),
        CAP_SETTING_ADDRESS = (1 << 15),
        CAP_SETTING_ADDRESS_LOCK = (1 << 16),
        CAP_SETTING_CALLING_REQUIRES_DOOR_READINESS = (1 << 17),
        CAP_SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS = (1 << 18),
        CAP_SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL = (1 << 19),
        CAP_SETTING_BUTTON_ROWS = (1 << 20),
        CAP_SETTING_HAS_CODE_LOCK = (1 << 21),
        CAP_UPDATE_DOORBELL_BUTTON = (1 << 22),
        CAP_ALT_RINGTONE_ADDRESS = (1 << 23),
        CAP_INDIVIDUAL_RESET = (1 << 24),
        CAP_SETTING_AUTO_ANSWER_CALL = (1 << 25),
        CAP_SETTING_CALL_TIME_UNLIMITED = (1 << 26),
        CAP_SETTING_PARALLEL_SERIAL_NUMBER = (1 << 27),
    };

    struct ModelData {
        Model model = MODEL_NONE;
        uint32_t firmware_version = 0;
        uint32_t capabilities = 0;
        uint8_t firmware_major = 0;
        uint8_t firmware_minor = 0;
        uint8_t firmware_patch = 0;
        uint8_t hardware_version = 0; 
        uint8_t device_group = 0;
        uint8_t memory_size = 0;
        uint8_t memory_size_side = 0;
        uint8_t sides = 0;
    };

    const char* setting_type_to_string(SettingType type);
    SettingType string_to_setting_type(const char* str);

    const char* doorbell_button_action_to_string(DoorbellButtonAction action);
    DoorbellButtonAction string_to_doorbell_button_action(const char* str);

    const char* device_group_to_string(uint8_t device_group);

    SettingCellData getSettingCellData(SettingType setting, Model model);
    SettingCellData getSettingCellData(SettingType setting, const ModelData& model_data);
    ModelData getModelData(Model model = MODEL_NONE);

    const char* model_to_string(Model model = MODEL_NONE);
    Model string_to_model(const char* str);

    uint16_t model_to_identifier(Model model);
    Model identifier_to_model(uint8_t device_group, uint16_t model_key, uint8_t hw_version = 0, uint16_t fw_version = 0);

    static constexpr const char* const ringtones[] = {
        "Ringtone 1",
        "Ringtone 2",
        "Ringtone 3",
        "Ringtone 4",
        "Ringtone 5",
        "Ringtone 6",
        "Ringtone 7",
        "Ringtone 8",
        "Ringtone 9",
        "Ringtone 10",
        "Ringtone 11",
        "Ringtone 12",
        "Ringtone 13"
    };

    uint8_t ringtone_to_int(const char* str);
    inline const char* int_to_ringtone(uint8_t ringtone)
    {
        return ringtones[ringtone > 12 ? 0 : ringtone];
    }

    uint32_t translate_setting_to_memory(SettingType type, Model model, uint32_t value);
    uint32_t translate_memory_to_setting(SettingType type, Model model, uint32_t value);
}