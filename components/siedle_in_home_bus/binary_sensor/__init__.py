import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID
from .. import (
    SiedleInHomeBusComponent,
    siedle_in_home_bus_ns,
    CONF_SIEDLE_IN_HOME_BUS_ID,
    CONF_COMMAND,
    CONF_DESTINATION,
    CONF_DESTINATION_BUS,
    CONF_SOURCE,
    CONF_SOURCE_BUS,
)

DEPENDENCIES = ["siedle_in_home_bus"]

SiedleInHomeBusBinarySensor = siedle_in_home_bus_ns.class_(
    "SiedleInHomeBusBinarySensor", binary_sensor.BinarySensor
)

CONF_AUTO_OFF = "auto_off"

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(SiedleInHomeBusBinarySensor).extend(
    {
        cv.GenerateID(CONF_SIEDLE_IN_HOME_BUS_ID): cv.use_id(SiedleInHomeBusComponent),
        cv.Optional(CONF_COMMAND): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_DESTINATION): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_DESTINATION_BUS): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_SOURCE): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_SOURCE_BUS): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_AUTO_OFF, default="3s"): cv.positive_time_period_milliseconds,
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_SIEDLE_IN_HOME_BUS_ID])
    var = await binary_sensor.new_binary_sensor(config)
    if (v := config.get(CONF_COMMAND)) is not None:
        cg.add(var.set_command(await cg.templatable(v, [], cg.uint8)))
    if (v := config.get(CONF_DESTINATION)) is not None:
        cg.add(var.set_destination(await cg.templatable(v, [], cg.uint8)))
    if (v := config.get(CONF_DESTINATION_BUS)) is not None:
        cg.add(var.set_destination_bus(await cg.templatable(v, [], cg.uint8)))
    if (v := config.get(CONF_SOURCE)) is not None:
        cg.add(var.set_source(await cg.templatable(v, [], cg.uint8)))
    if (v := config.get(CONF_SOURCE_BUS)) is not None:
        cg.add(var.set_source_bus(await cg.templatable(v, [], cg.uint8)))
    cg.add(var.set_auto_off_ms(config[CONF_AUTO_OFF]))
    cg.add(parent.register_listener(var))
