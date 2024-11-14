#include "protocol.h"
#include "esphome.h"
#include "esphome/core/helpers.h"
#include <algorithm>
#include <cctype>

namespace esphome
{
    namespace tc_bus
    {
        uint32_t buildCommand(CommandType type, uint8_t address, uint32_t payload, uint32_t serial_number)
        {
            uint32_t command = 0;

            switch (type)
            {
                case COMMAND_TYPE_DOOR_CALL:
                    command |= (0 << 28);  // 0
                    command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    command |= (1 << 7);  // 8
                    command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_INTERNAL_CALL:
                    command |= (0 << 28);  // 0
                    command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    command &= ~(1 << 7);  // 0
                    command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_FLOOR_CALL:
                    command |= (1 << 28);  // 1
                    command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    command |= 0x41; // 41
                    break;

                case COMMAND_TYPE_START_TALKING_DOOR_CALL:
                    command |= (3 << 28); // 3
                    command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    command |= (1 << 7);  // 8
                    command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_START_TALKING:
                    command |= (3 << 28); // 3
                    command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    command &= ~(1 << 7); // 0
                    command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_STOP_TALKING_DOOR_CALL:
                    command |= (3 << 12); // 3
                    command |= (1 << 7);  // 08
                    command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_STOP_TALKING:
                    command |= (3 << 12); // 3
                    command &= ~(1 << 7); // 00
                    command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_OPEN_DOOR:
                    if(serial_number == 0)
                    {
                        command |= (1 << 12); // 1
                        command |= (1 << 8); // 1
                        command |= (address & 0x3F); // 00
                    }
                    else
                    {
                        command |= (1 << 28);  // 1
                        command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                        command |= (1 << 7);  // 8
                        command |= (address & 0x3F); // 0
                    }
                    break;

                case COMMAND_TYPE_LIGHT:
                    command |= (1 << 12); // 1
                    command |= (2 << 8);  // 2
                    break;

                case COMMAND_TYPE_CONTROL_FUNCTION:
                    command |= (6 << 28); // 6
                    command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    command |= (payload & 0xFF); // 08
                    break;

                case COMMAND_TYPE_REQUEST_VERSION:
                    command |= (5 << 28); // 5
                    command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    command |= (0xC0 & 0xFF); // C0
                    break;

                case COMMAND_TYPE_RESET:
                    command |= (5 << 12); // 5
                    command |= (1 << 8);  // 100
                    break;

                case COMMAND_TYPE_SEARCH_DOORMAN_DEVICES:
                    command = 0x7FFF;
                    break;

                case COMMAND_TYPE_FOUND_DOORMAN_DEVICE:
                    command |= (0x7F << 24); // 7F
                    command |= payload & 0xFFFFFF; // MAC address
                    break;

                case COMMAND_TYPE_SELECT_DEVICE_GROUP:
                    command |= (5 << 12); // 5
                    command |= (8 << 8);  // 80
                    command |= (payload & 0xFF); // 0
                    break;

                case COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET:
                    command |= (5 << 12); // 5
                    command |= (9 << 8);  // 90
                    command |= (payload & 0xFF); // 0
                    break;

                case COMMAND_TYPE_SEARCH_DEVICES:
                    command |= (5 << 12); // 5
                    command |= (2 << 8);  // 20
                    break;

                case COMMAND_TYPE_PROGRAMMING_MODE:
                    command |= (5 << 12); // 5
                    command |= (0 << 8);  // 0
                    command |= (4 << 4);  // 4
                    command |= (payload & 0xF); // 0 / 1
                    break;

                case COMMAND_TYPE_READ_MEMORY_BLOCK:
                    command |= (8 << 12); // 8
                    command |= (4 << 8);  // 4
                    command |= ((address * 4) & 0xFF); // 00
                    break;

                case COMMAND_TYPE_WRITE_MEMORY:
                    command |= (8 << 28); // 8
                    command |= (2 << 24); // 2
                    command |= (address & 0xFF) << 16; // start address
                    command |= payload & 0xFFFF; // ABCD payload
                    break;

                case COMMAND_TYPE_SELECT_MEMORY_PAGE:
                    command |= (8 << 28); // 8
                    command |= (1 << 24); // 1
                    command |= (address & 0xF) << 20; // page
                    command |= serial_number & 0xFFFFF;
                    break;

                default:
                    break;
            }

            return command;
        }

