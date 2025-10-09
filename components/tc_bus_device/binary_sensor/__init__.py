import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_ICON, CONF_TYPE
from .. import tc_bus_ns, TCBusDeviceComponent, CONF_TC_BUS_DEVICE_ID
from ...tc_bus import TELEGRAM_TYPES

DeviceTelegramListenerBinarySensor = tc_bus_ns.class_(
    "DeviceTelegramListenerBinarySensor",
    binary_sensor.BinarySensor,
    cg.Component
)

CONF_ADDRESS = "address"
CONF_ADDRESS_LAMBDA = "address_lambda"

CONF_PAYLOAD = "payload"
CONF_PAYLOAD_LAMBDA = "payload_lambda"

CONF_NAME = "name"
CONF_AUTO_OFF = "auto_off"

DEPENDENCIES = ["tc_bus"]

def validate(config):
    config = config.copy()

    if CONF_ADDRESS in config and CONF_ADDRESS_LAMBDA in config:
        raise cv.Invalid("You can either set ADDRESS or ADDRESS_LAMBDA, not both.")

    if CONF_PAYLOAD in config and CONF_PAYLOAD_LAMBDA in config:
        raise cv.Invalid("You can either set PAYLOAD or PAYLOAD_LAMBDA, not both.")

    return config

CONFIG_SCHEMA = cv.All(
    binary_sensor.binary_sensor_schema(DeviceTelegramListenerBinarySensor).extend(
        {
            cv.GenerateID(): cv.declare_id(DeviceTelegramListenerBinarySensor),
            cv.GenerateID(CONF_TC_BUS_DEVICE_ID): cv.use_id(TCBusDeviceComponent),

            cv.Required(CONF_TYPE): cv.enum(TELEGRAM_TYPES, upper=False),
            cv.Optional(CONF_ADDRESS): cv.hex_uint8_t,
            cv.Optional(CONF_ADDRESS_LAMBDA): cv.returning_lambda,
            cv.Optional(CONF_PAYLOAD): cv.hex_uint32_t,
            cv.Optional(CONF_PAYLOAD_LAMBDA): cv.returning_lambda,

            cv.Optional(CONF_ICON, default="mdi:doorbell"): cv.icon,
            cv.Optional(CONF_NAME, default="Doorbell"): cv.string,
            cv.Optional(CONF_AUTO_OFF, default="3s"): cv.positive_time_period_milliseconds
        }
    ),
    validate,
)

async def new_binary_sensor(config, *args):
    var = cg.new_Pvariable(config[CONF_ID], *args)
    await binary_sensor.register_binary_sensor(var, config)
    return var

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)

    if CONF_TYPE in config:
        cg.add(var.set_telegram_type(config[CONF_TYPE]))

    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))

    if CONF_ADDRESS_LAMBDA in config:
        address_template_ = await cg.process_lambda(
            config[CONF_ADDRESS_LAMBDA], [], return_type=cg.optional.template(cg.uint8)
        )
        cg.add(var.set_address_lambda(address_template_))

    if CONF_PAYLOAD in config:
        cg.add(var.set_payload(config[CONF_PAYLOAD]))

    if CONF_PAYLOAD_LAMBDA in config:
        payload_template_ = await cg.process_lambda(
            config[CONF_PAYLOAD_LAMBDA], [], return_type=cg.optional.template(cg.uint32)
        )
        cg.add(var.set_payload_lambda(payload_template_))
    
    cg.add(var.set_auto_off(config[CONF_AUTO_OFF]))

    tc_bus_device = await cg.get_variable(config[CONF_TC_BUS_DEVICE_ID])
    cg.add(tc_bus_device.register_listener(var))
