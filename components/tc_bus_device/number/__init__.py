import esphome.codegen as cg
from esphome.components import number
from esphome.components.number import NUMBER_MODES
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    CONF_MODE,
)
from .. import CONF_TC_BUS_DEVICE_ID, TCBusDeviceComponent, tc_bus_ns

SerialNumberNumber = tc_bus_ns.class_("SerialNumberNumber", number.Number, cg.Component)
ParallelSerialNumberNumber = tc_bus_ns.class_("ParallelSerialNumberNumber", number.Number, cg.Component)
AddressNumber = tc_bus_ns.class_("AddressNumber", number.Number, cg.Component)

VolumeHandsetDoorCallNumber = tc_bus_ns.class_("VolumeHandsetDoorCallNumber", number.Number, cg.Component)
VolumeHandsetInternalCallNumber = tc_bus_ns.class_("VolumeHandsetInternalCallNumber", number.Number, cg.Component)
VolumeRingtoneNumber = tc_bus_ns.class_("VolumeRingtoneNumber", number.Number, cg.Component)

AddressDividerNumber = tc_bus_ns.class_("AddressDividerNumber", number.Number, cg.Component)
DoorReadinessDurationNumber = tc_bus_ns.class_("DoorReadinessDurationNumber", number.Number, cg.Component)
CallTimeDurationNumber = tc_bus_ns.class_("CallTimeDurationNumber", number.Number, cg.Component)
DoorOpenerDurationNumber = tc_bus_ns.class_("DoorOpenerDurationNumber", number.Number, cg.Component)

CONF_SERIAL_NUMBER = "serial_number"
CONF_PARALLEL_SERIAL_NUMBER = "parallel_serial_number"
CONF_ADDRESS = "address"

CONF_VOLUME_HANDSET_DOOR_CALL = "volume_handset_door_call"
CONF_VOLUME_HANDSET_INTERNAL_CALL = "volume_handset_internal_call"
CONF_VOLUME_RINGTONE = "volume_ringtone"

CONF_ADDRESS_DIVIDER = "address_divider"
CONF_DOOR_READINESS_DURATION = "door_readiness_duration"
CONF_CALL_TIME_DURATION = "call_time_duration"
CONF_DOOR_OPENER_DURATION = "door_opener_duration"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_BUS_DEVICE_ID): cv.use_id(TCBusDeviceComponent),
        cv.Optional(CONF_SERIAL_NUMBER): number.number_schema(
            SerialNumberNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_PARALLEL_SERIAL_NUMBER): number.number_schema(
            ParallelSerialNumberNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_ADDRESS): number.number_schema(
            AddressNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_VOLUME_HANDSET_DOOR_CALL): number.number_schema(
            VolumeHandsetDoorCallNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:volume-high"
        ),
        cv.Optional(CONF_VOLUME_HANDSET_INTERNAL_CALL): number.number_schema(
            VolumeHandsetInternalCallNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:volume-high"
        ),
        cv.Optional(CONF_VOLUME_RINGTONE): number.number_schema(
            VolumeRingtoneNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:volume-high"
        ),

        cv.Optional(CONF_ADDRESS_DIVIDER): number.number_schema(
            AddressDividerNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DOOR_READINESS_DURATION): number.number_schema(
            DoorReadinessDurationNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CALL_TIME_DURATION): number.number_schema(
            CallTimeDurationNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DOOR_OPENER_DURATION): number.number_schema(
            DoorOpenerDurationNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
    }
)


async def to_code(config):
    tc_bus_device_component = await cg.get_variable(config[CONF_TC_BUS_DEVICE_ID])

    if serial_number := config.get(CONF_SERIAL_NUMBER):
        n = await number.new_number(
            serial_number, min_value=0, max_value=1000000, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_serial_number_number(n))

    if parallel_serial_number := config.get(CONF_PARALLEL_SERIAL_NUMBER):
        n = await number.new_number(
            parallel_serial_number, min_value=0, max_value=1000000, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_parallel_serial_number_number(n))

    if address := config.get(CONF_ADDRESS):
        n = await number.new_number(
            address, min_value=0, max_value=63, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_address_number(n))

    if volume_handset_door_call := config.get(CONF_VOLUME_HANDSET_DOOR_CALL):
        n = await number.new_number(
            volume_handset_door_call, min_value=0, max_value=7, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_volume_handset_door_call_number(n))

    if volume_handset_internal_call := config.get(CONF_VOLUME_HANDSET_INTERNAL_CALL):
        n = await number.new_number(
            volume_handset_internal_call, min_value=0, max_value=7, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_volume_handset_internal_call_number(n))

    if volume_ringtone := config.get(CONF_VOLUME_RINGTONE):
        n = await number.new_number(
            volume_ringtone, min_value=0, max_value=7, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_volume_ringtone_number(n))

    if address_divider := config.get(CONF_ADDRESS_DIVIDER):
        n = await number.new_number(
            address_divider, min_value=0, max_value=63, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_address_divider_number(n))

    if door_readiness_duration := config.get(CONF_DOOR_READINESS_DURATION):
        n = await number.new_number(
            door_readiness_duration, min_value=0, max_value=120, step=8
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_door_readiness_duration_number(n))

    if call_time_duration := config.get(CONF_CALL_TIME_DURATION):
        n = await number.new_number(
            call_time_duration, min_value=0, max_value=120, step=8
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_call_time_duration_number(n))

    if door_opener_duration := config.get(CONF_DOOR_OPENER_DURATION):
        n = await number.new_number(
            door_opener_duration, min_value=0, max_value=15, step=1
        )
        await cg.register_parented(n, config[CONF_TC_BUS_DEVICE_ID])
        cg.add(tc_bus_device_component.set_door_opener_duration_number(n))