        CommandData parseCommand(uint32_t command)
        {
            CommandData data{};
            data.command = command;
            data.type = COMMAND_TYPE_UNKNOWN;
            data.address = 0;
            data.payload = 0;

            // Convert to HEX and determine length
            data.command_hex = str_upper_case(format_hex(command));
            data.length = (command >> 16 == 0) ? 16 : 32;

            if (data.length == 32)
            {
                data.serial_number = (command >> 8) & 0xFFFFF; // Serial (from bits 8 to 23)

                switch ((command >> 28) & 0xF)
                {
                    case 0:
                        data.type = (command & (1 << 7)) ? COMMAND_TYPE_DOOR_CALL : COMMAND_TYPE_INTERNAL_CALL;
                        data.address = command & 0x3F;
                        break;

                    case 1:
                        if ((command & 0xFF) == 0x41)
                        {
                            data.type = COMMAND_TYPE_FLOOR_CALL;
                        }
                        else if (command & (1 << 7))
                        {
                            data.type = COMMAND_TYPE_OPEN_DOOR;
                            data.address = command & 0x3F;
                        }
                        break;

                    case 3:
                        data.type = (command & (1 << 7)) ? COMMAND_TYPE_START_TALKING_DOOR_CALL : COMMAND_TYPE_START_TALKING;
                        data.address = command & 0x3F;

                        // Door Readiness
                        if(data.type == COMMAND_TYPE_START_TALKING_DOOR_CALL)
                        {
                            data.payload = (command & (1 << 8)) != 0;
                        }
                        break;

                    case 5:
                        switch ((command >> 4) & 0xF)
                        {
                            case 1:
                                data.type = COMMAND_TYPE_FOUND_DEVICE;
                                data.address = command & 0xF;
                                break;
                            
                            case 4:
                                data.type = COMMAND_TYPE_FOUND_DEVICE_SUBSYSTEM;
                                data.address = command & 0xF;
                                break;

                            case 8:
                                // Device test
                                // Not implemented
                                break;

                            case 0xC:
                                data.type = COMMAND_TYPE_REQUEST_VERSION;

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
                        data.type = COMMAND_TYPE_CONTROL_FUNCTION;
                        data.payload = (command & 0xFF); // Function number
                        break;

                    case 7:
                        if(((command >> 24) & 0xFF) == 0x7F)
                        {
                            data.type = COMMAND_TYPE_FOUND_DOORMAN_DEVICE;
                            data.payload = command & 0xFFFFFF; // MAC Address
                        }
                        break;

                    case 8:
                        switch ((command >> 24) & 0xF)
                        {
                            case 1:
                            case 9:
                                data.type = COMMAND_TYPE_SELECT_MEMORY_PAGE;
                                data.address = (command >> 20) & 0xF;
                                data.serial_number = command & 0xFFFFF;
                                break;

                            case 2:
                                data.type = COMMAND_TYPE_WRITE_MEMORY;
                                data.address = (command >> 16) & 0xFF;
                                data.payload = command & 0xFFFF;
                                break;
                        }
                        break;
                }
            }
            else
            {
                // For 16-bit commands, work on the lower 16 bits
                uint8_t first = (command >> 12) & 0xF;
                uint8_t second = (command >> 8) & 0xF;

                if (first == 1)
                {
                    if (second == 1)
                    {
                        data.type = COMMAND_TYPE_OPEN_DOOR;
                        data.address = command & 0x3F;
                    }
                    else if (second == 2)
                    {
                        data.type = COMMAND_TYPE_LIGHT;
                        data.address = 0;
                    }
                }
                else if (first == 2)
                {
                    switch (second)
                    {
                        case 1:
                            data.type = (command & (1 << 7)) ? COMMAND_TYPE_DOOR_CLOSED : COMMAND_TYPE_DOOR_OPENED;
                            break;
                        case 2:
                            data.type = COMMAND_TYPE_END_OF_RINGTONE;
                            break;
                        case 4:
                            data.type = COMMAND_TYPE_END_OF_DOOR_READINESS;
                            break;
                        case 8:
                            data.type = COMMAND_TYPE_INITIALIZE_DOOR_STATION;
                            break;
                    }

                    data.address = command & 0x3F;
                }
                else if (first == 3)
                {
                    data.type = (command & (1 << 7)) ? COMMAND_TYPE_STOP_TALKING_DOOR_CALL : COMMAND_TYPE_STOP_TALKING;
                    data.address = command & 0x3F;
                }
                else if (first == 5)
                {
                    switch(second)
                    {
                        case 0:
                            switch((command >> 4) & 0xF)
                            {
                                case 4:
                                    data.type = COMMAND_TYPE_PROGRAMMING_MODE;
                                    data.payload = command & 0xF;
                                    break;
                            }
                            break;

                        case 1:
                            data.type = COMMAND_TYPE_RESET;
                            data.address = command & 0xF;
                            break;

                        case 2:
                            data.type = COMMAND_TYPE_SEARCH_DEVICES;
                            data.payload = command & 0xF;
                            break;

                        case 8:
                            data.type = COMMAND_TYPE_SELECT_DEVICE_GROUP;
                            data.payload = command & 0xF;
                            break;

                        case 9:
                            data.type = COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET;
                            data.payload = command & 0xF;
                            break;
                    }
                }
                else if (first == 7)
                {
                    if(command == 0x7FFF)
                    {
                        data.type = COMMAND_TYPE_SEARCH_DOORMAN_DEVICES;
                    }
                }
                else if (first == 8)
                {
                    switch(second)
                    {
                        case 1:
                            data.type = COMMAND_TYPE_SELECT_MEMORY_PAGE;
                            data.address = (command & 0xFF);
                            break;

                        case 4:
                            data.type = COMMAND_TYPE_READ_MEMORY_BLOCK;
                            data.address = (command & 0xFF) / 4;
                            break;
                    }
                }
            }

            return data;
        }

        

        SettingType string_to_setting_type(std::string str)
        {
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);

            if (str == "RINGTONE_FLOOR_CALL") return SETTING_RINGTONE_FLOOR_CALL;
            if (str == "RINGTONE_DOOR_CALL") return SETTING_RINGTONE_DOOR_CALL;
            if (str == "RINGTONE_INTERNAL_CALL") return SETTING_RINGTONE_INTERNAL_CALL;
            if (str == "VOLUME_RINGTONE") return SETTING_VOLUME_RINGTONE;
            if (str == "VOLUME_HANDSET") return SETTING_VOLUME_HANDSET;

            return SETTING_UNKNOWN;
        }

