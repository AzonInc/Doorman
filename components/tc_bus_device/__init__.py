import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_TYPE, CONF_VALUE
from .. import tc_bus

AUTO_LOAD = ["tc_bus"]

CODEOWNERS = ["@azoninc"]

DEPENDENCIES = ["tc_bus"]
MULTI_CONF = False

tc_bus_ns = tc_bus.tc_bus_ns
TCBusComponent = tc_bus.TCBusComponent

TCBusDeviceComponent = tc_bus_ns.class_('TCBusDeviceComponent', cg.Component)

TCBusDeviceSendAction = tc_bus_ns.class_(
    "TCBusDeviceSendAction", automation.Action
)

TCBusDeviceUpdateSettingAction = tc_bus_ns.class_(
    "TCBusDeviceUpdateSettingAction", automation.Action
)

TCBusDeviceReadMemoryAction = tc_bus_ns.class_(
    "TCBusDeviceReadMemoryAction", automation.Action
)

TCBusDeviceIdentifyAction = tc_bus_ns.class_(
    "TCBusDeviceIdentifyAction", automation.Action
)

TelegramData = tc_bus_ns.struct(f"TelegramData")
SettingData = tc_bus_ns.struct(f"SettingData")
ModelData = tc_bus_ns.struct(f"ModelData")

IdentifyCompleteTrigger = tc_bus_ns.class_("IdentifyCompleteTrigger", automation.Trigger.template())
IdentifyUnknownTrigger = tc_bus_ns.class_("IdentifyUnknownTrigger", automation.Trigger.template())
IdentifyTimeoutTrigger = tc_bus_ns.class_("IdentifyTimeoutTrigger", automation.Trigger.template())
ReadMemoryCompleteTrigger = tc_bus_ns.class_("ReadMemoryCompleteTrigger", automation.Trigger.template())
ReadMemoryTimeoutTrigger = tc_bus_ns.class_("ReadMemoryTimeoutTrigger", automation.Trigger.template())

ReceivedTelegramTrigger = tc_bus_ns.class_("ReceivedTelegramTrigger", automation.Trigger.template())

SETTING_TYPE = tc_bus_ns.enum("SettingType")
SETTING_TYPES = {
    "ringtone_floor_call": SETTING_TYPE.SETTING_RINGTONE_FLOOR_CALL,
    "ringtone_entrance_door_call": SETTING_TYPE.SETTING_RINGTONE_ENTRANCE_DOOR_CALL,
    "ringtone_second_entrance_door_call": SETTING_TYPE.SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL,
    "ringtone_internal_call": SETTING_TYPE.SETTING_RINGTONE_INTERNAL_CALL,
    "volume_ringtone": SETTING_TYPE.SETTING_VOLUME_RINGTONE,
    "volume_handset_door": SETTING_TYPE.SETTING_VOLUME_HANDSET_DOOR,
    "volume_handset_internal": SETTING_TYPE.SETTING_VOLUME_HANDSET_INTERNAL
}

