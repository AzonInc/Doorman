import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_MESSAGE
from .. import (
    SiedleInHomeBusComponent,
    siedle_in_home_bus_ns,
    CONF_SIEDLE_IN_HOME_BUS_ID,
    CONFIG_MESSAGE_SCHEMA,
    compute_message_raw,
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
    parent = await cg.get_variable(config[CONF_SIEDLE_IN_HOME_BUS_ID])
    raw = compute_message_raw(config[CONF_MESSAGE])
    var = cg.new_Pvariable(config[CONF_ID], parent, raw)
    await button.register_button(var, config)
