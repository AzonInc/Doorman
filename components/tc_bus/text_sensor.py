import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import CONF_TC_ID, TCBusComponent

DEPENDENCIES = ["tc_bus"]

CONF_BUS_COMMAND = "bus_command"
CONF_HARDWARE_VERSION = "hardware_version"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_TC_ID): cv.use_id(TCBusComponent),
    cv.Optional(CONF_BUS_COMMAND): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:console-network",
    ),
    cv.Optional(CONF_HARDWARE_VERSION): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:router-network-wireless",
    ),
}


async def to_code(config):
    tc_bus_component = await cg.get_variable(config[CONF_TC_ID])

    if bus_command := config.get(CONF_BUS_COMMAND):
        sens = await text_sensor.new_text_sensor(bus_command)
        cg.add(tc_bus_component.set_bus_command_text_sensor(sens))

    if hardware_version := config.get(CONF_HARDWARE_VERSION):
        sens = await text_sensor.new_text_sensor(hardware_version)
        cg.add(tc_bus_component.set_hardware_version_text_sensor(sens))