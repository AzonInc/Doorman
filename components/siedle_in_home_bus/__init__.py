from esphome import automation, pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_RX_PIN, CONF_TX_PIN, CONF_DUMP, CONF_COMMAND, CONF_SOURCE

CODEOWNERS = []
MULTI_CONF = True
CONF_SIEDLE_IN_HOME_BUS_ID = "siedle_in_home_bus_id"

# Required for ESP_TIMER_ISR dispatch mode
cg.add_define("CONFIG_ESP_TIMER_IN_IRAM")
cg.add_define("CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD")

siedle_in_home_bus_ns = cg.esphome_ns.namespace("siedle_in_home_bus")
SiedleInHomeBusComponent = siedle_in_home_bus_ns.class_("SiedleInHomeBusComponent", cg.Component)
SiedleInHomeBusMessage = siedle_in_home_bus_ns.class_("SiedleInHomeBusMessage")
SiedleInHomeBusSendAction = siedle_in_home_bus_ns.class_(
    "SiedleInHomeBusSendAction", automation.Action
)

CONF_CARRIER_PIN = "carrier_pin"
CONF_LOAD_PIN = "load_pin"
CONF_DESTINATION = "destination"
CONF_DESTINATION_BUS = "destination_bus"
CONF_SOURCE_BUS = "source_bus"
CONF_ON_MESSAGE = "on_message"
CONF_RAW = "raw"

CONFIG_MESSAGE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_COMMAND): cv.All(cv.hex_int_range(max=63), cv.uint32_t),
        cv.Required(CONF_DESTINATION): cv.All(cv.hex_int_range(max=31), cv.uint8_t),
        cv.Optional(CONF_DESTINATION_BUS, default=0x08): cv.All(cv.hex_int_range(max=15), cv.uint8_t),
        cv.Required(CONF_SOURCE): cv.All(cv.hex_int_range(max=31), cv.uint8_t),
        cv.Optional(CONF_SOURCE_BUS, default=0x08): cv.All(cv.hex_int_range(max=15), cv.uint8_t),
    }
)


def compute_message_raw(msg_config):
    prolog = 0b010
    middle = 0b010
    epilog = 0b00
    return (
        (prolog                                       << 29) |
        ((msg_config[CONF_COMMAND]         & 0x3F)    << 23) |
        ((msg_config[CONF_DESTINATION]     & 0x1F)    << 18) |
        ((msg_config[CONF_DESTINATION_BUS] & 0x0F)    << 14) |
        (middle                                       << 11) |
        ((msg_config[CONF_SOURCE]          & 0x1F)    <<  6) |
        ((msg_config[CONF_SOURCE_BUS]      & 0x0F)    <<  2) |
        epilog
    )


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SiedleInHomeBusComponent),
        cv.Required(CONF_CARRIER_PIN): pins.internal_gpio_input_pin_schema,
        cv.Required(CONF_RX_PIN): pins.internal_gpio_input_pin_schema,
        cv.Required(CONF_LOAD_PIN): pins.internal_gpio_output_pin_schema,
        cv.Required(CONF_TX_PIN): pins.internal_gpio_output_pin_schema,
        cv.Optional(CONF_DUMP, default=True): cv.boolean,
        cv.Optional(CONF_ON_MESSAGE): automation.validate_automation({}),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    cg.add_global(siedle_in_home_bus_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_CARRIER_PIN])
    cg.add(var.set_carrier_pin(pin))
    pin = await cg.gpio_pin_expression(config[CONF_RX_PIN])
    cg.add(var.set_rx_pin(pin))
    pin = await cg.gpio_pin_expression(config[CONF_LOAD_PIN])
    cg.add(var.set_load_pin(pin))
    pin = await cg.gpio_pin_expression(config[CONF_TX_PIN])
    cg.add(var.set_tx_pin(pin))
    cg.add(var.set_dump(config[CONF_DUMP]))

    for conf in config.get(CONF_ON_MESSAGE, []):
        await automation.build_callback_automation(
            var, "add_on_message_callback", [(SiedleInHomeBusMessage, "msg")], conf
        )


def _validate_send(config):
    has_raw = CONF_RAW in config
    has_command = CONF_COMMAND in config
    if not has_raw and not has_command:
        raise cv.Invalid("Either 'raw' or 'command' (with 'destination' and 'source') must be set.")
    if has_raw and has_command:
        raise cv.Invalid("'raw' cannot be combined with 'command'.")
    if has_command and CONF_DESTINATION not in config:
        raise cv.Invalid("'destination' is required when 'command' is set.")
    if has_command and CONF_SOURCE not in config:
        raise cv.Invalid("'source' is required when 'command' is set.")
    return config


SIEDLE_SEND_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(SiedleInHomeBusComponent),
            cv.Optional(CONF_RAW): cv.templatable(cv.hex_uint32_t),
            cv.Optional(CONF_COMMAND): cv.templatable(cv.hex_uint8_t),
            cv.Optional(CONF_DESTINATION): cv.templatable(cv.hex_uint8_t),
            cv.Optional(CONF_DESTINATION_BUS, default=0x08): cv.templatable(cv.hex_uint8_t),
            cv.Optional(CONF_SOURCE): cv.templatable(cv.hex_uint8_t),
            cv.Optional(CONF_SOURCE_BUS, default=0x08): cv.templatable(cv.hex_uint8_t),
        }
    ),
    _validate_send,
)


@automation.register_action(
    "siedle_in_home_bus.send",
    SiedleInHomeBusSendAction,
    SIEDLE_SEND_SCHEMA,
    synchronous=True,
)
async def siedle_in_home_bus_send_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    if (raw := config.get(CONF_RAW)) is not None:
        cg.add(var.set_raw(await cg.templatable(raw, args, cg.uint32)))
    else:
        cg.add(var.set_command(await cg.templatable(config[CONF_COMMAND], args, cg.uint8)))
        cg.add(var.set_destination(await cg.templatable(config[CONF_DESTINATION], args, cg.uint8)))
        cg.add(var.set_destination_bus(await cg.templatable(config[CONF_DESTINATION_BUS], args, cg.uint8)))
        cg.add(var.set_source(await cg.templatable(config[CONF_SOURCE], args, cg.uint8)))
        cg.add(var.set_source_bus(await cg.templatable(config[CONF_SOURCE_BUS], args, cg.uint8)))

    return var
