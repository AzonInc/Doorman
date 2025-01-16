#include "protocol.h"
#include "esphome.h"
#include "esphome/core/helpers.h"
#include <algorithm>
#include <cctype>

namespace esphome
{
    namespace tc_bus
    {
        CommandData buildCommand(CommandType type, uint8_t address, uint32_t payload, uint32_t serial_number)
        {
            CommandData data{};
            data.command = 0;
            data.type = type;
            data.address = address;
            data.payload = payload;
            data.serial_number = serial_number;
            data.is_long = true;

            switch (type)
            {
                case COMMAND_TYPE_DOOR_CALL:
                    data.command |= (0 << 28);  // 0
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= (1 << 7);  // 8
                    data.command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_INTERNAL_CALL:
                    data.command |= (0 << 28);  // 0
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command &= ~(1 << 7);  // 0
                    data.command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_FLOOR_CALL:
                    data.command |= (1 << 28);  // 1
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= 0x41; // 41
                    break;

                case COMMAND_TYPE_START_TALKING_DOOR_CALL:
                    data.command |= (3 << 28); // 3
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= (1 << 7);  // 8
                    data.command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_START_TALKING:
                    data.command |= (3 << 28); // 3
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command &= ~(1 << 7); // 0
                    data.command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_STOP_TALKING_DOOR_CALL:
                    data.is_long = false;
                    data.command |= (3 << 12); // 3
                    data.command |= (1 << 7);  // 08
                    data.command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_STOP_TALKING:
                    data.is_long = false;
                    data.command |= (3 << 12); // 3
                    data.command &= ~(1 << 7); // 00
                    data.command |= (address & 0x3F); // 0
                    break;

                case COMMAND_TYPE_OPEN_DOOR:
                    data.is_long = false;
                    data.command |= (1 << 12); // 1
                    data.command |= (1 << 8); // 1
                    data.command |= (address & 0x3F); // 00
                    break;

                case COMMAND_TYPE_OPEN_DOOR_LONG:
                    if(serial_number == 0)
                    {
                        data.is_long = false;
                        data.command |= (1 << 12); // 1
                        data.command |= (1 << 8); // 1
                        data.command |= (address & 0x3F); // 00
                    }
                    else
                    {
                        data.command |= (1 << 28);  // 1
                        data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                        data.command |= (1 << 7);  // 8
                        data.command |= (address & 0x3F); // 0
                    }
                    break;

                case COMMAND_TYPE_LIGHT:
                    data.is_long = false;
                    data.command |= (1 << 12); // 1
                    data.command |= (2 << 8);  // 2
                    break;

                case COMMAND_TYPE_CONTROL_FUNCTION:
                    data.command |= (6 << 28); // 6
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= (payload & 0xFF); // 08
                    break;

                case COMMAND_TYPE_REQUEST_VERSION:
                    data.command |= (5 << 28); // 5
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= (0xC0 & 0xFF); // C0
                    break;

                case COMMAND_TYPE_RESET:
                    data.is_long = false;
                    data.command |= (5 << 12); // 5
                    data.command |= (1 << 8);  // 100
                    break;

                case COMMAND_TYPE_SEARCH_DOORMAN_DEVICES:
                    data.is_long = false;    
                    data.command = 0x7FFF;
                    break;

                case COMMAND_TYPE_FOUND_DOORMAN_DEVICE:
                    data.command |= (0x7F << 24); // 7F
                    data.command |= payload & 0xFFFFFF; // MAC address
                    break;

                case COMMAND_TYPE_SELECT_DEVICE_GROUP:
                    data.is_long = false;
                    data.command |= (5 << 12); // 5
                    data.command |= (8 << 8);  // 80
                    data.command |= (payload & 0xFF); // 0
                    break;

                case COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET:
                    data.is_long = false;
                    data.command |= (5 << 12); // 5
                    data.command |= (9 << 8);  // 90
                    data.command |= (payload & 0xFF); // 0
                    break;

                case COMMAND_TYPE_SEARCH_DEVICES:
                    data.is_long = false;
                    data.command |= (5 << 12); // 5
                    data.command |= (2 << 8);  // 20
                    break;

                case COMMAND_TYPE_PROGRAMMING_MODE:
                    data.is_long = false;
                    data.command |= (5 << 12); // 5
                    data.command |= (0 << 8);  // 0
                    data.command |= (4 << 4);  // 4
                    data.command |= (payload & 0xF); // 0 / 1
                    break;

                case COMMAND_TYPE_READ_MEMORY_BLOCK:
                    data.is_long = false;
                    data.command |= (8 << 12); // 8
                    data.command |= (4 << 8);  // 4
                    data.command |= ((data.address * 4) & 0xFF); // 00
                    break;

                case COMMAND_TYPE_WRITE_MEMORY:
                    data.command |= (8 << 28); // 8
                    data.command |= (2 << 24); // 2
                    data.command |= (address & 0xFF) << 16; // start address
                    data.command |= payload & 0xFFFF; // ABCD payload
                    break;

                case COMMAND_TYPE_SELECT_MEMORY_PAGE:
                    data.command |= (8 << 28); // 8
                    data.command |= (1 << 24); // 1
                    data.command |= (address & 0xF) << 20; // page
                    data.command |= serial_number & 0xFFFFF;
                    break;

                default:
                    break;
            }

            return data;
        }

