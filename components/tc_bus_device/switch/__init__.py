import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_SWITCH
)
from .. import CONF_TC_BUS_DEVICE_ID, TCBusDeviceComponent, tc_bus_ns

UseLongDoorOpenerProtocolSwitch = tc_bus_ns.class_("UseLongDoorOpenerProtocolSwitch", switch.Switch, cg.Component)
RingtoneMuteSwitch = tc_bus_ns.class_("RingtoneMuteSwitch", switch.Switch, cg.Component)
AutoAnswerCallSwitch = tc_bus_ns.class_("AutoAnswerCallSwitch", switch.Switch, cg.Component)
CallingRequiresDoorReadinessSwitch = tc_bus_ns.class_("CallingRequiresDoorReadinessSwitch", switch.Switch, cg.Component)
DoorOpenerRequiresActiveCall = tc_bus_ns.class_("DoorOpenerRequiresActiveCall", switch.Switch, cg.Component)
DoorOpenerRequiresDoorReadiness = tc_bus_ns.class_("DoorOpenerRequiresDoorReadiness", switch.Switch, cg.Component)
AddressLockSwitch = tc_bus_ns.class_("AddressLockSwitch", switch.Switch, cg.Component)
CallTimeUnlimitedSwitch = tc_bus_ns.class_("CallTimeUnlimitedSwitch", switch.Switch, cg.Component)

CONF_FORCE_LONG_DOOR_OPENER_PROTOCOL = "force_long_door_opener_protocol"
CONF_RINGTONE_MUTE = "ringtone_mute"
CONF_AUTO_ANSWER_CALL = "auto_answer_call"
CONF_CALLING_REQUIRES_DOOR_READINESS = "calling_requires_door_readiness"
CONF_DOOR_OPENER_REQUIRES_ACTIVE_CALL = "door_opener_requires_active_call"
CONF_DOOR_OPENER_REQUIRES_DOOR_READINESS = "door_opener_requires_door_readiness"
CONF_ADDRESS_LOCK = "address_lock"
CONF_CALL_TIME_UNLIMITED = "call_time_unlimited"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_DEVICE_ID): cv.use_id(TCBusDeviceComponent),
        cv.Optional(CONF_FORCE_LONG_DOOR_OPENER_PROTOCOL): switch.switch_schema(
            UseLongDoorOpenerProtocolSwitch,
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
        cv.Optional(CONF_CALLING_REQUIRES_DOOR_READINESS): switch.switch_schema(
            CallingRequiresDoorReadinessSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:todo"
        ),
        cv.Optional(CONF_DOOR_OPENER_REQUIRES_ACTIVE_CALL): switch.switch_schema(
            DoorOpenerRequiresActiveCall,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:todo"
        ),
        cv.Optional(CONF_DOOR_OPENER_REQUIRES_DOOR_READINESS): switch.switch_schema(
            DoorOpenerRequiresDoorReadiness,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:todo"
        ),
        cv.Optional(CONF_ADDRESS_LOCK): switch.switch_schema(
            AddressLockSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:lock"
        ),
        cv.Optional(CONF_CALL_TIME_UNLIMITED): switch.switch_schema(
            CallTimeUnlimitedSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:timer-off"
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

    if calling_requires_door_readiness := config.get(CONF_CALLING_REQUIRES_DOOR_READINESS):
        s = await switch.new_switch(calling_requires_door_readiness)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_calling_requires_door_readiness_switch(s))

    if door_opener_requires_active_call := config.get(CONF_DOOR_OPENER_REQUIRES_ACTIVE_CALL):
        s = await switch.new_switch(door_opener_requires_active_call)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_door_opener_requires_active_call_switch(s))

    if door_opener_requires_door_readiness := config.get(CONF_DOOR_OPENER_REQUIRES_DOOR_READINESS):
        s = await switch.new_switch(door_opener_requires_door_readiness)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_door_opener_requires_door_readiness_switch(s))

    if address_lock := config.get(CONF_ADDRESS_LOCK):
        s = await switch.new_switch(address_lock)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_address_lock_switch(s))

    if call_time_unlimited := config.get(CONF_CALL_TIME_UNLIMITED):
        s = await switch.new_switch(call_time_unlimited)
        await cg.register_parented(s, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_call_time_unlimited_switch(s))