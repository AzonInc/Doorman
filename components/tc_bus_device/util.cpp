#include "util.h"
#include "esphome.h"
#include "esphome/core/helpers.h"
#include <algorithm>
#include <cctype>

namespace esphome
{
    namespace tc_bus
    {
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

        Model string_to_model(const std::string& str)
        {
            if (str == "TCS ISW3030 / Koch TC50 / Scantron Stilux") return MODEL_IS_ISW3030;
            if (str == "TCS ISW3130 / Koch TC50P") return MODEL_IS_ISW3130;
            if (str == "TCS ISW3230 / Koch TC50 GFA") return MODEL_IS_ISW3230;
            if (str == "TCS ISW3330 / Koch TC50 BW") return MODEL_IS_ISW3330;
            if (str == "TCS ISW3340") return MODEL_IS_ISW3340;
            if (str == "TCS ISW5010 / Koch TC60") return MODEL_IS_ISW5010;
            if (str == "TCS ISW5020") return MODEL_IS_ISW5020;
            if (str == "TCS ISW5030") return MODEL_IS_ISW5030;
            if (str == "TCS ISW5031") return MODEL_IS_ISW5031;
            if (str == "TCS ISW5033") return MODEL_IS_ISW5033;
            if (str == "TCS IVW511x / Koch VTC60 / Scantron VIVO") return MODEL_IS_IVW511X;
            if (str == "TCS IVW521x / Koch VTC60/2D") return MODEL_IS_IVW521X;
            if (str == "TCS ISW6031") return MODEL_IS_ISW6031;
            if (str == "TCS ISW7030 / Koch TC70") return MODEL_IS_ISW7030;
            if (str == "TCS IVW7510 / Koch VTC70") return MODEL_IS_IVW7510;
            if (str == "TCS ISH7030 / Koch TCH70") return MODEL_IS_ISH7030;
            if (str == "TCS IVH7510 / Koch VTCH70") return MODEL_IS_IVH7510;
            if (str == "TCS ISW6010") return MODEL_IS_ISW6010;
            if (str == "TCS IVW6511") return MODEL_IS_IVW6511;
            if (str == "TCS ISWM7000") return MODEL_IS_ISWM7000;
            if (str == "TCS IVWM7000") return MODEL_IS_IVWM7000;
            if (str == "TCS ISW4100 / Koch TC31") return MODEL_IS_ISW4100;
            if (str == "TCS IMM2100 / Koch TCE31") return MODEL_IS_IMM2100;
            if (str == "TCS IVW2210 / Koch Ecoos") return MODEL_IS_IVW2210;
            if (str == "TCS IVW2211 / Koch Ecoos") return MODEL_IS_IVW2211;
            if (str == "TCS IVW2212 / Koch Ecoos / Scantron SLIM60T") return MODEL_IS_IVW2212;
            if (str == "TCS VTC42V2") return MODEL_IS_VTC42V2;
            if (str == "TCS TC40V2") return MODEL_IS_TC40V2;
            if (str == "TCS VTC40") return MODEL_IS_VTC40;
            if (str == "TCS TC40") return MODEL_IS_TC40;
            if (str == "TCS TC2000") return MODEL_IS_TC2000;
            if (str == "TCS TC20P") return MODEL_IS_TC20P;
            if (str == "TCS TC20F") return MODEL_IS_TC20F;
            if (str == "TCS ISH3022 / Koch TCH50P") return MODEL_IS_ISH3022;
            if (str == "TCS ISH3130 / Koch TCH50P / Scantron LuxPlus") return MODEL_IS_ISH3130;
            if (str == "TCS ISH3230 / Koch TCH50 GFA") return MODEL_IS_ISH3230;
            if (str == "TCS ISH3030 / Koch TCH50 / Scantron Lux2") return MODEL_IS_ISH3030;
            if (str == "TCS ISH1030 / Koch TTS25") return MODEL_IS_ISH1030;
            if (str == "TCS TTC-XX") return MODEL_IS_TTCXX;
            if (str == "TCS TTS-XX") return MODEL_IS_TTSXX;
            if (str == "TCS IMM1000 / Koch TCH30") return MODEL_IS_IMM1000;
            if (str == "TCS IMM1100 / Koch TCHE30") return MODEL_IS_IMM1100;
            if (str == "TCS IMM1300 / Koch VTCH30") return MODEL_IS_IMM1300;
            if (str == "TCS IMM1500") return MODEL_IS_IMM1500;
            if (str == "TCS IMM1310 / Koch VTCHE30") return MODEL_IS_IMM1310;
            if (str == "TCS IMM1110 / Koch TCHEE30") return MODEL_IS_IMM1110;
            if (str == "TCS IVH3222 / Koch VTCH50 / Scantron VLux") return MODEL_IS_IVH3222;
            if (str == "TCS IVH4222 / Koch VTCH50/2D") return MODEL_IS_IVH4222;
            if (str == "TCS IVW2220 / Koch Sky") return MODEL_IS_IVW2220;
            if (str == "TCS IVW2221 / Koch Sky R1.00") return MODEL_IS_IVW2221;
            if (str == "TCS IVW3011 / Koch Skyline Plus") return MODEL_IS_IVW3011;
            if (str == "TCS IVW3012 / Koch Skyline/Aldup") return MODEL_IS_IVW3012;
            if (str == "TCS VMH / Koch VMH") return MODEL_IS_VMH;
            if (str == "TCS VML / Koch VML") return MODEL_IS_VML;
            if (str == "TCS VMF / Koch VMF") return MODEL_IS_VMF;
            if (str == "Jung TKIS") return MODEL_IS_TKIS;
            if (str == "Jung TKISV") return MODEL_IS_TKISV;
            if (str == "TCS CAIXXXX / Koch CAIXXXX") return MODEL_IS_CAIXXXX;
            if (str == "TCS CAI2000 / Koch Carus") return MODEL_IS_CAI2000;
            if (str == "TCS ISW42X0") return MODEL_IS_ISW42X0;
            if (str == "TCS IVW9010") return MODEL_IS_IVW9010;
            if (str == "TCS IVW9011 / Koch VTP10") return MODEL_IS_IVW9011;
            if (str == "TCS IVW9110") return MODEL_IS_IVW9110;
            if (str == "TCS IVW9030 / Scantron SLIM50T") return MODEL_IS_IVW9030;
            if (str == "TCS IVE70") return MODEL_IS_IVE70;
            if (str == "DEBUG IS0") return MODEL_IS_DEBUG_0;
            if (str == "DEBUG IS1") return MODEL_IS_DEBUG_1;

            if (str == "DEBUG AS") return MODEL_AS_DEBUG;

            if (str == "TCS BVS20") return MODEL_CTRL_BVS20;
            if (str == "TCS BVS30") return MODEL_CTRL_BVS30;
            if (str == "TCS NBV3210") return MODEL_CTRL_NBV3210;
            if (str == "TCS VBVS30") return MODEL_CTRL_VBVS30;
            if (str == "TCS NBV2600") return MODEL_CTRL_NBV2600;
            if (str == "DEBUG CONTROLLER") return MODEL_CTRL_DEBUG;
            
            if (str == "TCS TRE2") return MODEL_EXT_TRE2;
            if (str == "DEBUG EXTENSION") return MODEL_EXT_DEBUG;

            return MODEL_NONE;
        }

