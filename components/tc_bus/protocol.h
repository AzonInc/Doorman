#pragma once

#include "esphome/core/hal.h"

namespace esphome
{
    namespace tc_bus
    {
        enum TelegramType {
            TELEGRAM_TYPE_UNKNOWN,
            TELEGRAM_TYPE_ACK,
            TELEGRAM_TYPE_DATA,
            TELEGRAM_TYPE_SEARCH_DOORMAN_DEVICES,
            TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE,
            TELEGRAM_TYPE_DOOR_CALL,
            TELEGRAM_TYPE_FLOOR_CALL,
            TELEGRAM_TYPE_INTERNAL_CALL,
            TELEGRAM_TYPE_CONTROL_FUNCTION,
            TELEGRAM_TYPE_START_TALKING_DOOR_CALL,
            TELEGRAM_TYPE_START_TALKING,
            TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL,
            TELEGRAM_TYPE_STOP_TALKING,
            TELEGRAM_TYPE_OPEN_DOOR,
            TELEGRAM_TYPE_OPEN_DOOR_LONG,
            TELEGRAM_TYPE_LIGHT,
            TELEGRAM_TYPE_DOOR_OPENED,
            TELEGRAM_TYPE_DOOR_CLOSED,
            TELEGRAM_TYPE_END_OF_RINGTONE,
            TELEGRAM_TYPE_END_OF_DOOR_READINESS,
            TELEGRAM_TYPE_INITIALIZE_DOOR_STATION,
            TELEGRAM_TYPE_RESET,
            TELEGRAM_TYPE_SELECT_DEVICE_GROUP,
            TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET,
            TELEGRAM_TYPE_SEARCH_DEVICES,
            TELEGRAM_TYPE_FOUND_DEVICE,
            TELEGRAM_TYPE_FOUND_DEVICE_SUBSYSTEM,
            TELEGRAM_TYPE_PROGRAMMING_MODE,
            TELEGRAM_TYPE_READ_MEMORY_BLOCK,
            TELEGRAM_TYPE_SELECT_MEMORY_PAGE,
            TELEGRAM_TYPE_WRITE_MEMORY,
            TELEGRAM_TYPE_REQUEST_VERSION
        };
        
        struct TelegramData {
            uint32_t raw;
            std::string hex;

            TelegramType type;
            uint8_t address;
            uint32_t serial_number;
            uint32_t payload;
            
            bool is_long;
            bool is_response;
            bool is_data;
        };

        TelegramData buildTelegram(TelegramType type, uint8_t address = 0, uint32_t payload = 0, uint32_t serial_number = 0);
        TelegramData parseTelegram(uint32_t telegram, bool is_long = true, bool is_response = false, bool is_data = false);

        const char* telegram_type_to_string(TelegramType type);
        TelegramType string_to_telegram_type(std::string str);

    }  // namespace tc_bus
}  // namespace esphome