TELEGRAM_TYPE = tc_bus_ns.enum("TelegramType")
TELEGRAM_TYPES = {
    "unknown": TELEGRAM_TYPE.TELEGRAM_TYPE_UNKNOWN,
    "ack": TELEGRAM_TYPE.TELEGRAM_TYPE_ACK,
    "data": TELEGRAM_TYPE.TELEGRAM_TYPE_DATA,
    "search_doorman_devices": TELEGRAM_TYPE.TELEGRAM_TYPE_SEARCH_DOORMAN_DEVICES,
    "found_doorman_device": TELEGRAM_TYPE.TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE,
    "door_call": TELEGRAM_TYPE.TELEGRAM_TYPE_DOOR_CALL,
    "floor_call": TELEGRAM_TYPE.TELEGRAM_TYPE_FLOOR_CALL,
    "internal_call": TELEGRAM_TYPE.TELEGRAM_TYPE_INTERNAL_CALL,
    "control_function": TELEGRAM_TYPE.TELEGRAM_TYPE_CONTROL_FUNCTION,
    "start_talking_door_call": TELEGRAM_TYPE.TELEGRAM_TYPE_START_TALKING_DOOR_CALL,
    "start_talking": TELEGRAM_TYPE.TELEGRAM_TYPE_START_TALKING,
    "stop_talking_door_call": TELEGRAM_TYPE.TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL,
    "stop_talking": TELEGRAM_TYPE.TELEGRAM_TYPE_STOP_TALKING,
    "open_door": TELEGRAM_TYPE.TELEGRAM_TYPE_OPEN_DOOR,
    "open_door_long": TELEGRAM_TYPE.TELEGRAM_TYPE_OPEN_DOOR_LONG,
    "light": TELEGRAM_TYPE.TELEGRAM_TYPE_LIGHT,
    "door_opened": TELEGRAM_TYPE.TELEGRAM_TYPE_DOOR_OPENED,
    "door_closed": TELEGRAM_TYPE.TELEGRAM_TYPE_DOOR_CLOSED,
    "end_of_ringtone": TELEGRAM_TYPE.TELEGRAM_TYPE_END_OF_RINGTONE,
    "end_of_door_readiness": TELEGRAM_TYPE.TELEGRAM_TYPE_END_OF_DOOR_READINESS,
    "initialize_door_station": TELEGRAM_TYPE.TELEGRAM_TYPE_INITIALIZE_DOOR_STATION,
    "reset": TELEGRAM_TYPE.TELEGRAM_TYPE_RESET,
    "select_device_group": TELEGRAM_TYPE.TELEGRAM_TYPE_SELECT_DEVICE_GROUP,
    "select_device_group_reset": TELEGRAM_TYPE.TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET,
    "search_devices": TELEGRAM_TYPE.TELEGRAM_TYPE_SEARCH_DEVICES,
    "found_device": TELEGRAM_TYPE.TELEGRAM_TYPE_FOUND_DEVICE,
    "found_device_subsystem": TELEGRAM_TYPE.TELEGRAM_TYPE_FOUND_DEVICE_SUBSYSTEM,
    "programming_mode": TELEGRAM_TYPE.TELEGRAM_TYPE_PROGRAMMING_MODE,
    "read_memory_block": TELEGRAM_TYPE.TELEGRAM_TYPE_READ_MEMORY_BLOCK,
    "select_memory_page": TELEGRAM_TYPE.TELEGRAM_TYPE_SELECT_MEMORY_PAGE,
    "write_memory": TELEGRAM_TYPE.TELEGRAM_TYPE_WRITE_MEMORY,
    "request_version": TELEGRAM_TYPE.TELEGRAM_TYPE_REQUEST_VERSION
}

CONF_IS_MODELS = [
    "None",
    "TCS ISW3030 / Koch TC50 / Scantron Stilux",
    "TCS ISW3130 / Koch TC50P",
    "TCS ISW3230 / Koch TC50 GFA",
    "TCS ISW3330 / Koch TC50 BW",
    "TCS ISW3340",
    "TCS ISW5010 / Koch TC60",
    "TCS ISW5020",
    "TCS ISW5030",
    "TCS ISW5031",
    "TCS ISW5033",
    "TCS IVW511x / Koch VTC60 / Scantron VIVO",
    "TCS IVW521x / Koch VTC60/2D",
    "TCS ISW6031",
    "TCS ISW7030 / Koch TC70",
    "TCS IVW7510 / Koch VTC70",
    "TCS ISH7030 / Koch TCH70",
    "TCS IVH7510 / Koch VTCH70",
    "TCS ISW6010",
    "TCS IVW6511",
    "TCS ISWM7000",
    "TCS IVWM7000",
    "TCS ISW4100 / Koch TC31",
    "TCS IMM2100 / Koch TCE31",
    "TCS IVW2210 / Koch Ecoos",
    "TCS IVW2211 / Koch Ecoos",
    "TCS IVW2212 / Koch Ecoos / Scantron SLIM60T",
    "TCS VTC42V2",
    "TCS TC40V2",
    "TCS VTC40",
    "TCS TC40",
    "TCS TC2000",
    "TCS TC20P",
    "TCS TC20F",
    "TCS ISH3022 / Koch TCH50P",
    "TCS ISH3130 / Koch TCH50P / Scantron LuxPlus",
    "TCS ISH3230 / Koch TCH50 GFA",
    "TCS ISH3030 / Koch TCH50 / Scantron Lux2",
    "TCS ISH1030 / Koch TTS25",
    "TCS TTC-XX",
    "TCS TTS-XX",
    "TCS IMM1000 / Koch TCH30",
    "TCS IMM1100 / Koch TCHE30",
    "TCS IMM1300 / Koch VTCH30",
    "TCS IMM1500",
    "TCS IMM1310 / Koch VTCHE30",
    "TCS IMM1110 / Koch TCHEE30",
    "TCS IVH3222 / Koch VTCH50 / Scantron VLux",
    "TCS IVH4222 / Koch VTCH50/2D",
    "TCS IVW2220 / Koch Sky",
    "TCS IVW2221 / Koch Sky R1.00",
    "TCS IVW3011 / Koch Skyline Plus",
    "TCS IVW3012 / Koch Skyline/Aldup",
    "TCS VMH / Koch VMH",
    "TCS VML / Koch VML",
    "TCS VMF / Koch VMF",
    "Jung TKIS",
    "Jung TKISV",
    "TCS CAIXXXX / Koch CAIXXXX",
    "TCS CAI2000 / Koch Carus",
    "TCS ISW42X0",
    "TCS IVW9010",
    "TCS IVW9011 / Koch VTP10",
    "TCS IVW9110",
    "TCS IVW9030 / Scantron SLIM50T",
    "TCS IVE70",

    "TCS BVS20",
    "TCS BVS30",
    "TCS NBV3210",
    "TCS VBVS30",
    "TCS NBV2600",

    "DEBUG AS",
    "DEBUG IS0",
    "DEBUG IS1",
    "DEBUG CONTROLLER",
]

