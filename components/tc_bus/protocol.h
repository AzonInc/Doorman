#pragma once

#include "esphome/core/hal.h"

namespace esphome
{
    namespace tc_bus
    {
        enum Model {
            MODEL_NONE,
            MODEL_TCS_ISH1030,
            MODEL_TCS_ISH3030,
            MODEL_TCS_ISH3230,
            MODEL_TCS_ISH3340,
            MODEL_TCS_ISW3030,
            MODEL_TCS_ISW3230,
            MODEL_TCS_ISW3340,
            MODEL_TCS_IVH3222,
            MODEL_KOCH_TC50,
            MODEL_KOCH_TCH50,
            MODEL_KOCH_TCH50P
        };

        enum SettingType {
            SETTING_UNKNOWN,
            SETTING_RINGTONE_FLOOR_CALL,
            SETTING_RINGTONE_DOOR_CALL,
            SETTING_RINGTONE_INTERNAL_CALL,
            SETTING_VOLUME_RINGTONE,
            SETTING_VOLUME_HANDSET
        };

        enum CommandType {
            COMMAND_TYPE_UNKNOWN,
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
            uint8_t length;
        };

        struct SettingCellData {
            uint8_t index = 0;
            bool left_nibble = false;
        };


        uint32_t buildCommand(CommandType type, uint8_t address = 0, uint32_t payload = 0, uint32_t serial_number = 0);
        CommandData parseCommand(uint32_t command);

        const char* command_type_to_string(CommandType type);
        CommandType string_to_command_type(std::string str);

        const char* setting_type_to_string(SettingType type);
        SettingType string_to_setting_type(std::string str);

        const char* model_to_string(Model model);
        Model string_to_model(std::string str);

    }  // namespace tc_bus
}  // namespace esphome