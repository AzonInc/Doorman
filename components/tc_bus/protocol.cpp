#include "protocol.h"
#include "esphome.h"
#include "esphome/core/helpers.h"
#include <algorithm>
#include <cctype>

namespace esphome::tc_bus
{
    TelegramData buildTelegram(TelegramType type, uint8_t address, uint32_t payload, uint32_t serial_number)
    {
        TelegramData data{};
        data.raw = 0;
        data.type = type;
        data.is_long = true;
        data.is_response = false;

        switch (type)
        {
            case TELEGRAM_TYPE_ACK_DATA:
                data.is_long = true;
                data.is_response = true;
                data.payload = payload;
                data.serial_number = 0;
                data.address = 0;
                data.raw = payload;
                break;

            case TELEGRAM_TYPE_ACK_STATUS:
                data.is_long = false;
                data.is_response = true;
                data.serial_number = 0;
                data.address = 0;

                if(payload < 1 || payload > 15)
                {
                    payload = 1;
                }
                data.payload = payload;
                
                data.raw |= (payload & 0xF); // 1
                break;

            case TELEGRAM_TYPE_DOOR_CALL:
            case TELEGRAM_TYPE_INTERNAL_CALL:
                data.serial_number = serial_number;
                data.address = address;

                data.raw |= (0 << 28);  // 0
                data.raw |= ((serial_number & 0xFFFFF) << 8); // C30BA

                // Call type
                if (type == TELEGRAM_TYPE_INTERNAL_CALL)
                {
                    data.raw |= (1 << 6); // 8
                }
                else
                {
                    data.raw &= ~(1 << 6); // 0
                }

                // Flags
                //data.raw |= (1 << 7);

                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_FLOOR_CALL:
                data.serial_number = serial_number;

                data.raw |= (1 << 28);  // 1
                data.raw |= ((serial_number & 0xFFFFF) << 8); // C30BA
                data.raw |= 0x41; // 41
                break;

            case TELEGRAM_TYPE_START_TALKING_DOOR_CALL:
            case TELEGRAM_TYPE_START_TALKING:
                data.serial_number = serial_number;
                data.address = address;
                data.payload = payload;

                data.raw |= (3 << 28); // 3
                data.raw |= ((serial_number & 0xFFFFF) << 8); // C30BA

                // Call type
                if (type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL) {
                    data.raw &= ~(1 << 6); // AS

                    // Flags: Door readiness
                    if(payload > 0)
                    {
                        data.raw |= (1 << 7); // door readiness active
                    }
                    else
                    {
                        data.raw &= ~(1 << 7); // door readiness inactive
                    }
                } else {
                    data.raw |= (1 << 6); // IA

                    // Flags: Talk Mode
                    if(payload > 0)
                    {
                        data.raw |= (1 << 7); // full duplex
                    }
                    else
                    {
                        data.raw &= ~(1 << 7); // half duplex
                    }
                }

                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL:
            case TELEGRAM_TYPE_STOP_TALKING:
                data.address = address;
                data.is_long = false;

                data.raw |= (3 << 12); // 3

                // Flags
                if (type == TELEGRAM_TYPE_STOP_TALKING)
                {
                    data.raw |= (1 << 6);  // IA
                }
                else
                {
                    data.raw &= ~(1 << 6); // AS
                }
                //data.payload |= (1 << 7);

                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_OPEN_DOOR:
                data.address = address;
                data.payload = payload;
                data.is_long = false;

                data.raw |= (1 << 12); // 1
                data.raw |= (1 << 8); // 1

                // Flags
                if(payload > 0)
                {
                    data.raw |= (1 << 7); // door readiness active
                }
                else
                {
                    data.raw &= ~(1 << 7); // door readiness inactive
                }
                //data.raw |= (1 << 6);

                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_OPEN_DOOR_LONG:
                if(serial_number == 0)
                {
                    // Convert to short door opener telegram
                    data.type = TELEGRAM_TYPE_OPEN_DOOR;
                    data.address = address;
                    data.payload = payload;
                    data.is_long = false;

                    data.raw |= (1 << 12); // 1
                    data.raw |= (1 << 8); // 1

                    // Flags
                    if(payload > 0)
                    {
                        data.raw |= (1 << 7); // door readiness active
                    }
                    else
                    {
                        data.raw &= ~(1 << 7); // door readiness inactive
                    }
                    //data.raw |= (1 << 6);

                    data.raw |= (address & 0x3F); // 0
                }
                else
                {
                    data.serial_number = serial_number;
                    data.address = address;
                    data.payload = payload;

                    data.raw |= (1 << 28);  // 1
                    data.raw |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    
                    data.raw |= (1 << 7);

                    // Flags
                    if(payload > 0)
                    {
                        data.raw |= (1 << 6); // door readiness active
                    }
                    else
                    {
                        data.raw &= ~(1 << 6); // door readiness inactive
                    }

                    data.raw |= (address & 0x3F); // 0
                }
                break;

            case TELEGRAM_TYPE_LIGHT:
                data.is_long = false;

                data.raw |= (1 << 12); // 1
                data.raw |= (2 << 8);  // 2
                break;

            case TELEGRAM_TYPE_CONTROL_FUNCTION:
                data.serial_number = serial_number;
                data.payload = payload;

                data.raw |= (6 << 28); // 6
                data.raw |= ((serial_number & 0xFFFFF) << 8); // C30BA
                data.raw |= (payload & 0xFF); // 08
                break;

            case TELEGRAM_TYPE_REQUEST_VERSION:
                data.serial_number = serial_number;

                data.raw |= (5 << 28); // 5
                data.raw |= ((serial_number & 0xFFFFF) << 8); // C30BA
                data.raw |= (0xC0 & 0xFF); // C0
                break;

            case TELEGRAM_TYPE_RESET:
                data.is_long = false;

                data.raw |= (5 << 12); // 5
                data.raw |= (1 << 8);  // 100
                break;

            case TELEGRAM_TYPE_SEARCH_DOORMAN_DEVICES:
                data.is_long = false;  

                data.raw = 0x7FFF;
                break;

            case TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE:
                data.payload = payload;

                data.raw |= (0x7F << 24); // 7F
                data.raw |= payload & 0xFFFFFF; // MAC address
                break;

            case TELEGRAM_TYPE_FOUND_DEVICE:
                data.serial_number = serial_number;

                data.raw |= (5 << 28); // 5
                data.raw |= ((serial_number & 0xFFFFF) << 8); // C30BA
                data.raw |= (0x10 & 0xFF); // 10
                break;

            case TELEGRAM_TYPE_SELECT_DEVICE_GROUP:
                data.payload = payload;
                data.is_long = false;

                data.raw |= (5 << 12); // 5
                data.raw |= (8 << 8);  // 80
                data.raw |= (payload & 0xFF); // 0
                break;

            case TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET:
                data.payload = payload;
                data.is_long = false;

                data.raw |= (5 << 12); // 5
                data.raw |= (9 << 8);  // 90
                data.raw |= (payload & 0xFF); // 0
                break;

            case TELEGRAM_TYPE_SEARCH_DEVICES:
                data.is_long = false;

                data.raw |= (5 << 12); // 5
                data.raw |= (2 << 8);  // 20
                break;

            case TELEGRAM_TYPE_PROGRAMMING_MODE:
                data.payload = payload;
                data.is_long = false;

                data.raw |= (5 << 12); // 5
                data.raw |= (0 << 8);  // 0
                data.raw |= (4 << 4);  // 4
                data.raw |= (payload & 0xF); // 0 / 1
                break;

            case TELEGRAM_TYPE_INITIALIZE_DOOR_STATION:
                data.address = address;
                data.is_long = false;

                data.raw |= (2 << 12); // 2
                data.raw |= (8 << 8); // 8
                data.raw |= (0 << 7); // 0
                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_END_OF_RINGTONE:
                data.address = address;
                data.is_long = false;

                data.raw |= (2 << 12); // 2
                data.raw |= (2 << 8); // 2
                data.raw |= (0 << 7); // 0
                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_END_OF_DOOR_READINESS:
                data.address = address;
                data.is_long = false;

                data.raw |= (2 << 12); // 2
                data.raw |= (4 << 8); // 4
                data.raw |= (0 << 7); // 0
                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_DOOR_CLOSED:
                data.address = address;
                data.is_long = false;

                data.raw |= (2 << 12); // 2
                data.raw |= (1 << 8); // 1
                data.raw |= (1 << 7); // 8
                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_DOOR_OPENED:
                data.address = address;
                data.is_long = false;

                data.raw |= (2 << 12); // 2
                data.raw |= (1 << 8); // 1
                data.raw &= ~(1 << 7); // 0
                data.raw |= (address & 0x3F); // 0
                break;

            case TELEGRAM_TYPE_READ_MEMORY_BLOCK:
                data.address = address;
                data.is_long = false;

                data.raw |= (8 << 12); // 8
                data.raw |= (4 << 8);  // 4
                data.raw |= (address & 0xFF); // 00
                break;

            case TELEGRAM_TYPE_WRITE_MEMORY:
                data.address = address;
                data.payload = payload;

                data.raw |= (8 << 28); // 8
                data.raw |= (2 << 24); // 2
                data.raw |= (address & 0xFF) << 16; // start address
                data.raw |= payload & 0xFFFF; // ABCD payload
                break;

            case TELEGRAM_TYPE_SELECT_MEMORY_PAGE:
                data.serial_number = serial_number;
                data.address = address;

                data.raw |= (8 << 28); // 8
                data.raw |= (1 << 24); // 1
                data.raw |= (address & 0xF) << 20; // page
                data.raw |= serial_number & 0xFFFFF;
                break;

            default:
                break;
        }

        // Generate telegram HEX
        size_t pos = 0;
        size_t len =  data.is_long ? 7 : (data.type == TELEGRAM_TYPE_ACK_STATUS ? 0 : 3);
        for (int i = len; i >= 0; --i) {
            uint8_t nibble = (data.raw >> (i * 4)) & 0xF;
            data.hex[pos++] = "0123456789ABCDEF"[nibble];
        }

        return data;
    }