        Model identifier_string_to_model(const uint8_t& device_group, const std::string& model_key, const uint8_t& hw_version, const uint32_t& fw_version)
        {
            if(device_group == 0 || device_group == 1)
            {
                if (model_key == "000") return MODEL_IS_ISH3030;
                else if (model_key == "010") return MODEL_IS_ISW3030;
                else if (model_key == "001") return MODEL_IS_ISH3230;
                else if (model_key == "011") return MODEL_IS_ISW3230;
                else if (model_key == "003") return MODEL_IS_ISH3130;
                else if (model_key == "013") return MODEL_IS_ISW3130;
                else if (model_key == "015") return MODEL_IS_ISW3330;
                else if (model_key == "002") return MODEL_IS_ISH3022;
                else if (model_key == "017") return MODEL_IS_ISW3340;
                else if (model_key == "800") return MODEL_IS_IVH3222;
                else if (model_key == "900") return MODEL_IS_IVH4222;
                else if (model_key == "B00") return MODEL_IS_IMM1000;
                else if (model_key == "200") return MODEL_IS_ISW4100;
                else if (model_key == "201") return MODEL_IS_IMM2100;
                else if (model_key == "020" || model_key == "021" || model_key == "022" || model_key == "023" ||
                        model_key == "024" || model_key == "025" || model_key == "026" || model_key == "027")
                    return MODEL_IS_ISW5010;

                else if (model_key == "030" || model_key == "031" || model_key == "032")
                    return MODEL_IS_IVW511X;

                else if (model_key == "03A" || model_key == "03B" || model_key == "03C" || model_key == "03D" || model_key == "03F")
                    return MODEL_IS_IVW521X;

                else if (model_key == "028" || model_key == "02B" || model_key == "02F")
                    return MODEL_IS_ISW5020;

                else if (model_key == "068" || model_key == "06F")
                    return MODEL_IS_ISW5030;

                else if (model_key == "068" || model_key == "06F")
                    return MODEL_IS_ISW5031;

                else if (model_key == "060") return MODEL_IS_ISW5033;

                else if (model_key == "070" || model_key == "071" || model_key == "072" || model_key == "073" ||
                        model_key == "074" || model_key == "075" || model_key == "076" || model_key == "077")
                    return MODEL_IS_ISW6031;

                else if (model_key == "080" || model_key == "081" || model_key == "082" || model_key == "083" ||
                        model_key == "084" || model_key == "085" || model_key == "086" || model_key == "087")
                    return MODEL_IS_ISW7030;

                else if (model_key == "088" || model_key == "089" || model_key == "08A" || model_key == "08B" ||
                        model_key == "08C" || model_key == "08D" || model_key == "08E" || model_key == "08F")
                    return MODEL_IS_IVW7510;

                else if (model_key == "180" || model_key == "181" || model_key == "182" || model_key == "183" ||
                        model_key == "184" || model_key == "185" || model_key == "186" || model_key == "187")
                    return MODEL_IS_ISH7030;

                else if (model_key == "188" || model_key == "189" || model_key == "18A" || model_key == "18B" ||
                        model_key == "18C" || model_key == "18D" || model_key == "18E" || model_key == "18F")
                    return MODEL_IS_IVH7510;

                else if (model_key == "078" || model_key == "079" || model_key == "07A" || model_key == "07B" ||
                        model_key == "07C" || model_key == "07D" || model_key == "07E" || model_key == "07F")
                    return MODEL_IS_ISW6010;

                else if (model_key == "058" || model_key == "059" || model_key == "05A" || model_key == "05B" ||
                        model_key == "05C" || model_key == "05D" || model_key == "05E" || model_key == "05F")
                    return MODEL_IS_IVW6511;

                else if (model_key == "C70" || model_key == "C71" || model_key == "C72" || model_key == "C73" ||
                        model_key == "C74" || model_key == "C75" || model_key == "C76" || model_key == "C77")
                    return MODEL_IS_ISW7030;

                else if (model_key == "C90" || model_key == "C91" || model_key == "C92" || model_key == "C93" ||
                        model_key == "C94" || model_key == "C95" || model_key == "C96" || model_key == "C97")
                    return MODEL_IS_ISWM7000;

                else if (model_key == "C80" || model_key == "C81" || model_key == "C82" || model_key == "C83" ||
                        model_key == "C84" || model_key == "C85" || model_key == "C86" || model_key == "C87")
                    return MODEL_IS_IVWM7000;

                else if (model_key == "800" || model_key == "805") return MODEL_IS_IVW2210;
                else if (model_key == "807") return MODEL_IS_IVW2211;
                else if (model_key == "80C") return MODEL_IS_IVW2212;
                else if (model_key == "810") return MODEL_IS_IVW2220;
                else if (model_key == "815") return MODEL_IS_IVW2221;
                else if (model_key == "820") return MODEL_IS_IVW3011;
                else if (model_key == "830") return MODEL_IS_IVW3012;

                else if (model_key == "C01") return MODEL_IS_VMH;
                else if (model_key == "C00") return MODEL_IS_VML;
                else if (model_key == "C02") return MODEL_IS_VMF;
                else if (model_key == "400") return MODEL_IS_ISW42X0;
                else if (model_key == "410") return MODEL_IS_TKIS;
                else if (model_key == "420") return MODEL_IS_TKISV;
                else if (model_key == "208") return MODEL_IS_CAIXXXX;
                else if (model_key == "809") return MODEL_IS_CAI2000;
                else if (model_key == "280") {
                    if(fw_version >= 512) return MODEL_IS_VTC42V2;
                    else return MODEL_IS_VTC40;
                }
                else if (model_key == "281") {
                    if(fw_version >= 512) return MODEL_IS_TC40V2;
                    else return MODEL_IS_TC40;
                }
            
                else if (model_key == "194") return MODEL_IS_IVW9030;
                else if (model_key == "1E8") return MODEL_IS_IVW9010;
                else if (model_key == "1EA") return MODEL_IS_IVW9110;
                else if (model_key == "1E9") return MODEL_IS_IVW9011;
                else if (model_key == "1B3" || model_key == "1B4" || model_key == "1B5")
                    return MODEL_IS_IVE70;
            }
            else if(device_group == 4)
            {
                if (model_key == "008") return MODEL_CTRL_BVS30;
                else if (model_key == "010") return MODEL_CTRL_NBV3210;
                else if (model_key == "009") return MODEL_CTRL_VBVS30;
                else if (model_key == "D2D") return MODEL_CTRL_NBV2600;
            }
            else
            {
                // Other device groups
                // Not implemented
            }

            return MODEL_NONE;
        }

