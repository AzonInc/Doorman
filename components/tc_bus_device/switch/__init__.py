import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_SWITCH
)
from .. import CONF_TC_BUS_DEVICE_ID, TCBusDeviceComponent, tc_bus_ns

ForceLongDoorOpenerProtocolSwitch = tc_bus_ns.class_("ForceLongDoorOpenerProtocolSwitch", switch.Switch, cg.Component)
RingtoneMuteSwitch = tc_bus_ns.class_("RingtoneMuteSwitch", switch.Switch, cg.Component)
AutoAnswerCallSwitch = tc_bus_ns.class_("AutoAnswerCallSwitch", switch.Switch, cg.Component)

CONF_FORCE_LONG_DOOR_OPENER_PROTOCOL = "force_long_door_opener_protocol"
CONF_RINGTONE_MUTE = "ringtone_mute"
CONF_AUTO_ANSWER_CALL = "auto_answer_call"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_DEVICE_ID): cv.use_id(TCBusDeviceComponent),
        cv.Optional(CONF_FORCE_LONG_DOOR_OPENER_PROTOCOL): switch.switch_schema(
            ForceLongDoorOpenerProtocolSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:flash-alert"
        ),
        cv.Optional(CONF_RINGTONE_MUTE): switch.switch_schema(
            RingtoneMuteSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:mute"
        ),
        cv.Optional(CONF_AUTO_ANSWER_CALL): switch.switch_schema(
            AutoAnswerCallSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:todo"
        ),
    }
)

async def to_code(config):
    tc_bus_device_component = await cg.get_variable(config[CONF_TC_BUS_DEVICE_ID])

    if force_long_door_opener_protocol := config.get(CONF_FORCE_LONG_DOOR_OPENER_PROTOCOL):
        s = await switch.new_switch(force_long_door_opener_protocol)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_force_long_door_opener_protocol_switch(s))

    if ringtone_mute := config.get(CONF_RINGTONE_MUTE):
        s = await switch.new_switch(ringtone_mute)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_ringtone_mute_switch(s))

    if auto_answer_call := config.get(CONF_AUTO_ANSWER_CALL):
        s = await switch.new_switch(auto_answer_call)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_auto_answer_call_switch(s))