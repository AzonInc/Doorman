import esphome.codegen as cg
from esphome.components import number
from esphome.components.number import NUMBER_MODES
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    CONF_MODE,
)
from .. import CONF_TC_BUS_ID, TCBusComponent, tc_bus_ns

EntranceAddressNumber = tc_bus_ns.class_("EntranceAddressNumber", number.Number, cg.Component)
SecondEntranceAddressNumber = tc_bus_ns.class_("SecondEntranceAddressNumber", number.Number, cg.Component)

CONF_ENTRANCE_ADDRESS = "entrance_address"
CONF_SECOND_ENTRANCE_ADDRESS = "second_entrance_address"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_ENTRANCE_ADDRESS): number.number_schema(
            EntranceAddressNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_SECOND_ENTRANCE_ADDRESS): number.number_schema(
            SecondEntranceAddressNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
    }
)


async def to_code(config):
    tc_bus_component = await cg.get_variable(config[CONF_TC_BUS_ID])

    if entrance_address := config.get(CONF_ENTRANCE_ADDRESS):
        n = await number.new_number(
            entrance_address, min_value=0, max_value=63, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_ID])
        cg.add(tc_bus_component.set_entrance_address_number(n))

    if second_entrance_address := config.get(CONF_SECOND_ENTRANCE_ADDRESS):
        n = await number.new_number(
            second_entrance_address, min_value=0, max_value=63, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_ID])
        cg.add(tc_bus_component.set_second_entrance_address_number(n))