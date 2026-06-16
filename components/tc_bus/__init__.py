import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins, automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_TYPE

CODEOWNERS = ["@azoninc"]

DEPENDENCIES = []
MULTI_CONF = False

tc_bus_ns = cg.esphome_ns.namespace("tc_bus")
TCBusComponent = tc_bus_ns.class_("TCBusComponent", cg.Component)

TCBusSendAction = tc_bus_ns.class_(
    "TCBusSendAction", automation.Action, cg.Parented.template(TCBusComponent)
)

TCBusProgrammingModeAction = tc_bus_ns.class_(
    "TCBusProgrammingModeAction", automation.Action, cg.Parented.template(TCBusComponent)
)

TCBusSystemDiscoveryAction = tc_bus_ns.class_(
    "TCBusSystemDiscoveryAction", automation.Action, cg.Parented.template(TCBusComponent)
)

TCBusAddressDiscoveryAction = tc_bus_ns.class_(
    "TCBusAddressDiscoveryAction", automation.Action, cg.Parented.template(TCBusComponent)
)

TelegramData = tc_bus_ns.struct("TelegramData")

ReceivedTelegramTrigger = tc_bus_ns.class_("ReceivedTelegramTrigger", automation.Trigger.template())
SystemDiscoveryCompleteTrigger = tc_bus_ns.class_("SystemDiscoveryCompleteTrigger", automation.Trigger.template())
AddressDiscoveryCompleteTrigger = tc_bus_ns.class_("AddressDiscoveryCompleteTrigger", automation.Trigger.template())

TELEGRAM_TYPE = tc_bus_ns.enum("TelegramType")
TELEGRAM_TYPES = {
    "unknown": TELEGRAM_TYPE.TELEGRAM_TYPE_UNKNOWN,
    "ack_status": TELEGRAM_TYPE.TELEGRAM_TYPE_ACK_STATUS,
    "ack_data": TELEGRAM_TYPE.TELEGRAM_TYPE_ACK_DATA,
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

CONF_TC_BUS_ID = "tc_bus_id"

CONF_RX_PIN = "rx_pin"
CONF_TX_PIN = "tx_pin"

CONF_TELEGRAM = "telegram"
CONF_IS_LONG = "is_long"
CONF_ADDRESS = "address"
CONF_PAYLOAD = "payload"
CONF_SERIAL_NUMBER = "serial_number"

CONF_ON_TELEGRAM = "on_telegram"
CONF_ON_SYSTEM_DISCOVERY_COMPLETE = "on_system_discovery_complete"
CONF_ON_ADDRESS_DISCOVERY_COMPLETE = "on_address_discovery_complete"

CONF_PROGRAMMING_MODE = "programming_mode"
CONF_DEVICE_GROUP = "device_group"

def validate_config(config):
    return config

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(TCBusComponent),
        cv.Optional(CONF_RX_PIN, default=9): pins.internal_gpio_input_pin_schema,
        cv.Optional(CONF_TX_PIN, default=8): pins.internal_gpio_output_pin_schema,
        cv.Optional(CONF_ON_TELEGRAM): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReceivedTelegramTrigger),
            }
        ),
        cv.Optional(CONF_ON_SYSTEM_DISCOVERY_COMPLETE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(SystemDiscoveryCompleteTrigger),
            }
        ),
        cv.Optional(CONF_ON_ADDRESS_DISCOVERY_COMPLETE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(AddressDiscoveryCompleteTrigger),
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

    tx_pin = await cg.gpio_pin_expression(config[CONF_TX_PIN])
    cg.add(var.set_tx_pin(tx_pin))

    if CONF_ON_TELEGRAM in config:
        cg.add_define("USE_RECEIVED_TELEGRAM_CALLBACK")
        for conf in config.get(CONF_ON_TELEGRAM, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    if CONF_ON_SYSTEM_DISCOVERY_COMPLETE in config:
        cg.add_define("USE_SYSTEM_DISCOVERY_COMPLETE_CALLBACK")
        for conf in config.get(CONF_ON_SYSTEM_DISCOVERY_COMPLETE, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [(cg.uint16, "x")], conf)

    if CONF_ON_ADDRESS_DISCOVERY_COMPLETE in config:
        cg.add_define("USE_ADDRESS_DISCOVERY_COMPLETE_CALLBACK")
        for conf in config.get(CONF_ON_ADDRESS_DISCOVERY_COMPLETE, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [(cg.uint8, "x")], conf)

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
    TC_BUS_SEND_SCHEMA,
    synchronous=True
)
async def tc_bus_send_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

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
            cv.Optional(CONF_PROGRAMMING_MODE, default=False): cv.templatable(cv.boolean)
        }
    ),
    synchronous=True
)
async def tc_bus_set_programming_mode_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    programming_mode_template_ = await cg.templatable(config[CONF_PROGRAMMING_MODE], args, cg.bool_)
    cg.add(var.set_programming_mode(programming_mode_template_))
    
    return var


@automation.register_action(
    "tc_bus.discover_system_devices",
    TCBusSystemDiscoveryAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(TCBusComponent),
            cv.Optional(CONF_DEVICE_GROUP, default=255): cv.templatable(cv.uint8_t)
        }
    ),
    synchronous=True
)
async def tc_bus_discover_system_devices_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    device_group_template_ = await cg.templatable(config[CONF_DEVICE_GROUP], args, cg.uint8)
    cg.add(var.set_device_group(device_group_template_))
    
    return var


@automation.register_action(
    "tc_bus.discover_outdoor_station_addresses",
    TCBusAddressDiscoveryAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(TCBusComponent)
        }
    ),
    synchronous=True
)
async def tc_bus_discover_outdoor_station_addresses_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    return var