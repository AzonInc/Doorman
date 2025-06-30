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

TCBusProgrammingModeAction = tc_bus_ns.class_(
    "TCBusProgrammingModeAction", automation.Action
)

TelegramData = tc_bus_ns.struct("TelegramData")

ReceivedTelegramTrigger = tc_bus_ns.class_("ReceivedTelegramTrigger", automation.Trigger.template())

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

CONF_TC_BUS_ID = "tc_bus"

CONF_TRANSMITTER_ID = "transmitter_id"
CONF_RECEIVER_ID = "receiver_id"

CONF_EVENT = "event"

CONF_SERIAL_NUMBER = "serial_number"
CONF_DEVICE_GROUP = "device_group"
CONF_TELEGRAM = "telegram"
CONF_IS_LONG = "is_long"
CONF_ADDRESS = "address"
CONF_ADDRESS_LAMBDA = "address_lambda"
CONF_PAYLOAD = "payload"
CONF_PAYLOAD_LAMBDA = "payload_lambda"

CONF_ON_TELEGRAM = "on_telegram"

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
        cv.Optional(CONF_ON_TELEGRAM): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReceivedTelegramTrigger),
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

    for conf in config.get(CONF_ON_TELEGRAM, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(TelegramData, "x")], conf)


def validate(config):
    config = config.copy()

    if CONF_TELEGRAM in config and CONF_TYPE in config:
        raise cv.Invalid("You can either set TELEGRAM or TYPE, ADDRESS, PAYLOAD and SERIAL_NUMBER.")

    return config

TC_BUS_SEND_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(): cv.use_id(TCBusComponent),
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
    "tc_bus.send",
    TCBusSendAction,
    TC_BUS_SEND_SCHEMA
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