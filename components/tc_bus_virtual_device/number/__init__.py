import esphome.codegen as cg
from esphome.components import number
from esphome.components.number import NUMBER_MODES
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    CONF_MODE,
)
from .. import CONF_TC_BUS_VIRTUAL_DEVICE_ID, TCBusVirtualDeviceComponent, tc_bus_ns

VSerialNumberNumber = tc_bus_ns.class_("VSerialNumberNumber", number.Number, cg.Component)
VAddressNumber = tc_bus_ns.class_("VAddressNumber", number.Number, cg.Component)
VAddressDividerNumber = tc_bus_ns.class_("VAddressDividerNumber", number.Number, cg.Component)
VDoorReadinessDurationNumber = tc_bus_ns.class_("VDoorReadinessDurationNumber", number.Number, cg.Component)
VCallTimeDurationNumber = tc_bus_ns.class_("VCallTimeDurationNumber", number.Number, cg.Component)
VDoorOpenerDurationNumber = tc_bus_ns.class_("VDoorOpenerDurationNumber", number.Number, cg.Component)

CONF_SERIAL_NUMBER = "serial_number"
CONF_ADDRESS = "address"

CONF_ADDRESS_DIVIDER = "address_divider"
CONF_DOOR_READINESS_DURATION = "door_readiness_duration"
CONF_CALL_TIME_DURATION = "call_time_duration"
CONF_DOOR_OPENER_DURATION = "door_opener_duration"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_VIRTUAL_DEVICE_ID): cv.use_id(TCBusVirtualDeviceComponent),
        cv.Optional(CONF_SERIAL_NUMBER): number.number_schema(
            VSerialNumberNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_ADDRESS): number.number_schema(
            VAddressNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_ADDRESS_DIVIDER): number.number_schema(
            VAddressDividerNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DOOR_READINESS_DURATION): number.number_schema(
            VDoorReadinessDurationNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CALL_TIME_DURATION): number.number_schema(
            VCallTimeDurationNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DOOR_OPENER_DURATION): number.number_schema(
            VDoorOpenerDurationNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
    }
)


async def to_code(config):
    tc_bus_device_component = await cg.get_variable(config[CONF_TC_BUS_VIRTUAL_DEVICE_ID])

    if serial_number := config.get(CONF_SERIAL_NUMBER):
        n = await number.new_number(
            serial_number, min_value=0, max_value=1048575, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_VIRTUAL_DEVICE_ID])
        cg.add(tc_bus_device_component.set_serial_number_number(n))

    if address := config.get(CONF_ADDRESS):
        n = await number.new_number(
            address, min_value=0, max_value=63, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_VIRTUAL_DEVICE_ID])
        cg.add(tc_bus_device_component.set_address_number(n))

    if address_divider := config.get(CONF_ADDRESS_DIVIDER):
        n = await number.new_number(
            address_divider, min_value=0, max_value=63, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_VIRTUAL_DEVICE_ID])
        cg.add(tc_bus_device_component.set_address_divider_number(n))

    if door_readiness_duration := config.get(CONF_DOOR_READINESS_DURATION):
        n = await number.new_number(
            door_readiness_duration, min_value=0, max_value=15, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_VIRTUAL_DEVICE_ID])
        cg.add(tc_bus_device_component.set_door_readiness_duration_number(n))

    if call_time_duration := config.get(CONF_CALL_TIME_DURATION):
        n = await number.new_number(
            call_time_duration, min_value=0, max_value=15, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_VIRTUAL_DEVICE_ID])
        cg.add(tc_bus_device_component.set_call_time_duration_number(n))

    if door_opener_duration := config.get(CONF_DOOR_OPENER_DURATION):
        n = await number.new_number(
            door_opener_duration, min_value=0, max_value=15, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_VIRTUAL_DEVICE_ID])
        cg.add(tc_bus_device_component.set_door_opener_duration_number(n))