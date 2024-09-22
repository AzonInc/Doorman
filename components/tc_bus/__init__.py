import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, binary_sensor
from esphome import pins, automation
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC, CONF_TRIGGER_ID, CONF_TYPE

AUTO_LOAD = ["binary_sensor", "text_sensor"]

CODEOWNERS = ["@azoninc"]
tc_bus_ns = cg.esphome_ns.namespace("tc_bus")
TCBus = tc_bus_ns.class_("TCBusComponent", cg.Component)

TCBusSendAction = tc_bus_ns.class_(
    "TCBusSendAction", automation.Action
)

TCBusProgrammingModeAction = tc_bus_ns.class_(
    "TCBusProgrammingModeAction", automation.Action
)

TCBusReadEEPROMAction = tc_bus_ns.class_(
    "TCBusReadEEPROMAction", automation.Action
)

ReceivedCommandTrigger = tc_bus_ns.class_("ReceivedCommandTrigger", automation.Trigger.template())
CommandData = tc_bus_ns.struct(f"CommandData")

CommandType = tc_bus_ns.enum("Command_Type")
COMMAND_TYPES = {
    "unknown": CommandType.COMMAND_TYPE_UNKNOWN,
    "door_call": CommandType.COMMAND_TYPE_DOOR_CALL,
    "floor_call": CommandType.COMMAND_TYPE_FLOOR_CALL,
    "internal_call": CommandType.COMMAND_TYPE_INTERNAL_CALL,
    "control_function": CommandType.COMMAND_TYPE_CONTROL_FUNCTION,
    "start_talking_door_station": CommandType.COMMAND_TYPE_START_TALKING_DOOR_STATION,
    "start_talking_ia": CommandType.COMMAND_TYPE_START_TALKING_IA,
    "stop_talking_door_station": CommandType.COMMAND_TYPE_STOP_TALKING_DOOR_STATION,
    "stop_talking_ia": CommandType.COMMAND_TYPE_STOP_TALKING_IA,
    "open_door": CommandType.COMMAND_TYPE_OPEN_DOOR,
    "light": CommandType.COMMAND_TYPE_LIGHT,
    "door_opened": CommandType.COMMAND_TYPE_DOOR_OPENED,
    "door_closed": CommandType.COMMAND_TYPE_DOOR_CLOSED,
    "end_of_ringtone": CommandType.COMMAND_TYPE_END_OF_RINGTONE,
    "end_of_door_readiness": CommandType.COMMAND_TYPE_END_OF_DOOR_READINESS,
    "initialize_door_station": CommandType.COMMAND_TYPE_INITIALIZE_DOOR_STATION,
    "reset": CommandType.COMMAND_TYPE_RESET,
    "select_device_group": CommandType.COMMAND_TYPE_SELECT_DEVICE_GROUP,
    "select_device_group_reset": CommandType.COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET,
    "search_devices": CommandType.COMMAND_TYPE_SEARCH_DEVICES,
    "found_device": CommandType.COMMAND_TYPE_FOUND_DEVICE,
    "found_device_subsystem": CommandType.COMMAND_TYPE_FOUND_DEVICE_SUBSYSTEM,
    "programming_mode": CommandType.COMMAND_TYPE_PROGRAMMING_MODE,
    "read_eeprom_block": CommandType.COMMAND_TYPE_READ_EEPROM_BLOCK,
    "select_eeprom_page": CommandType.COMMAND_TYPE_SELECT_EEPROM_PAGE,
}

CONF_TC_ID = "tc_bus"

CONF_RX_PIN = "rx_pin"
CONF_TX_PIN = "tx_pin"

CONF_EVENT = "event"

CONF_COMMAND = "command"
CONF_SERIAL_NUMBER = "serial_number"
CONF_SERIAL_NUMBER_LAMBDA = "serial_number_lambda"
CONF_ADDRESS = "address"
CONF_ADDRESS_LAMBDA = "address_lambda"

