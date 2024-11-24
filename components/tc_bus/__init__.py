import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins, automation
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC, ENTITY_CATEGORY_CONFIG, CONF_TRIGGER_ID, CONF_TYPE, CONF_VALUE

CODEOWNERS = ["@azoninc"]

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

CommandData = tc_bus_ns.struct(f"CommandData")
SettingData = tc_bus_ns.struct(f"SettingData")

ReadMemoryCompleteTrigger = tc_bus_ns.class_("ReadMemoryCompleteTrigger", automation.Trigger.template())
ReadMemoryTimeoutTrigger = tc_bus_ns.class_("ReadMemoryTimeoutTrigger", automation.Trigger.template())
ReceivedCommandTrigger = tc_bus_ns.class_("ReceivedCommandTrigger", automation.Trigger.template())

SETTING_TYPE = tc_bus_ns.enum("SettingType")
SETTING_TYPES = {
    "ringtone_floor_call": SETTING_TYPE.SETTING_RINGTONE_FLOOR_CALL,
    "ringtone_door_call": SETTING_TYPE.SETTING_RINGTONE_DOOR_CALL,
    "ringtone_internal_call": SETTING_TYPE.SETTING_RINGTONE_INTERNAL_CALL,
    "volume_ringtone": SETTING_TYPE.SETTING_VOLUME_RINGTONE,
    "volume_handset": SETTING_TYPE.SETTING_VOLUME_HANDSET
}

COMMAND_TYPE = tc_bus_ns.enum("CommandType")
COMMAND_TYPES = {
    "unknown": COMMAND_TYPE.COMMAND_TYPE_UNKNOWN,
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
    "TCS ISH1030", 
    "TCS ISH3030",
    "TCS ISH3230",
    "TCS ISH3340",
    "TCS ISW3030",
    "TCS ISW3230",
    "TCS ISW3340",
    "TCS IVH3222",
    "Koch TC50",
    "Koch TCH50",
    "Koch TCH50P"
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

CONF_RX_PIN = "rx_pin"
CONF_TX_PIN = "tx_pin"
CONF_EVENT = "event"

CONF_SERIAL_NUMBER = "serial_number"
CONF_COMMAND = "command"
CONF_ADDRESS = "address"
CONF_ADDRESS_LAMBDA = "address_lambda"
CONF_PAYLOAD = "payload"
CONF_PAYLOAD_LAMBDA = "payload_lambda"

CONF_ON_COMMAND = "on_command"
CONF_ON_MEMORY = "on_read_memory_complete"
CONF_ON_MEMORY_TIMEOUT = "on_read_memory_timeout"

CONF_PROGRAMMING_MODE = "programming_mode"

MULTI_CONF = False

def validate_config(config):
    return config

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(TCBusComponent),
        cv.Optional(CONF_RX_PIN, default=9): pins.internal_gpio_input_pin_schema,
        cv.Optional(CONF_TX_PIN, default=8): pins.internal_gpio_output_pin_schema,
        cv.Optional(CONF_EVENT, default="tc"): cv.string,
        cv.Optional(CONF_ON_COMMAND): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReceivedCommandTrigger),
            }
        ),
        cv.Optional(CONF_ON_MEMORY): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadMemoryCompleteTrigger),
            }
        ),
        cv.Optional(CONF_ON_MEMORY_TIMEOUT): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadMemoryTimeoutTrigger),
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

    rx_pin = await cg.gpio_pin_expression(config[CONF_RX_PIN])
    cg.add(var.set_rx_pin(rx_pin))

    pin = await cg.gpio_pin_expression(config[CONF_TX_PIN])
    cg.add(var.set_tx_pin(pin))

    cg.add(var.set_event("esphome." + config[CONF_EVENT]))

    for conf in config.get(CONF_ON_COMMAND, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(CommandData, "x")], conf)

    for conf in config.get(CONF_ON_MEMORY, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_vector.template(cg.uint8), "x")], conf)

    for conf in config.get(CONF_ON_MEMORY_TIMEOUT, []):
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