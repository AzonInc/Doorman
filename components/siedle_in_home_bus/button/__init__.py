import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
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

SiedleInHomeBusButton = siedle_in_home_bus_ns.class_(
    "SiedleInHomeBusButton", button.Button
)

CONFIG_SCHEMA = button.button_schema(SiedleInHomeBusButton).extend(
    {
        cv.GenerateID(CONF_SIEDLE_IN_HOME_BUS_ID): cv.use_id(SiedleInHomeBusComponent),
        cv.Required(CONF_COMMAND): cv.templatable(cv.hex_uint8_t),
        cv.Required(CONF_DESTINATION): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_DESTINATION_BUS, default=0x08): cv.templatable(cv.hex_uint8_t),
        cv.Required(CONF_SOURCE): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_SOURCE_BUS, default=0x08): cv.templatable(cv.hex_uint8_t),
    }
)


async def to_code(config):
    var = await button.new_button(config)
    await cg.register_parented(var, config[CONF_SIEDLE_IN_HOME_BUS_ID])
    cg.add(var.set_command(await cg.templatable(config[CONF_COMMAND], [], cg.uint8)))
    cg.add(var.set_destination(await cg.templatable(config[CONF_DESTINATION], [], cg.uint8)))
    cg.add(var.set_destination_bus(await cg.templatable(config[CONF_DESTINATION_BUS], [], cg.uint8)))
    cg.add(var.set_source(await cg.templatable(config[CONF_SOURCE], [], cg.uint8)))
    cg.add(var.set_source_bus(await cg.templatable(config[CONF_SOURCE_BUS], [], cg.uint8)))
