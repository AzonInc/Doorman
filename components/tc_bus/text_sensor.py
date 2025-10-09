import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import CONF_TC_BUS_ID, TCBusComponent

DEPENDENCIES = ["tc_bus"]

CONF_BUS_TELEGRAM = "bus_telegram"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
    cv.Optional(CONF_BUS_TELEGRAM): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:console-network",
    ),
}

async def to_code(config):
    tc_bus_component = await cg.get_variable(config[CONF_TC_BUS_ID])

    if bus_telegram := config.get(CONF_BUS_TELEGRAM):
        sens = await text_sensor.new_text_sensor(bus_telegram)
        cg.add(tc_bus_component.set_bus_telegram_text_sensor(sens))