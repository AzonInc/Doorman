import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TYPE
from esphome.core import CORE
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
)
from .. import CONF_TC_BUS_DEVICE, CONF_TC_BUS_DEVICE_ID, CONF_MODEL_IS, CONF_MODEL_AS, CONF_MODEL_CTRL, CONF_MODEL_EXT, CONF_RINGTONES, TCBusDeviceComponent, tc_bus_ns

ModelSelect = tc_bus_ns.class_("ModelSelect", select.Select, cg.Component)
RingtoneEntranceDoorCallSelect = tc_bus_ns.class_("RingtoneEntranceDoorCallSelect", select.Select, cg.Component)
RingtoneSecondEntranceDoorCallSelect = tc_bus_ns.class_("RingtoneSecondEntranceDoorCallSelect", select.Select, cg.Component)
RingtoneFloorCallSelect = tc_bus_ns.class_("RingtoneFloorCallSelect", select.Select, cg.Component)
RingtoneInternalCallSelect = tc_bus_ns.class_("RingtoneInternalCallSelect", select.Select, cg.Component)

CONF_MODEL = "model"
CONF_RINGTONE_ENTRANCE_DOOR_CALL = "ringtone_entrance_door_call"
CONF_RINGTONE_SECOND_ENTRANCE_DOOR_CALL = "ringtone_second_entrance_door_call"
CONF_RINGTONE_FLOOR_CALL = "ringtone_floor_call"
CONF_RINGTONE_INTERNAL_CALL = "ringtone_internal_call"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_DEVICE_ID): cv.use_id(TCBusDeviceComponent),
        cv.Optional(CONF_MODEL): select.select_schema(
            ModelSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:doorbell-video"
        ),
        cv.Optional(CONF_RINGTONE_ENTRANCE_DOOR_CALL): select.select_schema(
            RingtoneEntranceDoorCallSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:music"
        ),
        cv.Optional(CONF_RINGTONE_SECOND_ENTRANCE_DOOR_CALL): select.select_schema(
            RingtoneSecondEntranceDoorCallSelect,
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
    tc_bus_device_component = await cg.get_variable(config[CONF_TC_BUS_DEVICE_ID])

    # Get device group from config entry
    device_group = ""
    tc_bus_devices = CORE.config.get(CONF_TC_BUS_DEVICE)
    if tc_bus_devices is not None:
        for tc_bus_device in tc_bus_devices:
            if tc_bus_device.get(CONF_ID) == config[CONF_TC_BUS_DEVICE_ID]:
                device_group = tc_bus_device.get(CONF_TYPE, "")

    # Map device group models
    group_to_options = {
        "indoor_station": CONF_MODEL_IS,
        "outdoor_station": CONF_MODEL_AS,
        "controller": CONF_MODEL_CTRL,
        "extension": CONF_MODEL_EXT,
    }
    model_options = ["None"] + group_to_options.get(device_group, [])

    if model := config.get(CONF_MODEL):
        sel = await select.new_select(
            model,
            options=model_options,
        )
        await cg.register_parented(sel, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_model_select(sel))

    if ringtone_entrance_door_call := config.get(CONF_RINGTONE_ENTRANCE_DOOR_CALL):
        sel = await select.new_select(
            ringtone_entrance_door_call,
            options=[CONF_RINGTONES],
        )
        await cg.register_parented(sel, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_ringtone_entrance_door_call_select(sel))

    if ringtone_second_entrance_door_call := config.get(CONF_RINGTONE_SECOND_ENTRANCE_DOOR_CALL):
        sel = await select.new_select(
            ringtone_second_entrance_door_call,
            options=[CONF_RINGTONES],
        )
        await cg.register_parented(sel, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_ringtone_second_entrance_door_call_select(sel))

    if ringtone_floor_call := config.get(CONF_RINGTONE_FLOOR_CALL):
        sel = await select.new_select(
            ringtone_floor_call,
            options=[CONF_RINGTONES],
        )
        await cg.register_parented(sel, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_ringtone_floor_call_select(sel))

    if ringtone_internal_call := config.get(CONF_RINGTONE_INTERNAL_CALL):
        sel = await select.new_select(
            ringtone_internal_call,
            options=[CONF_RINGTONES],
        )
        await cg.register_parented(sel, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_ringtone_internal_call_select(sel))