        const char* setting_type_to_string(SettingType type)
        {
            switch (type)
            {
                case SETTING_RINGTONE_FLOOR_CALL: return "RINGTONE_FLOOR_CALL";
                case SETTING_RINGTONE_DOOR_CALL: return "RINGTONE_DOOR_CALL";
                case SETTING_RINGTONE_INTERNAL_CALL: return "RINGTONE_INTERNAL_CALL";
                case SETTING_VOLUME_RINGTONE: return "VOLUME_RINGTONE";
                case SETTING_VOLUME_HANDSET: return "VOLUME_HANDSET";
                default: return "UNKNOWN";
            }
        }


        CommandType string_to_command_type(std::string str)
        {
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);

            if (str == "SEARCH_DOORMAN_DEVICES") return COMMAND_TYPE_SEARCH_DOORMAN_DEVICES;
            if (str == "FOUND_DOORMAN_DEVICE") return COMMAND_TYPE_FOUND_DOORMAN_DEVICE;
            if (str == "DOOR_CALL") return COMMAND_TYPE_DOOR_CALL;
            if (str == "FLOOR_CALL") return COMMAND_TYPE_FLOOR_CALL;
            if (str == "INTERNAL_CALL") return COMMAND_TYPE_INTERNAL_CALL;
            if (str == "CONTROL_FUNCTION") return COMMAND_TYPE_CONTROL_FUNCTION;
            if (str == "START_TALKING_DOOR_CALL") return COMMAND_TYPE_START_TALKING_DOOR_CALL;
            if (str == "START_TALKING") return COMMAND_TYPE_START_TALKING;
            if (str == "STOP_TALKING_DOOR_CALL") return COMMAND_TYPE_STOP_TALKING_DOOR_CALL;
            if (str == "STOP_TALKING") return COMMAND_TYPE_STOP_TALKING;
            if (str == "OPEN_DOOR") return COMMAND_TYPE_OPEN_DOOR;
            if (str == "LIGHT") return COMMAND_TYPE_LIGHT;
            if (str == "DOOR_OPENED") return COMMAND_TYPE_DOOR_OPENED;
            if (str == "DOOR_CLOSED") return COMMAND_TYPE_DOOR_CLOSED;
            if (str == "END_OF_RINGTONE") return COMMAND_TYPE_END_OF_RINGTONE;
            if (str == "END_OF_DOOR_READINESS") return COMMAND_TYPE_END_OF_DOOR_READINESS;
            if (str == "INITIALIZE_DOOR_STATION") return COMMAND_TYPE_INITIALIZE_DOOR_STATION;
            if (str == "RESET") return COMMAND_TYPE_RESET;
            if (str == "SELECT_DEVICE_GROUP") return COMMAND_TYPE_SELECT_DEVICE_GROUP;
            if (str == "SELECT_DEVICE_GROUP_RESET") return COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET;
            if (str == "SEARCH_DEVICES") return COMMAND_TYPE_SEARCH_DEVICES;
            if (str == "FOUND_DEVICE") return COMMAND_TYPE_FOUND_DEVICE;
            if (str == "FOUND_DEVICE_SUBSYSTEM") return COMMAND_TYPE_FOUND_DEVICE_SUBSYSTEM;
            if (str == "PROGRAMMING_MODE") return COMMAND_TYPE_PROGRAMMING_MODE;
            if (str == "READ_MEMORY_BLOCK") return COMMAND_TYPE_READ_MEMORY_BLOCK;
            if (str == "SELECT_MEMORY_PAGE") return COMMAND_TYPE_SELECT_MEMORY_PAGE;
            if (str == "WRITE_MEMORY") return COMMAND_TYPE_WRITE_MEMORY;
            if (str == "REQUEST_VERSION") return COMMAND_TYPE_REQUEST_VERSION;