        CommandData parseCommand(uint32_t command, bool is_long)
        {
            CommandData data{};
            data.command = command;
            data.type = COMMAND_TYPE_UNKNOWN;
            data.address = 0;
            data.payload = 0;
            data.is_long = is_long;

            data.command_hex = str_upper_case(format_hex(command));

            if (is_long)
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
                if (data.command_hex.substr(0, 4) == "0000")
                {
                    data.command_hex = data.command_hex.substr(4);
                }

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
            if (str == "OPEN_DOOR_LONG") return COMMAND_TYPE_OPEN_DOOR_LONG;
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
                case COMMAND_TYPE_OPEN_DOOR_LONG: return "OPEN_DOOR_LONG";
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
            if (str == "TCS ISW3030 / Koch TC50") return MODEL_ISW3030;
            if (str == "TCS ISW3130 / Koch TC50P") return MODEL_ISW3130;
            if (str == "TCS ISW3230 / Koch TC50 GFA") return MODEL_ISW3230;
            if (str == "TCS ISW3330 / Koch TC50 BW") return MODEL_ISW3330;
            if (str == "TCS ISW3340") return MODEL_ISW3340;
            if (str == "TCS ISW5010 / Koch TC60") return MODEL_ISW5010;
            if (str == "TCS ISW5020") return MODEL_ISW5020;
            if (str == "TCS ISW5030") return MODEL_ISW5030;
            if (str == "TCS ISW5031") return MODEL_ISW5031;
            if (str == "TCS ISW5033") return MODEL_ISW5033;
            if (str == "TCS ISW6031") return MODEL_ISW6031;
            if (str == "TCS ISW7030 / Koch TC70") return MODEL_ISW7030;
            if (str == "TCS IVW7510 / Koch VTC70") return MODEL_IVW7510;
            if (str == "TCS ISH7030 / Koch TCH70") return MODEL_ISH7030;
            if (str == "TCS IVH7510 / Koch VTCH70") return MODEL_IVH7510;
            if (str == "TCS ISW6010") return MODEL_ISW6010;
            if (str == "TCS IVW6511") return MODEL_IVW6511;
            if (str == "TCS ISWM7000") return MODEL_ISWM7000;
            if (str == "TCS IVWM7000") return MODEL_IVWM7000;
            if (str == "TCS ISW4100 / Koch TC31") return MODEL_ISW4100;
            if (str == "TCS IMM2100 / Koch TCE31") return MODEL_IMM2100;
            if (str == "TCS IVW2210 / Koch Ecoos") return MODEL_IVW2210;
            if (str == "TCS IVW2211 / Koch Ecoos") return MODEL_IVW2211;
            if (str == "TCS IVW2212 / Koch Ecoos") return MODEL_IVW2212;
            if (str == "TCS VTC42V2") return MODEL_VTC42V2;
            if (str == "TCS TC40V2") return MODEL_TC40V2;
            if (str == "TCS VTC40") return MODEL_VTC40;
            if (str == "TCS TC40") return MODEL_TC40;
            if (str == "TCS TC2000") return MODEL_TC2000;
            if (str == "TCS TC20P") return MODEL_TC20P;
            if (str == "TCS TC20F") return MODEL_TC20F;
            if (str == "TCS ISH3340") return MODEL_ISH3340;
            if (str == "TCS ISH3022 / Koch TCH50P") return MODEL_ISH3022;
            if (str == "TCS ISH3130 / Koch TCH50P") return MODEL_ISH3130;
            if (str == "TCS ISW3022") return MODEL_ISW3022;
            if (str == "TCS ISH3230 / Koch TCH50 GFA") return MODEL_ISH3230;
            if (str == "TCS ISH3030 / Koch TCH50") return MODEL_ISH3030;
            if (str == "TCS ISH1030 / Koch TTS25") return MODEL_ISH1030;
            if (str == "TCS IMM1000 / Koch TCH30") return MODEL_IMM1000;
            if (str == "TCS IMM1100 / Koch TCHE30") return MODEL_IMM1100;
            if (str == "TCS IMM1300 / Koch VTCH30") return MODEL_IMM1300;
            if (str == "TCS IMM1500") return MODEL_IMM1500;
            if (str == "TCS IMM1310 / Koch VTCHE30") return MODEL_IMM1310;
            if (str == "TCS IMM1110 / Koch TCHEE30") return MODEL_IMM1110;
            if (str == "TCS IVH3222 / Koch VTCH50") return MODEL_IVH3222;
            if (str == "TCS IVH4222 / Koch VTCH50/2D") return MODEL_IVH4222;

            return MODEL_NONE;
        }

