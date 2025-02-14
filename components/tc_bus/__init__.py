import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import remote_transmitter, remote_receiver
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_TYPE, CONF_VALUE

CODEOWNERS = ["@azoninc"]

DEPENDENCIES = ["remote_transmitter", "remote_receiver"]
MULTI_CONF = False

tc_bus_ns = cg.esphome_ns.namespace("tc_bus")
TCBusComponent = tc_bus_ns.class_("TCBusComponent", cg.Component)

TCBusSendAction = tc_bus_ns.class_(
    "TCBusSendAction", automation.Action
)

TCBusUpdateSettingAction = tc_bus_ns.class_(
    "TCBusUpdateSettingAction", automation.Action
)

TCBusProgrammingModeAction = tc_bus_ns.class_(
    "TCBusProgrammingModeAction", automation.Action
)

TCBusReadMemoryAction = tc_bus_ns.class_(
    "TCBusReadMemoryAction", automation.Action
)

TCBusIdentifyAction = tc_bus_ns.class_(
    "TCBusIdentifyAction", automation.Action
)

CommandData = tc_bus_ns.struct(f"CommandData")
SettingData = tc_bus_ns.struct(f"SettingData")
ModelData = tc_bus_ns.struct(f"ModelData")

IdentifyCompleteTrigger = tc_bus_ns.class_("IdentifyCompleteTrigger", automation.Trigger.template())
IdentifyTimeoutTrigger = tc_bus_ns.class_("IdentifyTimeoutTrigger", automation.Trigger.template())
ReadMemoryCompleteTrigger = tc_bus_ns.class_("ReadMemoryCompleteTrigger", automation.Trigger.template())
ReadMemoryTimeoutTrigger = tc_bus_ns.class_("ReadMemoryTimeoutTrigger", automation.Trigger.template())

ReceivedCommandTrigger = tc_bus_ns.class_("ReceivedCommandTrigger", automation.Trigger.template())

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

COMMAND_TYPE = tc_bus_ns.enum("CommandType")
COMMAND_TYPES = {
    "unknown": COMMAND_TYPE.COMMAND_TYPE_UNKNOWN,
    "ack": COMMAND_TYPE.COMMAND_TYPE_ACK,
    "search_doorman_devices": COMMAND_TYPE.COMMAND_TYPE_SEARCH_DOORMAN_DEVICES,
    "found_doorman_device": COMMAND_TYPE.COMMAND_TYPE_FOUND_DOORMAN_DEVICE,
    "door_call": COMMAND_TYPE.COMMAND_TYPE_DOOR_CALL,
    "floor_call": COMMAND_TYPE.COMMAND_TYPE_FLOOR_CALL,
    "internal_call": COMMAND_TYPE.COMMAND_TYPE_INTERNAL_CALL,
    "control_function": COMMAND_TYPE.COMMAND_TYPE_CONTROL_FUNCTION,
    "start_talking_door_call": COMMAND_TYPE.COMMAND_TYPE_START_TALKING_DOOR_CALL,
    "start_talking": COMMAND_TYPE.COMMAND_TYPE_START_TALKING,
    "stop_talking_door_call": COMMAND_TYPE.COMMAND_TYPE_STOP_TALKING_DOOR_CALL,
    "stop_talking": COMMAND_TYPE.COMMAND_TYPE_STOP_TALKING,
    "open_door": COMMAND_TYPE.COMMAND_TYPE_OPEN_DOOR,
    "open_door_long": COMMAND_TYPE.COMMAND_TYPE_OPEN_DOOR_LONG,
    "light": COMMAND_TYPE.COMMAND_TYPE_LIGHT,
    "door_opened": COMMAND_TYPE.COMMAND_TYPE_DOOR_OPENED,
    "door_closed": COMMAND_TYPE.COMMAND_TYPE_DOOR_CLOSED,
    "end_of_ringtone": COMMAND_TYPE.COMMAND_TYPE_END_OF_RINGTONE,
    "end_of_door_readiness": COMMAND_TYPE.COMMAND_TYPE_END_OF_DOOR_READINESS,
    "initialize_door_station": COMMAND_TYPE.COMMAND_TYPE_INITIALIZE_DOOR_STATION,
    "reset": COMMAND_TYPE.COMMAND_TYPE_RESET,
    "select_device_group": COMMAND_TYPE.COMMAND_TYPE_SELECT_DEVICE_GROUP,
    "select_device_group_reset": COMMAND_TYPE.COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET,
    "search_devices": COMMAND_TYPE.COMMAND_TYPE_SEARCH_DEVICES,
    "found_device": COMMAND_TYPE.COMMAND_TYPE_FOUND_DEVICE,
    "found_device_subsystem": COMMAND_TYPE.COMMAND_TYPE_FOUND_DEVICE_SUBSYSTEM,
    "programming_mode": COMMAND_TYPE.COMMAND_TYPE_PROGRAMMING_MODE,
    "read_memory_block": COMMAND_TYPE.COMMAND_TYPE_READ_MEMORY_BLOCK,
    "select_memory_page": COMMAND_TYPE.COMMAND_TYPE_SELECT_MEMORY_PAGE,
    "write_memory": COMMAND_TYPE.COMMAND_TYPE_WRITE_MEMORY,
    "request_version": COMMAND_TYPE.COMMAND_TYPE_REQUEST_VERSION
}

