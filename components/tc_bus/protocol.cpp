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
            data.is_long = true;

            switch (type)
            {
                case COMMAND_TYPE_DOOR_CALL:
                    data.command |= (0 << 28);  // 0
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= (1 << 7);  // 8
                    data.command |= (address & 0x3F); // 0
                    data.serial_number = serial_number;
                    data.address = address;
                    break;

                case COMMAND_TYPE_INTERNAL_CALL:
                    data.command |= (0 << 28);  // 0
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command &= ~(1 << 7);  // 0
                    data.command |= (address & 0x3F); // 0
                    data.serial_number = serial_number;
                    data.address = address;
                    break;

                case COMMAND_TYPE_FLOOR_CALL:
                    data.command |= (1 << 28);  // 1
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= 0x41; // 41
                    data.serial_number = serial_number;
                    break;

                case COMMAND_TYPE_START_TALKING_DOOR_CALL:
                    data.command |= (3 << 28); // 3
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= (1 << 7);  // 8
                    data.command |= (address & 0x3F); // 0
                    data.serial_number = serial_number;
                    data.address = address;
                    break;

                case COMMAND_TYPE_START_TALKING:
                    data.command |= (3 << 28); // 3
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command &= ~(1 << 7); // 0
                    data.command |= (address & 0x3F); // 0
                    data.serial_number = serial_number;
                    data.address = address;
                    break;

                case COMMAND_TYPE_STOP_TALKING_DOOR_CALL:
                    data.is_long = false;
                    data.command |= (3 << 12); // 3
                    data.command |= (1 << 7);  // 08
                    data.command |= (address & 0x3F); // 0
                    data.address = address;
                    break;

                case COMMAND_TYPE_STOP_TALKING:
                    data.is_long = false;
                    data.command |= (3 << 12); // 3
                    data.command &= ~(1 << 7); // 00
                    data.command |= (address & 0x3F); // 0
                    data.address = address;
                    break;

                case COMMAND_TYPE_OPEN_DOOR:
                    data.is_long = false;
                    data.command |= (1 << 12); // 1
                    data.command |= (1 << 8); // 1
                    data.command |= (address & 0x3F); // 00
                    data.address = address;
                    break;

                case COMMAND_TYPE_OPEN_DOOR_LONG:
                    if(serial_number == 0)
                    {
                        data.is_long = false;
                        data.command |= (1 << 12); // 1
                        data.command |= (1 << 8); // 1
                        data.command |= (address & 0x3F); // 00
                        data.address = address;
                    }
                    else
                    {
                        data.command |= (1 << 28);  // 1
                        data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                        data.command |= (1 << 7);  // 8
                        data.command |= (address & 0x3F); // 0
                        data.serial_number = serial_number;
                        data.address = address;
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
                    data.payload = payload;
                    break;

                case COMMAND_TYPE_REQUEST_VERSION:
                    data.command |= (5 << 28); // 5
                    data.command |= ((serial_number & 0xFFFFF) << 8); // C30BA
                    data.command |= (0xC0 & 0xFF); // C0
                    data.serial_number = serial_number;
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
                    data.payload = payload;
                    break;

                case COMMAND_TYPE_SELECT_DEVICE_GROUP:
                    data.is_long = false;
                    data.command |= (5 << 12); // 5
                    data.command |= (8 << 8);  // 80
                    data.command |= (payload & 0xFF); // 0
                    data.payload = payload;
                    break;

                case COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET:
                    data.is_long = false;
                    data.command |= (5 << 12); // 5
                    data.command |= (9 << 8);  // 90
                    data.command |= (payload & 0xFF); // 0
                    data.payload = payload;
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
                    data.payload = payload;
                    break;

                case COMMAND_TYPE_READ_MEMORY_BLOCK:
                    data.is_long = false;
                    data.command |= (8 << 12); // 8
                    data.command |= (4 << 8);  // 4
                    data.command |= ((data.address * 4) & 0xFF); // 00
                    data.address = address;
                    break;

                case COMMAND_TYPE_WRITE_MEMORY:
                    data.command |= (8 << 28); // 8
                    data.command |= (2 << 24); // 2
                    data.command |= (address & 0xFF) << 16; // start address
                    data.command |= payload & 0xFFFF; // ABCD payload
                    data.address = address;
                    data.payload = payload;
                    break;

                case COMMAND_TYPE_SELECT_MEMORY_PAGE:
                    data.command |= (8 << 28); // 8
                    data.command |= (1 << 24); // 1
                    data.command |= (address & 0xF) << 20; // page
                    data.command |= serial_number & 0xFFFFF;
                    data.serial_number = serial_number;
                    data.address = address;
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
                            data.serial_number = 0;
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
                                data.serial_number = 0;
                                break;
                        }
                        break;
                }
            }
            else
            {
                data.command_hex = data.command_hex.substr(4);

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
            if (str == "RINGTONE_ENTRANCE_DOOR_CALL") return SETTING_RINGTONE_ENTRANCE_DOOR_CALL;
            if (str == "RINGTONE_SECOND_ENTRANCE_DOOR_CALL") return SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
            if (str == "RINGTONE_INTERNAL_CALL") return SETTING_RINGTONE_INTERNAL_CALL;
            if (str == "VOLUME_RINGTONE") return SETTING_VOLUME_RINGTONE;
            if (str == "VOLUME_HANDSET_DOOR_CALL") return SETTING_VOLUME_HANDSET_DOOR_CALL;
            if (str == "VOLUME_HANDSET_INTERNAL_CALL") return SETTING_VOLUME_HANDSET_INTERNAL_CALL;

            return SETTING_UNKNOWN;
        }

        const char* setting_type_to_string(SettingType type)
        {
            switch (type)
            {
                case SETTING_RINGTONE_FLOOR_CALL: return "RINGTONE_FLOOR_CALL";
                case SETTING_RINGTONE_ENTRANCE_DOOR_CALL: return "RINGTONE_ENTRANCE_DOOR_CALL";
                case SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL: return "RINGTONE_SECOND_ENTRANCE_DOOR_CALL";
                case SETTING_RINGTONE_INTERNAL_CALL: return "RINGTONE_INTERNAL_CALL";
                case SETTING_VOLUME_RINGTONE: return "VOLUME_RINGTONE";
                case SETTING_VOLUME_HANDSET_DOOR_CALL: return "VOLUME_HANDSET_DOOR_CALL";
                case SETTING_VOLUME_HANDSET_INTERNAL_CALL: return "VOLUME_HANDSET_INTERNAL_CALL";
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

        Model string_to_model(const std::string& str)
        {
            if (str == "TCS ISW3030 / Koch TC50 / Scantron Stilux") return MODEL_ISW3030;
            if (str == "TCS ISW3130 / Koch TC50P") return MODEL_ISW3130;
            if (str == "TCS ISW3230 / Koch TC50 GFA") return MODEL_ISW3230;
            if (str == "TCS ISW3330 / Koch TC50 BW") return MODEL_ISW3330;
            if (str == "TCS ISW3340") return MODEL_ISW3340;
            if (str == "TCS ISW5010 / Koch TC60") return MODEL_ISW5010;
            if (str == "TCS ISW5020") return MODEL_ISW5020;
            if (str == "TCS ISW5030") return MODEL_ISW5030;
            if (str == "TCS ISW5031") return MODEL_ISW5031;
            if (str == "TCS ISW5033") return MODEL_ISW5033;
            if (str == "TCS IVW511x / Koch VTC60 / Scantron VIVO") return MODEL_IVW511X;
            if (str == "TCS IVW521x / Koch VTC60/2D") return MODEL_IVW521X;
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
            if (str == "TCS IVW2212 / Koch Ecoos / Scantron SLIM60T") return MODEL_IVW2212;
            if (str == "TCS VTC42V2") return MODEL_VTC42V2;
            if (str == "TCS TC40V2") return MODEL_TC40V2;
            if (str == "TCS VTC40") return MODEL_VTC40;
            if (str == "TCS TC40") return MODEL_TC40;
            if (str == "TCS TC2000") return MODEL_TC2000;
            if (str == "TCS TC20P") return MODEL_TC20P;
            if (str == "TCS TC20F") return MODEL_TC20F;
            if (str == "TCS ISH3022 / Koch TCH50P") return MODEL_ISH3022;
            if (str == "TCS ISH3130 / Koch TCH50P / Scantron LuxPlus") return MODEL_ISH3130;
            if (str == "TCS ISH3230 / Koch TCH50 GFA") return MODEL_ISH3230;
            if (str == "TCS ISH3030 / Koch TCH50 / Scantron Lux2") return MODEL_ISH3030;
            if (str == "TCS ISH1030 / Koch TTS25") return MODEL_ISH1030;
            if (str == "TCS IMM1000 / Koch TCH30") return MODEL_IMM1000;
            if (str == "TCS IMM1100 / Koch TCHE30") return MODEL_IMM1100;
            if (str == "TCS IMM1300 / Koch VTCH30") return MODEL_IMM1300;
            if (str == "TCS IMM1500") return MODEL_IMM1500;
            if (str == "TCS IMM1310 / Koch VTCHE30") return MODEL_IMM1310;
            if (str == "TCS IMM1110 / Koch TCHEE30") return MODEL_IMM1110;
            if (str == "TCS IVH3222 / Koch VTCH50 / Scantron VLux") return MODEL_IVH3222;
            if (str == "TCS IVH4222 / Koch VTCH50/2D") return MODEL_IVH4222;
            if (str == "TCS IVW2220 / Koch Sky") return MODEL_IVW2220;
            if (str == "TCS IVW2221 / Koch Sky R1.00") return MODEL_IVW2221;
            if (str == "TCS IVW3011 / Koch Skyline Plus") return MODEL_IVW3011;
            if (str == "TCS IVW3012 / Koch Skyline/Aldup") return MODEL_IVW3012;
            if (str == "TCS VMH / Koch VMH") return MODEL_VMH;
            if (str == "TCS VML / Koch VML") return MODEL_VML;
            if (str == "TCS VMF / Koch VMF") return MODEL_VMF;
            if (str == "Jung TKIS") return MODEL_TKIS;
            if (str == "Jung TKISV") return MODEL_TKISV;
            if (str == "TCS CAIXXXX / Koch CAIXXXX") return MODEL_CAIXXXX;
            if (str == "TCS CAI2000 / Koch Carus") return MODEL_CAI2000;
            if (str == "TCS ISW42X0") return MODEL_ISW42X0;
            if (str == "TCS IVW9010") return MODEL_IVW9010;
            if (str == "TCS IVW9011 / Koch VTP10") return MODEL_IVW9011;
            if (str == "TCS IVW9110") return MODEL_IVW9110;
            if (str == "TCS IVW9030 / Scantron SLIM50T") return MODEL_IVW9030;
            if (str == "TCS IVE70") return MODEL_IVE70;

            return MODEL_NONE;
        }

        Model identifier_string_to_model(const std::string& model_key, const uint8_t& hw_version, const uint32_t& fw_version)
        {
            if (model_key == "000") return MODEL_ISH3030;
            else if (model_key == "010") return MODEL_ISW3030;
            else if (model_key == "001") return MODEL_ISH3230;
            else if (model_key == "011") return MODEL_ISW3230;
            else if (model_key == "003") return MODEL_ISH3130;
            else if (model_key == "013") return MODEL_ISW3130;
            else if (model_key == "015") return MODEL_ISW3330;
            else if (model_key == "002") return MODEL_ISH3022;
            else if (model_key == "017") return MODEL_ISW3340;
            else if (model_key == "800") return MODEL_IVH3222;
            else if (model_key == "900") return MODEL_IVH4222;
            else if (model_key == "B00") return MODEL_IMM1000;
            else if (model_key == "200") return MODEL_ISW4100;
            else if (model_key == "201") return MODEL_IMM2100;
            else if (model_key == "020" || model_key == "021" || model_key == "022" || model_key == "023" ||
                    model_key == "024" || model_key == "025" || model_key == "026" || model_key == "027")
                return MODEL_ISW5010;

            else if (model_key == "030" || model_key == "031" || model_key == "032")
                return MODEL_IVW511X;

            else if (model_key == "03A" || model_key == "03B" || model_key == "03C" || model_key == "03D" || model_key == "03F")
                return MODEL_IVW521X;

            else if (model_key == "028" || model_key == "02B" || model_key == "02F")
                return MODEL_ISW5020;

            else if (model_key == "068" || model_key == "06F")
                return MODEL_ISW5030;

            else if (model_key == "068" || model_key == "06F")
                return MODEL_ISW5031;

            else if (model_key == "060") return MODEL_ISW5033;

            else if (model_key == "070" || model_key == "071" || model_key == "072" || model_key == "073" ||
                    model_key == "074" || model_key == "075" || model_key == "076" || model_key == "077")
                return MODEL_ISW6031;

            else if (model_key == "080" || model_key == "081" || model_key == "082" || model_key == "083" ||
                    model_key == "084" || model_key == "085" || model_key == "086" || model_key == "087")
                return MODEL_ISW7030;

            else if (model_key == "088" || model_key == "089" || model_key == "08A" || model_key == "08B" ||
                    model_key == "08C" || model_key == "08D" || model_key == "08E" || model_key == "08F")
                return MODEL_IVW7510;

            else if (model_key == "180" || model_key == "181" || model_key == "182" || model_key == "183" ||
                    model_key == "184" || model_key == "185" || model_key == "186" || model_key == "187")
                return MODEL_ISH7030;

            else if (model_key == "188" || model_key == "189" || model_key == "18A" || model_key == "18B" ||
                    model_key == "18C" || model_key == "18D" || model_key == "18E" || model_key == "18F")
                return MODEL_IVH7510;

            else if (model_key == "078" || model_key == "079" || model_key == "07A" || model_key == "07B" ||
                    model_key == "07C" || model_key == "07D" || model_key == "07E" || model_key == "07F")
                return MODEL_ISW6010;

            else if (model_key == "058" || model_key == "059" || model_key == "05A" || model_key == "05B" ||
                    model_key == "05C" || model_key == "05D" || model_key == "05E" || model_key == "05F")
                return MODEL_IVW6511;

            else if (model_key == "C70" || model_key == "C71" || model_key == "C72" || model_key == "C73" ||
                    model_key == "C74" || model_key == "C75" || model_key == "C76" || model_key == "C77")
                return MODEL_ISW7030;

            else if (model_key == "C90" || model_key == "C91" || model_key == "C92" || model_key == "C93" ||
                    model_key == "C94" || model_key == "C95" || model_key == "C96" || model_key == "C97")
                return MODEL_ISWM7000;

            else if (model_key == "C80" || model_key == "C81" || model_key == "C82" || model_key == "C83" ||
                    model_key == "C84" || model_key == "C85" || model_key == "C86" || model_key == "C87")
                return MODEL_IVWM7000;

            else if (model_key == "800" || model_key == "805") return MODEL_IVW2210;
            else if (model_key == "807") return MODEL_IVW2211;
            else if (model_key == "80C") return MODEL_IVW2212;
            else if (model_key == "810") return MODEL_IVW2220;
            else if (model_key == "815") return MODEL_IVW2221;
            else if (model_key == "820") return MODEL_IVW3011;
            else if (model_key == "830") return MODEL_IVW3012;

            else if (model_key == "C01") return MODEL_VMH;
            else if (model_key == "C00") return MODEL_VML;
            else if (model_key == "C02") return MODEL_VMF;
            else if (model_key == "400") return MODEL_ISW42X0;
            else if (model_key == "410") return MODEL_TKIS;
            else if (model_key == "420") return MODEL_TKISV;
            else if (model_key == "208") return MODEL_CAIXXXX;
            else if (model_key == "809") return MODEL_CAI2000;
            else if (model_key == "280") {
                if(fw_version >= 512) return MODEL_VTC42V2;
                else return MODEL_VTC40;
            }
            else if (model_key == "281") {
                if(fw_version >= 512) return MODEL_TC40V2;
                else return MODEL_TC40;
            }
        
            else if (model_key == "194") return MODEL_IVW9030;
            else if (model_key == "1E8") return MODEL_IVW9010;
            else if (model_key == "1EA") return MODEL_IVW9110;
            else if (model_key == "1E9") return MODEL_IVW9011;
            else if (model_key == "1B3" || model_key == "1B4" || model_key == "1B5")
                return MODEL_IVE70;

            return MODEL_NONE;
        }

        const char* model_to_string(Model model)
        {
            switch (model)
            {
                case MODEL_ISW3030: return "TCS ISW3030 / Koch TC50 / Scantron Stilux";
                case MODEL_ISW3130: return "TCS ISW3130 / Koch TC50P";
                case MODEL_ISW3230: return "TCS ISW3230 / Koch TC50 GFA";
                case MODEL_ISW3330: return "TCS ISW3330 / Koch TC50 BW";
                case MODEL_ISW3340: return "TCS ISW3340";
                case MODEL_ISW5010: return "TCS ISW5010 / Koch TC60";
                case MODEL_ISW5020: return "TCS ISW5020";
                case MODEL_ISW5030: return "TCS ISW5030";
                case MODEL_ISW5031: return "TCS ISW5031";
                case MODEL_ISW5033: return "TCS ISW5033";
                case MODEL_IVW511X: return "TCS IVW511x / Koch VTC60 / Scantron VIVO";
                case MODEL_IVW521X: return "TCS IVW521x / Koch VTC60/2D";
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
                case MODEL_IVW2212: return "TCS IVW2212 / Koch Ecoos / Scantron SLIM60T";
                case MODEL_VTC42V2: return "TCS VTC42V2";
                case MODEL_TC40V2: return "TCS TC40V2";
                case MODEL_VTC40: return "TCS VTC40";
                case MODEL_TC40: return "TCS TC40";
                case MODEL_TC2000: return "TCS TC2000";
                case MODEL_TC20P: return "TCS TC20P";
                case MODEL_TC20F: return "TCS TC20F";
                case MODEL_ISH3022: return "TCS ISH3022";
                case MODEL_ISH3130: return "TCS ISH3130 / Koch TCH50P / Scantron LuxPlus";
                case MODEL_ISH3230: return "TCS ISH3230 / Koch TCH50 GFA";
                case MODEL_ISH3030: return "TCS ISH3030 / Koch TCH50 / Scantron Lux2";
                case MODEL_ISH1030: return "TCS ISH1030 / Koch TTS25";
                case MODEL_IMM1000: return "TCS IMM1000 / Koch TCH30";
                case MODEL_IMM1100: return "TCS IMM1100 / Koch TCHE30";
                case MODEL_IMM1300: return "TCS IMM1300 / Koch VTCH30";
                case MODEL_IMM1500: return "TCS IMM1500";
                case MODEL_IMM1310: return "TCS IMM1310 / Koch VTCHE30";
                case MODEL_IMM1110: return "TCS IMM1110 / Koch TCHEE30";
                case MODEL_IVH3222: return "TCS IVH3222 / Koch VTCH50 / Scantron VLux";
                case MODEL_IVH4222: return "TCS IVH4222 / Koch VTCH50/2D";
                case MODEL_IVW2220: return "TCS IVW2220 / Koch Sky";
                case MODEL_IVW2221: return "TCS IVW2221 / Koch Sky R1.00";
                case MODEL_IVW3011: return "TCS IVW3011 / Koch Skyline Plus";
                case MODEL_IVW3012: return "TCS IVW3012 / Koch Skyline/Aldup";
                case MODEL_VMH: return "TCS VMH / Koch VMH";
                case MODEL_VML: return "TCS VML / Koch VML";
                case MODEL_VMF: return "TCS VMF / Koch VMF";
                case MODEL_TKIS: return "Jung TKIS";
                case MODEL_TKISV: return "Jung TKISV";
                case MODEL_CAIXXXX: return "TCS CAIXXXX / Koch CAIXXXX";
                case MODEL_CAI2000: return "TCS CAI2000 / Koch Carus";
                case MODEL_ISW42X0: return "TCS ISW42X0";
                case MODEL_IVW9010: return "TCS IVW9010";
                case MODEL_IVW9011: return "TCS IVW9011 / Koch VTP10";
                case MODEL_IVW9110: return "TCS IVW9110";
                case MODEL_IVW9030: return "TCS IVW9030 / Scantron SLIM50T";
                case MODEL_IVE70: return "TCS IVE70";
                default: return "None";
            }
        }

        ModelData getModelData(Model model)
        {
            ModelData modelData;
            modelData.model = model;

            switch (model) {
                // Category 1
                case MODEL_ISW3030: /* TC50 */
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW3130: /* TC50P */
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW3230: /* TC50 GFA */
                    modelData.category = 1;
                    modelData.memory_size = 40;
                    break;
                case MODEL_ISW3330: /* TC50 BW */
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_ISW3340:
                    modelData.category = 1;
                    modelData.memory_size = 128;
                    break;
                case MODEL_ISW5010: /* TC60 */
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW5020:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW5030:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW5031:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW5033:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVW511X: /* VTC60 */
                    modelData.category = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_IVW521X: /* VTC60/2D */
                    modelData.category = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_ISW6031:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW7030: /* TC70 */
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVW7510: /* VTC70 */
                    modelData.category = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_ISH7030: /* TCH70 */
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVH7510: /* VTCH70 */
                    modelData.category = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_ISW6010:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVW6511:
                    modelData.category = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_ISWM7000:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVWM7000:
                    modelData.category = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_ISW4100: /* TC31 */
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IMM2100: /* TCE31 */
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVW2210: /* Ecoos */
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IVW2211: /* Ecoos */
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IVW2212: /* Ecoos */
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_VTC42V2:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_TC40V2:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_VTC40:
                    modelData.category = 1;
                    modelData.memory_size = 40;
                    break;
                case MODEL_TC40:
                    modelData.category = 1;
                    modelData.memory_size = 40;
                    break;
                case MODEL_TC2000:
                    modelData.category = 1;
                    modelData.memory_size = 16;
                    break;
                case MODEL_TC20P:
                    modelData.category = 1;
                    modelData.memory_size = 16;
                    break;
                case MODEL_TC20F:
                    modelData.category = 1;
                    modelData.memory_size = 16;
                    break;
                case MODEL_IVW2220:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IVW2221:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IVW3011:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IVW3012:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_TKIS:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_TKISV:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_CAI2000:
                    modelData.category = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_CAIXXXX:
                    modelData.category = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISW42X0:
                    modelData.category = 1;
                    modelData.memory_size = 40;
                    break;

                // Category 0
                case MODEL_ISH3022: /* TCH50P */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISH3130: /* TCH50P */
                    modelData.category = 0;
                    modelData.memory_size = 40;
                    break;
                case MODEL_ISH3230: /* TCH50 GFA */
                    modelData.category = 0;
                    modelData.memory_size = 40;
                    break;
                case MODEL_ISH3030: /* TCH50 */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_ISH1030: /* TTS25 */
                    modelData.category = 0;
                    modelData.memory_size = 16;
                    break;
                case MODEL_IMM1000: /* TCH30 */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IMM1100: /* TCHE30 */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IMM1300: /* VTCH30 */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IMM1500:
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IMM1310: /* VTCHE30 */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IMM1110: /* TCHEE30 */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVH3222: /* VTCH50 */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IVH4222: /* VTCH50/2D */
                    modelData.category = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_VMH:
                    modelData.category = 0;
                    modelData.memory_size = 24;
                    break;
                case MODEL_VML:
                    modelData.category = 0;
                    modelData.memory_size = 24;
                    break;
                case MODEL_VMF:
                    modelData.category = 0;
                    modelData.memory_size = 24;
                    break;

                case MODEL_IVW9010:
                case MODEL_IVW9011:
                case MODEL_IVW9110:
                case MODEL_IVW9030:
                case MODEL_IVE70:
                    modelData.category = 0;
                    modelData.memory_size = 0;
                    break;

                default:
                    break;
            }

            return modelData;
        }

        SettingCellData getSettingCellData(SettingType setting, Model model)
        {
            SettingCellData data;

            switch (model) {
                case MODEL_ISH3030:
                case MODEL_ISH3230:
                case MODEL_ISW3030:
                case MODEL_ISW3230:
                case MODEL_ISW3340:
                case MODEL_ISW3130:
                case MODEL_ISW3330:
                case MODEL_IVH4222:
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;

                        case SETTING_VOLUME_HANDSET_DOOR_CALL:
                            data.index = 21;
                            data.left_nibble = false;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_ISH1030:
                case MODEL_IVH3222:
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_IVW511X:
                case MODEL_IVW521X:
                    // TASTA Video
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL:
                            data.index = 12;
                            data.left_nibble = true;
                            break;

                        // Values: 0,2,4,6
                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_DOOR_CALL:
                            data.index = 21;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_INTERNAL_CALL:
                            data.index = 21;
                            data.left_nibble = true;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_ISW5010:
                case MODEL_ISW5020:
                case MODEL_ISW5030:
                case MODEL_ISW5031:
                case MODEL_ISW5033:
                    // TASTA Audio
                    switch (setting)
                    {
                        case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL:
                            data.index = 12;
                            data.left_nibble = true;
                            break;

                        // Values: 0,2,4,6
                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_DOOR_CALL:
                            data.index = 21;
                            data.left_nibble = false;
                            break;

                        // Values: 0,2,4,7
                        case SETTING_VOLUME_HANDSET_INTERNAL_CALL:
                            data.index = 21;
                            data.left_nibble = true;
                            break;

                        default: break;
                    }
                    break;

                case MODEL_IVW2210:
                case MODEL_IVW2211:
                case MODEL_IVW2212:
                    // ECOOS
                    switch (setting)
                    {
                        /*case SETTING_RINGTONE_ENTRANCE_DOOR_CALL:
                            data.index = 3;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_INTERNAL_CALL:
                            data.index = 6;
                            data.left_nibble = true;
                            break;

                        case SETTING_RINGTONE_FLOOR_CALL:
                            data.index = 9;
                            data.left_nibble = true;
                            break;

                        case SETTING_VOLUME_RINGTONE:
                            data.index = 20;
                            data.left_nibble = false;
                            break;*/

                        default: break;
                    }
                    break;
                default: break;
            }

            return data;
        }

        uint8_t ringtone_to_int(const std::string& str)
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