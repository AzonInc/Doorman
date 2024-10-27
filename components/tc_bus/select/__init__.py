import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
)
from .. import CONF_TC_ID, CONF_MODELS, CONF_RINGTONES, TCBusComponent, tc_bus_ns

ModelSelect = tc_bus_ns.class_("ModelSelect", select.Select, cg.Component)
RingtoneDoorCallSelect = tc_bus_ns.class_("RingtoneDoorCallSelect", select.Select, cg.Component)
RingtoneFloorCallSelect = tc_bus_ns.class_("RingtoneFloorCallSelect", select.Select, cg.Component)
RingtoneInternalCallSelect = tc_bus_ns.class_("RingtoneInternalCallSelect", select.Select, cg.Component)

CONF_MODEL = "model"
CONF_RINGTONE_DOOR_CALL = "ringtone_door_call"
CONF_RINGTONE_FLOOR_CALL = "ringtone_floor_call"
CONF_RINGTONE_INTERNAL_CALL = "ringtone_internal_call"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_MODEL): select.select_schema(
            ModelSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:doorbell-video"
        ),
        cv.Optional(CONF_RINGTONE_DOOR_CALL): select.select_schema(
            RingtoneDoorCallSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:music"
        ),
        cv.Optional(CONF_RINGTONE_FLOOR_CALL): select.select_schema(
            RingtoneFloorCallSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:music"
        ),
        cv.Optional(CONF_RINGTONE_INTERNAL_CALL): select.select_schema(
            RingtoneInternalCallSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:music"
        ),
    }
)


async def to_code(config):
    tc_bus_component = await cg.get_variable(config[CONF_TC_ID])

    if model := config.get(CONF_MODEL):
        sel = await select.new_select(
            model,
            options=[CONF_MODELS],
        )
        await cg.register_parented(sel, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_model_select(sel))

    if ringtone_door_call := config.get(CONF_RINGTONE_DOOR_CALL):
        sel = await select.new_select(
            ringtone_door_call,
            options=[CONF_RINGTONES],
        )
        await cg.register_parented(sel, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_ringtone_door_call_select(sel))

    if ringtone_floor_call := config.get(CONF_RINGTONE_FLOOR_CALL):
        sel = await select.new_select(
            ringtone_floor_call,
            options=[CONF_RINGTONES],
        )
        await cg.register_parented(sel, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_ringtone_floor_call_select(sel))

    if ringtone_internal_call := config.get(CONF_RINGTONE_INTERNAL_CALL):
        sel = await select.new_select(
            ringtone_internal_call,
            options=[CONF_RINGTONES],
        )
        await cg.register_parented(sel, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_ringtone_internal_call_select(sel))