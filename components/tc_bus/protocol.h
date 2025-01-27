#pragma once

#include "esphome/core/hal.h"

namespace esphome
{
    namespace tc_bus
    {
        enum Model {
            MODEL_NONE,
            MODEL_ISW3030,
            MODEL_ISW3130,
            MODEL_ISW3230,
            MODEL_ISW3330,
            MODEL_ISW3340,
            MODEL_ISW5010,
            MODEL_ISW5020,
            MODEL_ISW5030,
            MODEL_ISW5031,
            MODEL_ISW5033,
            MODEL_IVW511X,
            MODEL_IVW521X,
            MODEL_ISW6031,
            MODEL_ISW7030,
            MODEL_IVW7510,
            MODEL_ISH7030,
            MODEL_IVH7510,
            MODEL_ISW6010,
            MODEL_IVW6511,
            MODEL_ISWM7000,
            MODEL_IVWM7000,
            MODEL_ISW4100,
            MODEL_IMM2100,
            MODEL_IVW2210,
            MODEL_IVW2211,
            MODEL_IVW2212,
            MODEL_VTC42V2,
            MODEL_TC40V2,
            MODEL_VTC40,
            MODEL_TC40,
            MODEL_TC2000,
            MODEL_TC20P,
            MODEL_TC20F,
            MODEL_ISH3340,
            MODEL_ISH3022,
            MODEL_ISH3130,
            MODEL_ISW3022,
            MODEL_ISH3230,
            MODEL_ISH3030,
            MODEL_ISH1030,
            MODEL_IMM1000,
            MODEL_IMM1100,
            MODEL_IMM1300,
            MODEL_IMM1500,
            MODEL_IMM1310,
            MODEL_IMM1110,
            MODEL_IVH3222,
            MODEL_IVH4222,
            MODEL_IVW2220,
            MODEL_IVW2221,
            MODEL_IVW3011,
            MODEL_IVW3012,
            MODEL_VMH,
            MODEL_VML,
            MODEL_VMF,
            MODEL_TKIS,
            MODEL_TKISV,
            MODEL_CAIXXXX,
            MODEL_CAI2000,
            MODEL_ISW42X0
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

        enum CommandType {
            COMMAND_TYPE_UNKNOWN,
            COMMAND_TYPE_ACK,
            COMMAND_TYPE_SEARCH_DOORMAN_DEVICES,
            COMMAND_TYPE_FOUND_DOORMAN_DEVICE,
            COMMAND_TYPE_DOOR_CALL,
            COMMAND_TYPE_FLOOR_CALL,
            COMMAND_TYPE_INTERNAL_CALL,
            COMMAND_TYPE_CONTROL_FUNCTION,
            COMMAND_TYPE_START_TALKING_DOOR_CALL,
            COMMAND_TYPE_START_TALKING,
            COMMAND_TYPE_STOP_TALKING_DOOR_CALL,
            COMMAND_TYPE_STOP_TALKING,
            COMMAND_TYPE_OPEN_DOOR,
            COMMAND_TYPE_OPEN_DOOR_LONG,
            COMMAND_TYPE_LIGHT,
            COMMAND_TYPE_DOOR_OPENED,
            COMMAND_TYPE_DOOR_CLOSED,
            COMMAND_TYPE_END_OF_RINGTONE,
            COMMAND_TYPE_END_OF_DOOR_READINESS,
            COMMAND_TYPE_INITIALIZE_DOOR_STATION,
            COMMAND_TYPE_RESET,
            COMMAND_TYPE_SELECT_DEVICE_GROUP,
            COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET,
            COMMAND_TYPE_SEARCH_DEVICES,
            COMMAND_TYPE_FOUND_DEVICE,
            COMMAND_TYPE_FOUND_DEVICE_SUBSYSTEM,
            COMMAND_TYPE_PROGRAMMING_MODE,
            COMMAND_TYPE_READ_MEMORY_BLOCK,
            COMMAND_TYPE_SELECT_MEMORY_PAGE,
            COMMAND_TYPE_WRITE_MEMORY,
            COMMAND_TYPE_REQUEST_VERSION
        };
        
        struct CommandData {
            uint32_t command;
            std::string command_hex;
            CommandType type;
            uint8_t address;
            uint32_t serial_number;
            uint32_t payload;
            bool is_long;
        };

        struct SettingCellData {
            uint8_t index = 0;
            bool left_nibble = false;
        };

        struct ModelData {
            Model model = MODEL_NONE;
            uint32_t firmware_version = 0;
            uint8_t firmware_major = 0;
            uint8_t firmware_minor = 0;
            uint8_t firmware_patch = 0;
            uint8_t hardware_version = 0; 
            uint8_t category = 0;
            uint8_t memory_size = 0;
        };

        CommandData buildCommand(CommandType type, uint8_t address = 0, uint32_t payload = 0, uint32_t serial_number = 0);
        CommandData parseCommand(uint32_t command, bool is_long = true);

        const char* command_type_to_string(CommandType type);
        CommandType string_to_command_type(std::string str);

        const char* setting_type_to_string(SettingType type);
        SettingType string_to_setting_type(std::string str);

        SettingCellData getSettingCellData(SettingType setting, Model model);
        ModelData getModelData(Model model = MODEL_NONE);

        const char* model_to_string(Model model = MODEL_NONE);
        Model string_to_model(const std::string& str);
        Model identifier_string_to_model(const std::string& model_key, const uint8_t& hw_version = 0, const uint32_t& fw_version = 0);

        uint8_t ringtone_to_int(const std::string& str);
        std::string int_to_ringtone(uint8_t ringtone);

    }  // namespace tc_bus
}  // namespace esphome