            return COMMAND_TYPE_UNKNOWN;
        }

        const char* command_type_to_string(CommandType type)
        {
            switch (type)
            {
                case COMMAND_TYPE_SEARCH_DOORMAN_DEVICES: return "SEARCH_DOORMAN_DEVICES";
                case COMMAND_TYPE_FOUND_DOORMAN_DEVICE: return "FOUND_DOORMAN_DEVICE";
                case COMMAND_TYPE_DOOR_CALL: return "DOOR_CALL";
                case COMMAND_TYPE_FLOOR_CALL: return "FLOOR_CALL";
                case COMMAND_TYPE_INTERNAL_CALL: return "INTERNAL_CALL";
                case COMMAND_TYPE_CONTROL_FUNCTION: return "CONTROL_FUNCTION";
                case COMMAND_TYPE_START_TALKING_DOOR_CALL: return "START_TALKING_DOOR_CALL";
                case COMMAND_TYPE_START_TALKING: return "START_TALKING";
                case COMMAND_TYPE_STOP_TALKING_DOOR_CALL: return "STOP_TALKING_DOOR_CALL";
                case COMMAND_TYPE_STOP_TALKING: return "STOP_TALKING";
                case COMMAND_TYPE_OPEN_DOOR: return "OPEN_DOOR";
                case COMMAND_TYPE_LIGHT: return "LIGHT";
                case COMMAND_TYPE_DOOR_OPENED: return "DOOR_OPENED";
                case COMMAND_TYPE_DOOR_CLOSED: return "DOOR_CLOSED";
                case COMMAND_TYPE_END_OF_RINGTONE: return "END_OF_RINGTONE";
                case COMMAND_TYPE_END_OF_DOOR_READINESS: return "END_OF_DOOR_READINESS";
                case COMMAND_TYPE_INITIALIZE_DOOR_STATION: return "INITIALIZE_DOOR_STATION";
                case COMMAND_TYPE_RESET: return "RESET";
                case COMMAND_TYPE_SELECT_DEVICE_GROUP: return "SELECT_DEVICE_GROUP";
                case COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET: return "SELECT_DEVICE_GROUP_RESET";
                case COMMAND_TYPE_SEARCH_DEVICES: return "SEARCH_DEVICES";
                case COMMAND_TYPE_FOUND_DEVICE: return "FOUND_DEVICE";
                case COMMAND_TYPE_FOUND_DEVICE_SUBSYSTEM: return "FOUND_DEVICE_SUBSYSTEM";
                case COMMAND_TYPE_PROGRAMMING_MODE: return "PROGRAMMING_MODE";
                case COMMAND_TYPE_READ_MEMORY_BLOCK: return "READ_MEMORY_BLOCK";
                case COMMAND_TYPE_SELECT_MEMORY_PAGE: return "SELECT_MEMORY_PAGE";
                case COMMAND_TYPE_WRITE_MEMORY: return "WRITE_MEMORY";
                case COMMAND_TYPE_REQUEST_VERSION: return "REQUEST_VERSION";
                default: return "UNKNOWN";
            }
        }