        const char* model_to_string(Model model)
        {
            switch (model)
            {
                case MODEL_ISW3030: return "TCS ISW3030 / Koch TC50";
                case MODEL_ISW3130: return "TCS ISW3130 / Koch TC50P";
                case MODEL_ISW3230: return "TCS ISW3230 / Koch TC50 GFA";
                case MODEL_ISW3330: return "TCS ISW3330 / Koch TC50 BW";
                case MODEL_ISW3340: return "TCS ISW3340";
                case MODEL_ISW5010: return "TCS ISW5010 / Koch TC60";
                case MODEL_ISW5020: return "TCS ISW5020";
                case MODEL_ISW5030: return "TCS ISW5030";
                case MODEL_ISW5031: return "TCS ISW5031";
                case MODEL_ISW5033: return "TCS ISW5033";
                case MODEL_ISW6031: return "TCS ISW6031";
                case MODEL_ISW7030: return "TCS ISW7030 / Koch TC70";
                case MODEL_IVW7510: return "TCS IVW7510 / Koch VTC70";
                case MODEL_ISH7030: return "TCS ISH7030 / Koch TCH70";
                case MODEL_IVH7510: return "TCS IVH7510 / Koch VTCH70";
                case MODEL_ISW6010: return "TCS ISW6010";
                case MODEL_IVW6511: return "TCS IVW6511";
                case MODEL_ISWM7000: return "TCS ISWM7000";
                case MODEL_IVWM7000: return "TCS IVWM7000";
                case MODEL_ISW4100: return "TCS ISW4100 / Koch TC31";
                case MODEL_IMM2100: return "TCS IMM2100 / Koch TCE31";
                case MODEL_IVW2210: return "TCS IVW2210 / Koch Ecoos";
                case MODEL_IVW2211: return "TCS IVW2211 / Koch Ecoos";
                case MODEL_IVW2212: return "TCS IVW2212 / Koch Ecoos";
                case MODEL_VTC42V2: return "TCS VTC42V2";
                case MODEL_TC40V2: return "TCS TC40V2";
                case MODEL_VTC40: return "TCS VTC40";
                case MODEL_TC40: return "TCS TC40";
                case MODEL_TC2000: return "TCS TC2000";
                case MODEL_TC20P: return "TCS TC20P";
                case MODEL_TC20F: return "TCS TC20F";
                case MODEL_ISH3340: return "TCS ISH3340";
                case MODEL_ISH3022: return "TCS ISH3022";
                case MODEL_ISH3130: return "TCS ISH3130 / Koch TCH50P";
                case MODEL_ISW3022: return "TCS ISW3022";
                case MODEL_ISH3230: return "TCS ISH3230 / Koch TCH50 GFA";
                case MODEL_ISH3030: return "TCS ISH3030 / Koch TCH50";
                case MODEL_ISH1030: return "TCS ISH1030 / Koch TTS25";
                case MODEL_IMM1000: return "TCS IMM1000 / Koch TCH30";
                case MODEL_IMM1100: return "TCS IMM1100 / Koch TCHE30";
                case MODEL_IMM1300: return "TCS IMM1300 / Koch VTCH30";
                case MODEL_IMM1500: return "TCS IMM1500";
                case MODEL_IMM1310: return "TCS IMM1310 / Koch VTCHE30";
                case MODEL_IMM1110: return "TCS IMM1110 / Koch TCHEE30";
                case MODEL_IVH3222: return "TCS IVH3222 / Koch VTCH50";
                case MODEL_IVH4222: return "TCS IVH4222 / Koch VTCH50/2D";
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