CONF_RINGTONES = [
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
]

CONF_TC_BUS_ID = "tc_bus"

CONF_SERIAL_NUMBER = "serial_number"
CONF_DEVICE_GROUP = "device_group"
CONF_TELEGRAM = "telegram"
CONF_IS_LONG = "is_long"
CONF_ADDRESS = "address"
CONF_ADDRESS_LAMBDA = "address_lambda"
CONF_PAYLOAD = "payload"
CONF_PAYLOAD_LAMBDA = "payload_lambda"

CONF_ON_TELEGRAM = "on_telegram"
CONF_ON_READ_MEMORY_COMPLETE = "on_read_memory_complete"
CONF_ON_READ_MEMORY_TIMEOUT = "on_read_memory_timeout"
CONF_ON_IDENTIFY_COMPLETE = "on_identify_complete"
CONF_ON_IDENTIFY_UNKNOWN = "on_identify_unknown"
CONF_ON_IDENTIFY_TIMEOUT = "on_identify_timeout"

def validate_config(config):
    return config

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID() : cv.declare_id(TCBusDeviceComponent),
        cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_ON_TELEGRAM): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReceivedTelegramTrigger),
            }
        ),
        cv.Optional(CONF_ON_READ_MEMORY_COMPLETE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadMemoryCompleteTrigger),
            }
        ),
        cv.Optional(CONF_ON_READ_MEMORY_TIMEOUT): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadMemoryTimeoutTrigger),
            }
        ),
        cv.Optional(CONF_ON_IDENTIFY_COMPLETE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IdentifyCompleteTrigger),
            }
        ),
        cv.Optional(CONF_ON_IDENTIFY_UNKNOWN): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IdentifyUnknownTrigger),
            }
        ),
        cv.Optional(CONF_ON_IDENTIFY_TIMEOUT): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IdentifyTimeoutTrigger),
            }
        ),
    }
)

CONFIG_SCHEMA = cv.All(
    CONFIG_SCHEMA.extend(cv.COMPONENT_SCHEMA),
    validate_config,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    tc_bus_component = await cg.get_variable(config[CONF_TC_BUS_ID])
    cg.add(var.set_base_component(tc_bus_component))

    for conf in config.get(CONF_ON_TELEGRAM, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    for conf in config.get(CONF_ON_READ_MEMORY_COMPLETE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_vector.template(cg.uint8), "x")], conf)

    for conf in config.get(CONF_ON_READ_MEMORY_TIMEOUT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_IDENTIFY_COMPLETE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(ModelData, "x")], conf)

    for conf in config.get(CONF_ON_IDENTIFY_UNKNOWN, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_IDENTIFY_TIMEOUT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)



def validate(config):
    config = config.copy()

    if CONF_TELEGRAM in config and CONF_TYPE in config:
        raise cv.Invalid("You can either set TELEGRAM or TYPE, ADDRESS and SERIAL_NUMBER.")

    return config