        Model string_to_model(std::string str)
        {
            if (str == "TCS ISH1030") return MODEL_TCS_ISH1030;
            if (str == "TCS ISH3030") return MODEL_TCS_ISH3030;
            if (str == "TCS ISH3230") return MODEL_TCS_ISH3230;
            if (str == "TCS ISH3340") return MODEL_TCS_ISH3340;
            if (str == "TCS ISW3030") return MODEL_TCS_ISW3030;
            if (str == "TCS ISW3230") return MODEL_TCS_ISW3230;
            if (str == "TCS ISW3340") return MODEL_TCS_ISW3340;
            if (str == "TCS IVH3222") return MODEL_TCS_IVH3222;
            if (str == "Koch TC50") return MODEL_KOCH_TC50;
            if (str == "Koch TCH50") return MODEL_KOCH_TCH50;
            if (str == "Koch TCH50P") return MODEL_KOCH_TCH50P;

            return MODEL_NONE;
        }

        const char* model_to_string(Model model)
        {
            switch (model)
            {
                case MODEL_TCS_ISH1030: return "TCS ISH1030";
                case MODEL_TCS_ISH3030: return "TCS ISH3030";
                case MODEL_TCS_ISH3230: return "TCS ISH3230";
                case MODEL_TCS_ISH3340: return "TCS ISH3340";
                case MODEL_TCS_ISW3030: return "TCS ISW3030";
                case MODEL_TCS_ISW3230: return "TCS ISW3230";
                case MODEL_TCS_ISW3340: return "TCS ISW3340";
                case MODEL_TCS_IVH3222: return "TCS IVH3222";
                case MODEL_KOCH_TC50: return "Koch TC50";
                case MODEL_KOCH_TCH50: return "Koch TCH50";
                case MODEL_KOCH_TCH50P: return "Koch TCH50P";
                default: return "None";
            }
        }

        uint8_t ringtone_to_int(std::string str)
        {
            if(str == "Ringtone 1") return 0;
            if(str == "Ringtone 2") return 1;
            if(str == "Ringtone 3") return 2;
            if(str == "Ringtone 4") return 3;
            if(str == "Ringtone 5") return 4;
            if(str == "Ringtone 6") return 5;
            if(str == "Ringtone 7") return 6;
            if(str == "Ringtone 8") return 7;
            if(str == "Ringtone 9") return 8;
            if(str == "Ringtone 10") return 9;
            if(str == "Ringtone 11") return 10;
            if(str == "Ringtone 12") return 11;
            if(str == "Ringtone 13") return 12;
            return 0;
        }

        std::string int_to_ringtone(uint8_t ringtone)
        {
            if(ringtone > 12)
            {
                ringtone = 0;
            }

            ringtone = ringtone + 1;

            return "Ringtone " + std::to_string(ringtone);
        }

    }  // namespace tc_bus
}  // namespace esphome