CONF_MODELS = [
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
    "TCS IVE70"
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

CONF_TC_ID = "tc_bus"

CONF_TRANSMITTER_ID = "transmitter_id"
CONF_RECEIVER_ID = "receiver_id"

CONF_EVENT = "event"

CONF_SERIAL_NUMBER = "serial_number"
CONF_COMMAND = "command"
CONF_IS_LONG = "is_long"
CONF_ADDRESS = "address"
CONF_ADDRESS_LAMBDA = "address_lambda"
CONF_PAYLOAD = "payload"
CONF_PAYLOAD_LAMBDA = "payload_lambda"

CONF_ON_COMMAND = "on_command"
CONF_ON_READ_MEMORY_COMPLETE = "on_read_memory_complete"
CONF_ON_READ_MEMORY_TIMEOUT = "on_read_memory_timeout"
CONF_ON_IDENTIFY_COMPLETE = "on_identify_complete"
CONF_ON_IDENTIFY_TIMEOUT = "on_identify_timeout"

CONF_PROGRAMMING_MODE = "programming_mode"

def validate_config(config):
    return config

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(TCBusComponent),
        cv.GenerateID(CONF_TRANSMITTER_ID): cv.use_id(
            remote_transmitter.RemoteTransmitterComponent
        ),
        cv.GenerateID(CONF_RECEIVER_ID): cv.use_id(
            remote_receiver.RemoteReceiverComponent
        ),
        cv.Optional(CONF_EVENT, default="tc"): cv.string,
        cv.Optional(CONF_ON_COMMAND): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReceivedCommandTrigger),
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
    cg.add_global(tc_bus_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    transmitter = await cg.get_variable(config[CONF_TRANSMITTER_ID])
    cg.add(var.set_tx(transmitter))

    receiver = await cg.get_variable(config[CONF_RECEIVER_ID])
    cg.add(var.set_rx(receiver))

    cg.add(var.set_event("esphome." + config[CONF_EVENT]))

    for conf in config.get(CONF_ON_COMMAND, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(CommandData, "x")], conf)

    for conf in config.get(CONF_ON_READ_MEMORY_COMPLETE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_vector.template(cg.uint8), "x")], conf)

    for conf in config.get(CONF_ON_READ_MEMORY_TIMEOUT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_IDENTIFY_COMPLETE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(ModelData, "x")], conf)

    for conf in config.get(CONF_ON_IDENTIFY_TIMEOUT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)



def validate(config):
    config = config.copy()

    if CONF_COMMAND in config and CONF_TYPE in config:
        raise cv.Invalid("You can either set COMMAND or TYPE, ADDRESS and SERIAL_NUMBER.")

    return config

TC_BUS_SEND_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(): cv.use_id(TCBusComponent),
        cv.Optional(CONF_COMMAND): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_IS_LONG): cv.templatable(cv.boolean),
        cv.Optional(CONF_TYPE): cv.templatable(cv.enum(COMMAND_TYPES, upper=False)),
        cv.Optional(CONF_ADDRESS, default="0"): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_PAYLOAD, default="0"): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_SERIAL_NUMBER, default="0"): cv.templatable(cv.hex_uint32_t),
    }),
    validate
)

@automation.register_action(
    "tc_bus.send",
    TCBusSendAction,
    TC_BUS_SEND_SCHEMA
)
async def tc_bus_send_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    if CONF_COMMAND in config:
        command_template_ = await cg.templatable(config[CONF_COMMAND], args, cg.uint32)
        cg.add(var.set_command(command_template_))

    if CONF_IS_LONG in config:
        is_long_template_ = await cg.templatable(config[CONF_IS_LONG], args, cg.bool_)
        cg.add(var.set_is_long(is_long_template_))

    if CONF_TYPE in config:
        type_template_ = await cg.templatable(config[CONF_TYPE], args, COMMAND_TYPE)
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



TC_BUS_UPDATE_SETTING_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(): cv.use_id(TCBusComponent),
        cv.Required(CONF_TYPE): cv.templatable(cv.enum(SETTING_TYPES, upper=False)),
        cv.Required(CONF_VALUE): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_SERIAL_NUMBER, default="0"): cv.templatable(cv.hex_uint32_t),
    })
)

@automation.register_action(
    "tc_bus.update_setting",
    TCBusUpdateSettingAction,
    TC_BUS_UPDATE_SETTING_SCHEMA
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
    "tc_bus.set_programming_mode",
    TCBusProgrammingModeAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(TCBusComponent),
            cv.Required(CONF_PROGRAMMING_MODE): cv.templatable(cv.boolean)
        }
    ),
)
async def tc_bus_set_programming_mode_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    programming_mode_template_ = await cg.templatable(config[CONF_PROGRAMMING_MODE], args, cg.bool_)
    cg.add(var.set_programming_mode(programming_mode_template_))
    
    return var


@automation.register_action(
    "tc_bus.read_memory",
    TCBusReadMemoryAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(TCBusComponent),
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
    "tc_bus.identify",
    TCBusIdentifyAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(TCBusComponent),
            cv.Optional(CONF_SERIAL_NUMBER, default=0): cv.templatable(cv.hex_uint32_t)
        }
    ),
)
async def tc_bus_request_version_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    serial_number_template_ = await cg.templatable(config[CONF_SERIAL_NUMBER], args, cg.uint32)
    cg.add(var.set_serial_number(serial_number_template_))
    
    return var