import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_MESSAGE
from .. import (
    SiedleInHomeBusComponent,
    siedle_in_home_bus_ns,
    CONF_SIEDLE_IN_HOME_BUS_ID,
    CONFIG_MESSAGE_SCHEMA,
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
        cv.Required(CONF_MESSAGE): CONFIG_MESSAGE_SCHEMA,
    }
)


async def to_code(config):
    var = await button.new_button(config)
    await cg.register_parented(var, config[CONF_SIEDLE_IN_HOME_BUS_ID])
    msg = config[CONF_MESSAGE]
    cg.add(var.set_command(msg[CONF_COMMAND]))
    cg.add(var.set_destination(msg[CONF_DESTINATION]))
    cg.add(var.set_destination_bus(msg[CONF_DESTINATION_BUS]))
    cg.add(var.set_source(msg[CONF_SOURCE]))
    cg.add(var.set_source_bus(msg[CONF_SOURCE_BUS]))
