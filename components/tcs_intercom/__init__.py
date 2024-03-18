import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins, automation
from esphome.const import CONF_ID, CONF_FILTER, CONF_IDLE, CONF_BUFFER_SIZE
from esphome.core import coroutine_with_priority, TimePeriod

CODEOWNERS = ["@azoninc"]
tcs_intercom_ns = cg.esphome_ns.namespace("tcs_intercom")
TCSIntercom = tcs_intercom_ns.class_("TCSComponent", cg.Component)

TCSIntercomSendAction = tcs_intercom_ns.class_(
    "TCSIntercomSendAction", automation.Action
)

CONF_TCS_ID = "tcs_intercom"

CONF_RX_PIN = "rx_pin"
CONF_TX_PIN = "tx_pin"

CONF_EVENT = "event"
CONF_COMMAND = "command"

MULTI_CONF = False

def validate_config(config):
    return config

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TCSIntercom),
            cv.Optional(CONF_RX_PIN, default=12): pins.internal_gpio_input_pin_schema,
            cv.Optional(CONF_TX_PIN, default=5): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_EVENT, default="tcs"): cv.string,
        }   
    )
    .extend(cv.COMPONENT_SCHEMA),
    validate_config,
)

@coroutine_with_priority(1.0)
async def to_code(config):
    cg.add_global(tcs_intercom_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    rx_pin = await cg.gpio_pin_expression(config[CONF_RX_PIN])
    cg.add(var.set_rx_pin(rx_pin))

    pin = await cg.gpio_pin_expression(config[CONF_TX_PIN])
    cg.add(var.set_tx_pin(pin))

    cg.add(var.set_event("esphome." + config[CONF_EVENT]))

TCS_INTERCOM_SEND_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(TCSIntercom),
        cv.Required(CONF_COMMAND): cv.templatable(cv.hex_uint32_t),
    }
)

@automation.register_action(
    "tcs_intercom.send", TCSIntercomSendAction, TCS_INTERCOM_SEND_SCHEMA
)
async def tcs_intercom_send_to_code(config, action_id, template_args, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_args, paren)
    template_ = await cg.templatable(config[CONF_COMMAND], args, cg.uint32)
    cg.add(var.set_command(template_))
    return var