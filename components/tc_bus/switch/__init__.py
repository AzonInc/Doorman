import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_SWITCH
)
from .. import CONF_TC_ID, TCBusComponent, tc_bus_ns

ForceLongDoorOpenerSwitch = tc_bus_ns.class_("ForceLongDoorOpenerSwitch", switch.Switch, cg.Component)

CONF_FORCE_LONG_DOOR_OPENER = "force_long_door_opener"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_FORCE_LONG_DOOR_OPENER): switch.switch_schema(
            ForceLongDoorOpenerSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:flash-alert"
        ),
    }
)

async def to_code(config):
    tc_bus_component = await cg.get_variable(config[CONF_TC_ID])

    if force_long_door_opener := config.get(CONF_FORCE_LONG_DOOR_OPENER):
        s = await switch.new_switch(force_long_door_opener)
        await cg.register_parented(s, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_force_long_door_opener_switch(s))