import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_MESSAGE
from .. import (
    SiedleInHomeBusComponent,
    siedle_in_home_bus_ns,
    CONF_SIEDLE_IN_HOME_BUS_ID,
    CONFIG_MESSAGE_SCHEMA,
    compute_message_raw,
)

DEPENDENCIES = ["siedle_in_home_bus"]

SiedleInHomeBusBinarySensor = siedle_in_home_bus_ns.class_(
    "SiedleInHomeBusBinarySensor", binary_sensor.BinarySensor
)

CONF_AUTO_OFF = "auto_off"

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(SiedleInHomeBusBinarySensor).extend(
    {
        cv.GenerateID(CONF_SIEDLE_IN_HOME_BUS_ID): cv.use_id(SiedleInHomeBusComponent),
        cv.Required(CONF_MESSAGE): CONFIG_MESSAGE_SCHEMA,
        cv.Optional(CONF_AUTO_OFF, default="3s"): cv.positive_time_period_milliseconds,
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_SIEDLE_IN_HOME_BUS_ID])
    var = await binary_sensor.new_binary_sensor(config)
    cg.add(var.set_message_raw(compute_message_raw(config[CONF_MESSAGE])))
    cg.add(var.set_auto_reset_ms(config[CONF_AUTO_OFF]))
    cg.add(parent.register_listener(var))