CONF_BUS_COMMAND = "bus_command"
CONF_HARDWARE_VERSION = "hardware_version"
CONF_DOOR_READINESS = "door_readiness"

CONF_ON_COMMAND = "on_command_action"
CONF_PROGRAMMING_MODE = "programming_mode"

MULTI_CONF = False

def validate_config(config):
    return config

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TCBus),
            cv.Optional(CONF_RX_PIN, default=9): pins.internal_gpio_input_pin_schema,
            cv.Optional(CONF_TX_PIN, default=8): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_EVENT, default="tc"): cv.string,
            cv.Optional(CONF_SERIAL_NUMBER, default=0): cv.hex_uint32_t,
            cv.Optional(CONF_SERIAL_NUMBER_LAMBDA): cv.returning_lambda,
            cv.Optional(CONF_BUS_COMMAND): text_sensor.text_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                icon="mdi:console-network",
            ),
            cv.Optional(CONF_HARDWARE_VERSION): text_sensor.text_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                icon="mdi:router-network-wireless",
            ),
            cv.Optional(CONF_DOOR_READINESS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                icon="mdi:router-network-wireless",
            ),
            cv.Optional(CONF_ON_COMMAND): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReceivedCommandTrigger),
                }
            ),
        }   
    )
    .extend(cv.COMPONENT_SCHEMA),
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

    if CONF_SERIAL_NUMBER in config:
        cg.add(var.set_sn(config[CONF_SERIAL_NUMBER]))

    if CONF_SERIAL_NUMBER_LAMBDA in config:
        template_ = await cg.process_lambda(
            config[CONF_SERIAL_NUMBER_LAMBDA], [], return_type=cg.optional.template(cg.uint32)
        )
        cg.add(var.set_sn_lambda(template_))

    if CONF_BUS_COMMAND in config:
        sens = await text_sensor.new_text_sensor(config[CONF_BUS_COMMAND])
        cg.add(var.set_bus_command_sensor(sens))

    if CONF_HARDWARE_VERSION in config:
        sens = await text_sensor.new_text_sensor(config[CONF_HARDWARE_VERSION])
        cg.add(var.set_hardware_version_sensor(sens))

    if CONF_DOOR_READINESS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_DOOR_READINESS])
        cg.add(var.set_door_readiness_sensor(sens))

    for conf in config.get(CONF_ON_COMMAND, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(CommandData, "x")], conf)




def validate(config):
    config = config.copy()

    if CONF_COMMAND in config and CONF_TYPE in config:
        raise cv.Invalid("You can either set COMMAND or TYPE, ADDRESS and SERIAL_NUMBER.")

    return config

TC_BUS_SEND_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(): cv.use_id(TCBus),
        cv.Optional(CONF_COMMAND): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_ADDRESS, default="0"): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_TYPE): cv.templatable(cv.enum(COMMAND_TYPES, upper=False)),
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
        type_template_ = await cg.templatable(config[CONF_TYPE], args, CommandType)
        cg.add(var.set_type(type_template_))

    if CONF_ADDRESS in config:
        address_template_ = await cg.templatable(config[CONF_ADDRESS], args, cg.uint8)
        cg.add(var.set_address(address_template_))

    if CONF_SERIAL_NUMBER in config:
        serial_number_template_ = await cg.templatable(config[CONF_SERIAL_NUMBER], args, cg.uint32)
        cg.add(var.set_serial_number(serial_number_template_))

    return var


@automation.register_action(
    "tc_bus.set_programming_mode",
    TCBusProgrammingModeAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(TCBus),
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
    "tc_bus.read_eeprom",
    TCBusReadEEPROMAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(TCBus),
            cv.Optional(CONF_SERIAL_NUMBER, default=0): cv.templatable(cv.hex_uint32_t)
        }
    ),
)
async def tc_bus_read_eeprom_to_code(config, action_id, template_args, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, parent)

    serial_number_template_ = await cg.templatable(config[CONF_SERIAL_NUMBER], args, cg.uint32)
    cg.add(var.set_serial_number(serial_number_template_))
    
    return var