    TelegramData parseTelegram(uint32_t raw, bool is_long, bool is_response, bool is_retransmission)
    {
        TelegramData data{};
        data.raw = raw;
        data.type = TELEGRAM_TYPE_UNKNOWN;
        data.address = 0;
        data.payload = 0;
        data.is_long = is_long;
        data.is_response = is_response;
        data.is_retransmission = is_retransmission;

        if (is_response)
        {
            data.type = is_long ? TELEGRAM_TYPE_ACK_DATA : TELEGRAM_TYPE_ACK_STATUS;
            data.payload = raw;
            data.serial_number = 0;
            data.address = 0;
        }
        else
        {
            if (is_long)
            {
                // Handle 32-bit telegrams

                data.serial_number = (raw >> 8) & 0xFFFFF; // Serial (from bits 8 to 23)

                switch ((raw >> 28) & 0xF)
                {
                    case 0:
                        data.type = (raw & (1 << 6)) != 0 ? TELEGRAM_TYPE_INTERNAL_CALL : TELEGRAM_TYPE_DOOR_CALL;
                        data.address = raw & 0x3F;

                        // data.payload = raw & (1 << 7);
                        break;

                    case 1:
                        if ((raw & 0xFF) == 0x41)
                        {
                            data.type = TELEGRAM_TYPE_FLOOR_CALL;
                        }
                        else if (raw & (1 << 7))
                        {
                            data.type = TELEGRAM_TYPE_OPEN_DOOR;
                            data.address = raw & 0x3F;

                            // Door readiness
                            data.payload = (raw & (1 << 6)) != 0;
                        }
                        break;

                    case 3:
                        data.type = (raw & (1 << 6)) != 0 ? TELEGRAM_TYPE_START_TALKING : TELEGRAM_TYPE_START_TALKING_DOOR_CALL;
                        data.address = raw & 0x3F;

                        // Flags
                        if(data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
                        {
                            data.payload = (raw & (1 << 7)) != 0; // Door Readiness
                        }
                        else if(data.type == TELEGRAM_TYPE_START_TALKING)
                        {
                            data.payload = (raw & (1 << 7)) != 0; // half duplex / full duplex
                        }
                        break;

                    case 5:
                        switch ((raw >> 4) & 0xF)
                        {
                            case 1:
                                data.type = TELEGRAM_TYPE_FOUND_DEVICE;
                                data.address = raw & 0xF;
                                break;
                            
                            case 4:
                                data.type = TELEGRAM_TYPE_FOUND_DEVICE_SUBSYSTEM;
                                data.address = raw & 0xF;
                                break;

                            case 8:
                                // Device test
                                // Not implemented
                                break;

                            case 0xC:
                                data.type = TELEGRAM_TYPE_REQUEST_VERSION;

                                // request type and version
                                // request versions of subdevices
                                // Not implemented
                                // 5 12345 C0
                                break;

                            default:
                                // Not implemented
                                break;
                        }
                        break;

                    case 6:
                        data.type = TELEGRAM_TYPE_CONTROL_FUNCTION;
                        data.payload = (raw & 0xFF); // Function number
                        break;

                    case 7:
                        if(((raw >> 24) & 0xFF) == 0x7F)
                        {
                            data.type = TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE;
                            data.payload = raw & 0xFFFFFF; // MAC Address
                            data.serial_number = 0;
                        }
                        break;

                    case 8:
                        switch ((raw >> 24) & 0xF)
                        {
                            case 1:
                            case 9:
                                data.type = TELEGRAM_TYPE_SELECT_MEMORY_PAGE;
                                data.address = (raw >> 20) & 0xF;
                                data.serial_number = raw & 0xFFFFF;
                                break;

                            case 2:
                                data.type = TELEGRAM_TYPE_WRITE_MEMORY;
                                data.address = (raw >> 16) & 0xFF;
                                data.payload = raw & 0xFFFF;
                                data.serial_number = 0;
                                break;
                        }
                        break;
                }
            }
            else
            {
                // Handle 16 bit telegrams

                // For 16-bit telegrams, work on the lower 16 bits
                uint8_t first = (raw >> 12) & 0xF;
                uint8_t second = (raw >> 8) & 0xF;

                if (first == 1)
                {
                    if (second == 1)
                    {
                        data.type = TELEGRAM_TYPE_OPEN_DOOR;
                        data.address = raw & 0x3F;

                        // Door readiness
                        data.payload = (raw & (1 << 6)) != 0;

                    }
                    else if (second == 2)
                    {
                        data.type = TELEGRAM_TYPE_LIGHT;
                        data.address = 0;
                    }
                }
                else if (first == 2)
                {
                    switch (second)
                    {
                        case 1:
                            data.type = (raw & (1 << 7)) ? TELEGRAM_TYPE_DOOR_CLOSED : TELEGRAM_TYPE_DOOR_OPENED;
                            break;
                        case 2:
                            data.type = TELEGRAM_TYPE_END_OF_RINGTONE;
                            break;
                        case 4:
                            data.type = TELEGRAM_TYPE_END_OF_DOOR_READINESS;
                            break;
                        case 8:
                            data.type = TELEGRAM_TYPE_INITIALIZE_DOOR_STATION;
                            break;
                    }

                    data.address = raw & 0x3F;
                }
                else if (first == 3)
                {
                    data.type = (raw & (1 << 6)) ? TELEGRAM_TYPE_STOP_TALKING : TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL;
                    data.address = raw & 0x3F;
                }
                else if (first == 5)
                {
                    switch(second)
                    {
                        case 0:
                            switch((raw >> 4) & 0xF)
                            {
                                case 4:
                                    data.type = TELEGRAM_TYPE_PROGRAMMING_MODE;
                                    data.payload = raw & 0xF;
                                    break;
                            }
                            break;

                        case 1:
                            data.type = TELEGRAM_TYPE_RESET;
                            data.address = raw & 0xF;
                            break;

                        case 2:
                            data.type = TELEGRAM_TYPE_SEARCH_DEVICES;
                            data.payload = raw & 0xF;
                            break;

                        case 8:
                            data.type = TELEGRAM_TYPE_SELECT_DEVICE_GROUP;
                            data.payload = raw & 0xF;
                            break;

                        case 9:
                            data.type = TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET;
                            data.payload = raw & 0xF;
                            break;
                    }
                }
                else if (first == 7)
                {
                    if(raw == 0x7FFF)
                    {
                        data.type = TELEGRAM_TYPE_SEARCH_DOORMAN_DEVICES;
                    }
                }
                else if (first == 8)
                {
                    switch(second)
                    {
                        case 1:
                            data.type = TELEGRAM_TYPE_SELECT_MEMORY_PAGE;
                            data.address = (raw & 0xFF);
                            break;

                        case 4:
                            data.type = TELEGRAM_TYPE_READ_MEMORY_BLOCK;
                            data.address = (raw & 0xFF);
                            break;
                    }
                }
            }
        }

        // Generate telegram HEX
        size_t pos = 0;
        size_t len = data.is_long ? 7 : (data.type == TELEGRAM_TYPE_ACK_STATUS ? 0 : 3);
        for (int i = len; i >= 0; --i) {
            uint8_t nibble = (data.raw >> (i * 4)) & 0xF;
            data.hex[pos++] = "0123456789ABCDEF"[nibble];
        }

        return data;
    }

    const TelegramMapping telegram_mappings[] = {
        {TELEGRAM_TYPE_SEARCH_DOORMAN_DEVICES, "SEARCH_DOORMAN_DEVICES"},
        {TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE, "FOUND_DOORMAN_DEVICE"},
        {TELEGRAM_TYPE_DOOR_CALL, "DOOR_CALL"},
        {TELEGRAM_TYPE_FLOOR_CALL, "FLOOR_CALL"},
        {TELEGRAM_TYPE_INTERNAL_CALL, "INTERNAL_CALL"},
        {TELEGRAM_TYPE_CONTROL_FUNCTION, "CONTROL_FUNCTION"},
        {TELEGRAM_TYPE_START_TALKING_DOOR_CALL, "START_TALKING_DOOR_CALL"},
        {TELEGRAM_TYPE_START_TALKING, "START_TALKING"},
        {TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, "STOP_TALKING_DOOR_CALL"},
        {TELEGRAM_TYPE_STOP_TALKING, "STOP_TALKING"},
        {TELEGRAM_TYPE_OPEN_DOOR, "OPEN_DOOR"},
        {TELEGRAM_TYPE_OPEN_DOOR_LONG, "OPEN_DOOR_LONG"},
        {TELEGRAM_TYPE_LIGHT, "LIGHT"},
        {TELEGRAM_TYPE_DOOR_OPENED, "DOOR_OPENED"},
        {TELEGRAM_TYPE_DOOR_CLOSED, "DOOR_CLOSED"},
        {TELEGRAM_TYPE_END_OF_RINGTONE, "END_OF_RINGTONE"},
        {TELEGRAM_TYPE_END_OF_DOOR_READINESS, "END_OF_DOOR_READINESS"},
        {TELEGRAM_TYPE_INITIALIZE_DOOR_STATION, "INITIALIZE_DOOR_STATION"},
        {TELEGRAM_TYPE_RESET, "RESET"},
        {TELEGRAM_TYPE_SELECT_DEVICE_GROUP, "SELECT_DEVICE_GROUP"},
        {TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET, "SELECT_DEVICE_GROUP_RESET"},
        {TELEGRAM_TYPE_SEARCH_DEVICES, "SEARCH_DEVICES"},
        {TELEGRAM_TYPE_FOUND_DEVICE, "FOUND_DEVICE"},
        {TELEGRAM_TYPE_FOUND_DEVICE_SUBSYSTEM, "FOUND_DEVICE_SUBSYSTEM"},
        {TELEGRAM_TYPE_PROGRAMMING_MODE, "PROGRAMMING_MODE"},
        {TELEGRAM_TYPE_READ_MEMORY_BLOCK, "READ_MEMORY_BLOCK"},
        {TELEGRAM_TYPE_SELECT_MEMORY_PAGE, "SELECT_MEMORY_PAGE"},
        {TELEGRAM_TYPE_WRITE_MEMORY, "WRITE_MEMORY"},
        {TELEGRAM_TYPE_REQUEST_VERSION, "REQUEST_VERSION"},
        {TELEGRAM_TYPE_ACK_STATUS, "ACK_STATUS"},
        {TELEGRAM_TYPE_ACK_DATA, "ACK_DATA"},
    };
    
    const char* telegram_type_to_string(TelegramType type)
    {
        for (const auto& mapping : telegram_mappings) {
            if (mapping.type == type) {
                return mapping.name;
            }
        }
        return "UNKNOWN";
    }

    TelegramType string_to_telegram_type(const char* str)
    {
        if (str == nullptr) return TELEGRAM_TYPE_UNKNOWN;

        for (const auto& mapping : telegram_mappings) {
            // Compare lengths first
            if (strlen(str) == strlen(mapping.name)) {
                bool match = true;
                for (size_t i = 0; i < strlen(str); ++i) {
                    if (toupper(str[i]) != toupper(mapping.name[i])) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    return mapping.type;
                }
            }
        }

        return TELEGRAM_TYPE_UNKNOWN;
    }

}