TC_BUS_DEVICE_SEND_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
        cv.Optional(CONF_TELEGRAM): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_IS_LONG): cv.templatable(cv.boolean),
        cv.Optional(CONF_TYPE): cv.templatable(cv.enum(TELEGRAM_TYPES, upper=False)),
        cv.Optional(CONF_ADDRESS, default="0"): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_PAYLOAD, default="0"): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_SERIAL_NUMBER, default="0"): cv.templatable(cv.hex_uint32_t),
    }),
    validate
)

@automation.register_action(
    "tc_bus_device.send",
    TCBusDeviceSendAction,
    TC_BUS_DEVICE_SEND_SCHEMA
)
async def tc_bus_send_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    if CONF_TELEGRAM in config:
        telegram_template_ = await cg.templatable(config[CONF_TELEGRAM], args, cg.uint32)
        cg.add(var.set_telegram(telegram_template_))

    if CONF_IS_LONG in config:
        is_long_template_ = await cg.templatable(config[CONF_IS_LONG], args, cg.bool_)
        cg.add(var.set_is_long(is_long_template_))

    if CONF_TYPE in config:
        type_template_ = await cg.templatable(config[CONF_TYPE], args, TELEGRAM_TYPE)
        cg.add(var.set_type(type_template_))

    if CONF_ADDRESS in config:
        address_template_ = await cg.templatable(config[CONF_ADDRESS], args, cg.uint8)
        cg.add(var.set_address(address_template_))

    if CONF_PAYLOAD in config:
        payload_template_ = await cg.templatable(config[CONF_PAYLOAD], args, cg.uint32)
        cg.add(var.set_payload(payload_template_))

    if CONF_SERIAL_NUMBER in config:
        serial_number_template_ = await cg.templatable(config[CONF_SERIAL_NUMBER], args, cg.uint32)
        cg.add(var.set_serial_number(serial_number_template_))

    return var



TC_BUS_DEVICE_UPDATE_SETTING_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
        cv.Required(CONF_TYPE): cv.templatable(cv.enum(SETTING_TYPES, upper=False)),
        cv.Required(CONF_VALUE): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_SERIAL_NUMBER, default="0"): cv.templatable(cv.hex_uint32_t),
    })
)

@automation.register_action(
    "tc_bus_device.update_setting",
    TCBusDeviceUpdateSettingAction,
    TC_BUS_DEVICE_UPDATE_SETTING_SCHEMA
)
async def tc_bus_update_setting_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    if CONF_TYPE in config:
        type_template_ = await cg.templatable(config[CONF_TYPE], args, SETTING_TYPE)
        cg.add(var.set_type(type_template_))

    if CONF_VALUE in config:
        value_template_ = await cg.templatable(config[CONF_VALUE], args, cg.uint8)
        cg.add(var.set_value(value_template_))

    if CONF_SERIAL_NUMBER in config:
        serial_number_template_ = await cg.templatable(config[CONF_SERIAL_NUMBER], args, cg.uint32)
        cg.add(var.set_serial_number(serial_number_template_))

    return var



@automation.register_action(
    "tc_bus_device.read_memory",
    TCBusDeviceReadMemoryAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
            cv.Optional(CONF_SERIAL_NUMBER, default=0): cv.templatable(cv.hex_uint32_t)
        }
    ),
)
async def tc_bus_read_memory_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    serial_number_template_ = await cg.templatable(config[CONF_SERIAL_NUMBER], args, cg.uint32)
    cg.add(var.set_serial_number(serial_number_template_))
    
    return var



@automation.register_action(
    "tc_bus_device.identify",
    TCBusDeviceIdentifyAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
            cv.Optional(CONF_SERIAL_NUMBER, default=0): cv.templatable(cv.hex_uint32_t),
            cv.Optional(CONF_DEVICE_GROUP, default=0): cv.templatable(cv.uint8_t)
        }
    ),
)
async def tc_bus_request_version_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    serial_number_template_ = await cg.templatable(config[CONF_SERIAL_NUMBER], args, cg.uint32)
    cg.add(var.set_serial_number(serial_number_template_))

    device_group_template_ = await cg.templatable(config[CONF_DEVICE_GROUP], args, cg.uint8)
    cg.add(var.set_device_group(device_group_template_))
    
    return var