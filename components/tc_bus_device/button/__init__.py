import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_IDENTIFY
)
from .. import CONF_TC_BUS_DEVICE_ID, TCBusDeviceComponent, tc_bus_ns

IdentifyDeviceButton = tc_bus_ns.class_("IdentifyDeviceButton", button.Button, cg.Component)
ReadMemoryButton = tc_bus_ns.class_("ReadMemoryButton", button.Button, cg.Component)

CONF_IDENTIFY_DEVICE = "identify_device"
CONF_READ_MEMORY = "read_memory"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_DEVICE_ID): cv.use_id(TCBusDeviceComponent),
        cv.Optional(CONF_IDENTIFY_DEVICE): button.button_schema(
            IdentifyDeviceButton,
            device_class=DEVICE_CLASS_IDENTIFY,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:identifier"
        ),
        cv.Optional(CONF_READ_MEMORY): button.button_schema(
            ReadMemoryButton,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:file-arrow-up-down"
        ),
    }
)

async def to_code(config):
    tc_bus_device_component = await cg.get_variable(config[CONF_TC_BUS_DEVICE_ID])

    if identify_device_button := config.get(CONF_IDENTIFY_DEVICE):
        s = await button.new_button(identify_device_button)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_identify_device_button(s))

    if read_memory_button := config.get(CONF_READ_MEMORY):
        s = await button.new_button(read_memory_button)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_read_memory_button(s))