        const char* model_to_string(Model model)
        {
            switch (model)
            {
                case MODEL_IS_ISW3030: return "TCS ISW3030 / Koch TC50 / Scantron Stilux";
                case MODEL_IS_ISW3130: return "TCS ISW3130 / Koch TC50P";
                case MODEL_IS_ISW3230: return "TCS ISW3230 / Koch TC50 GFA";
                case MODEL_IS_ISW3330: return "TCS ISW3330 / Koch TC50 BW";
                case MODEL_IS_ISW3340: return "TCS ISW3340";
                case MODEL_IS_ISW5010: return "TCS ISW5010 / Koch TC60";
                case MODEL_IS_ISW5020: return "TCS ISW5020";
                case MODEL_IS_ISW5030: return "TCS ISW5030";
                case MODEL_IS_ISW5031: return "TCS ISW5031";
                case MODEL_IS_ISW5033: return "TCS ISW5033";
                case MODEL_IS_IVW511X: return "TCS IVW511x / Koch VTC60 / Scantron VIVO";
                case MODEL_IS_IVW521X: return "TCS IVW521x / Koch VTC60/2D";
                case MODEL_IS_ISW6031: return "TCS ISW6031";
                case MODEL_IS_ISW7030: return "TCS ISW7030 / Koch TC70";
                case MODEL_IS_IVW7510: return "TCS IVW7510 / Koch VTC70";
                case MODEL_IS_ISH7030: return "TCS ISH7030 / Koch TCH70";
                case MODEL_IS_IVH7510: return "TCS IVH7510 / Koch VTCH70";
                case MODEL_IS_ISW6010: return "TCS ISW6010";
                case MODEL_IS_IVW6511: return "TCS IVW6511";
                case MODEL_IS_ISWM7000: return "TCS ISWM7000";
                case MODEL_IS_IVWM7000: return "TCS IVWM7000";
                case MODEL_IS_ISW4100: return "TCS ISW4100 / Koch TC31";
                case MODEL_IS_IMM2100: return "TCS IMM2100 / Koch TCE31";
                case MODEL_IS_IVW2210: return "TCS IVW2210 / Koch Ecoos";
                case MODEL_IS_IVW2211: return "TCS IVW2211 / Koch Ecoos";
                case MODEL_IS_IVW2212: return "TCS IVW2212 / Koch Ecoos / Scantron SLIM60T";
                case MODEL_IS_VTC42V2: return "TCS VTC42V2";
                case MODEL_IS_TC40V2: return "TCS TC40V2";
                case MODEL_IS_VTC40: return "TCS VTC40";
                case MODEL_IS_TC40: return "TCS TC40";
                case MODEL_IS_TC2000: return "TCS TC2000";
                case MODEL_IS_TC20P: return "TCS TC20P";
                case MODEL_IS_TC20F: return "TCS TC20F";
                case MODEL_IS_ISH3022: return "TCS ISH3022";
                case MODEL_IS_ISH3130: return "TCS ISH3130 / Koch TCH50P / Scantron LuxPlus";
                case MODEL_IS_ISH3230: return "TCS ISH3230 / Koch TCH50 GFA";
                case MODEL_IS_ISH3030: return "TCS ISH3030 / Koch TCH50 / Scantron Lux2";
                case MODEL_IS_ISH1030: return "TCS ISH1030 / Koch TTS25";
                case MODEL_IS_TTCXX: return "TCS TTC-XX";
                case MODEL_IS_TTSXX: return "TCS TTS-XX";
                case MODEL_IS_IMM1000: return "TCS IMM1000 / Koch TCH30";
                case MODEL_IS_IMM1100: return "TCS IMM1100 / Koch TCHE30";
                case MODEL_IS_IMM1300: return "TCS IMM1300 / Koch VTCH30";
                case MODEL_IS_IMM1500: return "TCS IMM1500";
                case MODEL_IS_IMM1310: return "TCS IMM1310 / Koch VTCHE30";
                case MODEL_IS_IMM1110: return "TCS IMM1110 / Koch TCHEE30";
                case MODEL_IS_IVH3222: return "TCS IVH3222 / Koch VTCH50 / Scantron VLux";
                case MODEL_IS_IVH4222: return "TCS IVH4222 / Koch VTCH50/2D";
                case MODEL_IS_IVW2220: return "TCS IVW2220 / Koch Sky";
                case MODEL_IS_IVW2221: return "TCS IVW2221 / Koch Sky R1.00";
                case MODEL_IS_IVW3011: return "TCS IVW3011 / Koch Skyline Plus";
                case MODEL_IS_IVW3012: return "TCS IVW3012 / Koch Skyline/Aldup";
                case MODEL_IS_VMH: return "TCS VMH / Koch VMH";
                case MODEL_IS_VML: return "TCS VML / Koch VML";
                case MODEL_IS_VMF: return "TCS VMF / Koch VMF";
                case MODEL_IS_TKIS: return "Jung TKIS";
                case MODEL_IS_TKISV: return "Jung TKISV";
                case MODEL_IS_CAIXXXX: return "TCS CAIXXXX / Koch CAIXXXX";
                case MODEL_IS_CAI2000: return "TCS CAI2000 / Koch Carus";
                case MODEL_IS_ISW42X0: return "TCS ISW42X0";
                case MODEL_IS_IVW9010: return "TCS IVW9010";
                case MODEL_IS_IVW9011: return "TCS IVW9011 / Koch VTP10";
                case MODEL_IS_IVW9110: return "TCS IVW9110";
                case MODEL_IS_IVW9030: return "TCS IVW9030 / Scantron SLIM50T";
                case MODEL_IS_IVE70: return "TCS IVE70";
                case MODEL_IS_DEBUG_0: return "DEBUG IS0";
                case MODEL_IS_DEBUG_1: return "DEBUG IS1";

                case MODEL_AS_DEBUG: return "DEBUG AS";

                case MODEL_CTRL_BVS20: return "TCS BVS20";
                case MODEL_CTRL_BVS30: return "TCS BVS30";
                case MODEL_CTRL_NBV3210: return "TCS NBV3210";
                case MODEL_CTRL_VBVS30: return "TCS VBVS30";
                case MODEL_CTRL_NBV2600: return "TCS NBV2600";
                case MODEL_CTRL_DEBUG: return "DEBUG CONTROLLER";

                case MODEL_EXT_TRE2: return "TCS TRE2";
                case MODEL_EXT_DEBUG: return "DEBUG EXTENSION";

                default: return "None";
            }
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
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_ISW3130: /* TC50P */
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_ISW3230: /* TC50 GFA */
                    modelData.device_group = 1;
                    modelData.memory_size = 40;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_ISW3330: /* TC50 BW */
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_ISW3340:
                    modelData.device_group = 1;
                    modelData.memory_size = 128;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_ISW5010: /* TC60 */
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_INTERNAL_CALL;
                    break;
                case MODEL_IS_ISW5020:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_INTERNAL_CALL;
                    break;
                case MODEL_IS_ISW5030:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_INTERNAL_CALL;
                    break;
                case MODEL_IS_ISW5031:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_INTERNAL_CALL;
                    break;
                case MODEL_IS_ISW5033:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_INTERNAL_CALL;
                    break;
                case MODEL_IS_IVW511X: /* VTC60 */
                    modelData.device_group = 1;
                    modelData.memory_size = 48;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_INTERNAL_CALL;
                    break;
                case MODEL_IS_IVW521X: /* VTC60/2D */
                    modelData.device_group = 1;
                    modelData.memory_size = 48;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_INTERNAL_CALL;
                    break;
                case MODEL_IS_ISW6031:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_ISW7030: /* TC70 */
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IVW7510: /* VTC70 */
                    modelData.device_group = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_IS_ISH7030: /* TCH70 */
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IVH7510: /* VTCH70 */
                    modelData.device_group = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_IS_ISW6010:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IVW6511:
                    modelData.device_group = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_IS_ISWM7000:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IVWM7000:
                    modelData.device_group = 1;
                    modelData.memory_size = 48;
                    break;
                case MODEL_IS_ISW4100: /* TC31 */
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IMM2100: /* TCE31 */
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IVW2210: /* Ecoos */
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    // Supported? Not documented
                    //modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_IVW2211: /* Ecoos */
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    // Supported? Not documented
                    //modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_IVW2212: /* Ecoos */
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    // Supported? Not documented
                    //modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_VTC42V2:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_TC40V2:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_VTC40:
                    modelData.device_group = 1;
                    modelData.memory_size = 40;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_TC40:
                    modelData.device_group = 1;
                    modelData.memory_size = 40;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_TC2000:
                    modelData.device_group = 1;
                    modelData.memory_size = 16;
                    break;
                case MODEL_IS_TC20P:
                    modelData.device_group = 1;
                    modelData.memory_size = 16;
                    break;
                case MODEL_IS_TC20F:
                    modelData.device_group = 1;
                    modelData.memory_size = 16;
                    break;
                case MODEL_IS_IVW2220:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IS_IVW2221:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IS_IVW3011:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IS_IVW3012:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IS_TKIS:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IS_TKISV:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IS_CAI2000:
                    modelData.device_group = 1;
                    modelData.memory_size = 64;
                    break;
                case MODEL_IS_CAIXXXX:
                    modelData.device_group = 1;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_ISW42X0:
                    modelData.device_group = 1;
                    modelData.memory_size = 40;
                    break;

                // Group 0
                case MODEL_IS_ISH3022: /* TCH50P */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_ISH3130: /* TCH50P */
                    modelData.device_group = 0;
                    modelData.memory_size = 40;
                    break;
                case MODEL_IS_ISH3230: /* TCH50 GFA */
                    modelData.device_group = 0;
                    modelData.memory_size = 40;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_ISH3030: /* TCH50 */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_ISH1030: /* TTS25 */
                    modelData.device_group = 0;
                    modelData.memory_size = 16;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    break;
                case MODEL_IS_TTCXX:
                    modelData.device_group = 0;
                    modelData.memory_size = 16;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    break;
                case MODEL_IS_TTSXX:
                    modelData.device_group = 0;
                    modelData.memory_size = 16;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    break;
                case MODEL_IS_IMM1000: /* TCH30 */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IMM1100: /* TCHE30 */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IMM1300: /* VTCH30 */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IMM1500:
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IMM1310: /* VTCHE30 */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IMM1110: /* TCHEE30 */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    break;
                case MODEL_IS_IVH3222: /* VTCH50 */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    break;
                case MODEL_IS_IVH4222: /* VTCH50/2D */
                    modelData.device_group = 0;
                    modelData.memory_size = 32;
                    modelData.capabilities |= CAP_RINGTONE_ENTRANCE_DOOR_CALL;
                    modelData.capabilities |= CAP_RINGTONE_INTERNAL_CALL;
                    modelData.capabilities |= CAP_RINGTONE_FLOOR_CALL;
                    modelData.capabilities |= CAP_VOLUME_RINGTONE;
                    modelData.capabilities |= CAP_VOLUME_HANDSET_DOOR_CALL;
                    break;
                case MODEL_IS_VMH:
                    modelData.device_group = 0;
                    modelData.memory_size = 24;
                    break;
                case MODEL_IS_VML:
                    modelData.device_group = 0;
                    modelData.memory_size = 24;
                    break;
                case MODEL_IS_VMF:
                    modelData.device_group = 0;
                    modelData.memory_size = 24;
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

                // Group 3
                // Not implemented

                // Group 4
                case MODEL_CTRL_BVS20:
                case MODEL_CTRL_BVS30:
                case MODEL_CTRL_NBV3210:
                case MODEL_CTRL_VBVS30:
                case MODEL_CTRL_NBV2600:
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
                    modelData.device_group = 6;
                    modelData.memory_size = 0;
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
            SettingCellData data{};
            ModelData model_data = getModelData(model);

            // Only set data if model supports the capability
            if (setting == SETTING_RINGTONE_ENTRANCE_DOOR_CALL && (model_data.capabilities & CAP_RINGTONE_ENTRANCE_DOOR_CALL))
            {
                data.index = 3;
                data.left_nibble = true;
            }
            else if (setting == SETTING_RINGTONE_INTERNAL_CALL && (model_data.capabilities & CAP_RINGTONE_INTERNAL_CALL))
            {
                data.index = 6;
                data.left_nibble = true;
            }
            else if (setting == SETTING_RINGTONE_FLOOR_CALL && (model_data.capabilities & CAP_RINGTONE_FLOOR_CALL))
            {
                data.index = 9;
                data.left_nibble = true;
            }
            else if (setting == SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL && (model_data.capabilities & CAP_RINGTONE_SECOND_ENTRANCE_DOOR_CALL))
            {
                data.index = 12;
                data.left_nibble = true;
            }
            else if (setting == SETTING_VOLUME_RINGTONE && (model_data.capabilities & CAP_VOLUME_RINGTONE))
            {
                data.index = 20;
                data.left_nibble = false;
            }
            else if (setting == SETTING_VOLUME_HANDSET_DOOR_CALL && (model_data.capabilities & CAP_VOLUME_HANDSET_DOOR_CALL))
            {
                data.index = 21;
                data.left_nibble = false;
            }
            else if (setting == SETTING_VOLUME_HANDSET_INTERNAL_CALL && (model_data.capabilities & CAP_VOLUME_HANDSET_INTERNAL_CALL))
            {
                data.index = 21;
                data.left_nibble = true;
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