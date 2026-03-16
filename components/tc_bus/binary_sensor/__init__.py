import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_ICON, ENTITY_CATEGORY_DIAGNOSTIC
from .. import tc_bus_ns, TCBusComponent, CONF_TC_BUS_ID, TELEGRAM_TYPES

BusTelegramListenerBinarySensor = tc_bus_ns.class_("BusTelegramListenerBinarySensor", binary_sensor.BinarySensor, cg.Component)
DoorReadinessBinarySensor = tc_bus_ns.class_("DoorReadinessBinarySensor", binary_sensor.BinarySensor, cg.Component)

CONF_TELEGRAM = "telegram"
CONF_TYPE = "type"
CONF_ADDRESS = "address"
CONF_PAYLOAD = "payload"
CONF_SERIAL_NUMBER = "serial_number"
CONF_NAME = "name"
CONF_AUTO_OFF = "auto_off"
CONF_SENSOR_TYPE = "sensor_type"
CONF_DOOR_READINESS = "door_readiness"

DEPENDENCIES = ["tc_bus"]

def validate_telegram_sensor(config):
    has_telegram = CONF_TELEGRAM in config
    has_type = CONF_TYPE in config
    has_address = CONF_ADDRESS in config
    has_payload = CONF_PAYLOAD in config
    has_serial_number = CONF_SERIAL_NUMBER in config

    if not has_telegram and not has_type:
        raise cv.Invalid("Either 'telegram' or 'type' must be set.")

    if has_telegram and any([has_type, has_address, has_payload, has_serial_number]):
        raise cv.Invalid("'telegram' cannot be combined with 'type', 'address', 'payload', or 'serial_number'.")

    return config

TELEGRAM_SCHEMA = cv.All(
    binary_sensor.binary_sensor_schema(BusTelegramListenerBinarySensor).extend(
        {
            cv.GenerateID(): cv.declare_id(BusTelegramListenerBinarySensor),
            cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
            cv.Optional(CONF_TELEGRAM): cv.hex_uint32_t,
            cv.Optional(CONF_TYPE): cv.enum(TELEGRAM_TYPES, upper=False),
            cv.Optional(CONF_ADDRESS): cv.templatable(cv.hex_uint8_t),
            cv.Optional(CONF_PAYLOAD): cv.templatable(cv.hex_uint32_t),
            cv.Optional(CONF_SERIAL_NUMBER): cv.templatable(cv.hex_uint32_t),
            cv.Optional(CONF_ICON, default="mdi:doorbell"): cv.icon,
            cv.Optional(CONF_NAME, default="Doorbell"): cv.string,
            cv.Optional(CONF_AUTO_OFF, default="3s"): cv.positive_time_period_milliseconds,
        }
    ),
    validate_telegram_sensor,
)

DOOR_READINESS_SCHEMA = binary_sensor.binary_sensor_schema(
    DoorReadinessBinarySensor,
    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    icon="mdi:account-badge",
).extend({
    cv.GenerateID(): cv.declare_id(DoorReadinessBinarySensor),
    cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
})

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_TELEGRAM: TELEGRAM_SCHEMA,
        CONF_DOOR_READINESS: DOOR_READINESS_SCHEMA,
    },
    key=CONF_SENSOR_TYPE,
    default_type=CONF_TELEGRAM
)

async def to_code(config):
    sensor_type = config[CONF_SENSOR_TYPE]
    tc_bus = await cg.get_variable(config[CONF_TC_BUS_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)

    if sensor_type == CONF_TELEGRAM:
        if CONF_TELEGRAM in config:
            telegram = await cg.templatable(config[CONF_TELEGRAM], [], cg.uint32)
            cg.add(var.set_telegram(telegram))

        if CONF_TYPE in config:
            cg.add(var.set_type(config[CONF_TYPE]))

        if CONF_ADDRESS in config:
            telegram_address = await cg.templatable(config[CONF_ADDRESS], [], cg.uint8)
            cg.add(var.set_address(telegram_address))

        if CONF_PAYLOAD in config:
            telegram_payload = await cg.templatable(config[CONF_PAYLOAD], [], cg.uint32)
            cg.add(var.set_payload(telegram_payload))

        if CONF_SERIAL_NUMBER in config:
            telegram_serial_number = await cg.templatable(config[CONF_SERIAL_NUMBER], [], cg.uint32)
            cg.add(var.set_serial_number(telegram_serial_number))

        cg.add(var.set_auto_off(config[CONF_AUTO_OFF]))
        cg.add(tc_bus.register_listener(var))

    elif sensor_type == CONF_DOOR_READINESS:
        cg.add(tc_bus.set_door_readiness_binary_sensor(var))