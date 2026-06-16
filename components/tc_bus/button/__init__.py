import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_IDENTIFY
)
from .. import CONF_TC_BUS_ID, TCBusComponent, tc_bus_ns

SystemDiscoveryButton = tc_bus_ns.class_("SystemDiscoveryButton", button.Button, cg.Component)

CONF_SYSTEM_DISCOVERY = "system_discovery"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_SYSTEM_DISCOVERY): button.button_schema(
            SystemDiscoveryButton,
            device_class=DEVICE_CLASS_IDENTIFY,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:identifier"
        ),
    }
)

async def to_code(config):
    tc_bus_component = await cg.get_variable(config[CONF_TC_BUS_ID])

    if system_discovery_button := config.get(CONF_SYSTEM_DISCOVERY):
        s = await button.new_button(system_discovery_button)
        await cg.register_parented(s, config[CONF_TC_BUS_ID])
        cg.add(tc_bus_component.set_system_discovery_button(s))