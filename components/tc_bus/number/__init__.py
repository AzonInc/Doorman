import esphome.codegen as cg
from esphome.components import number
from esphome.components.number import NUMBER_MODES
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
    CONF_MODE,
)
from .. import CONF_TC_ID, TCBusComponent, tc_bus_ns

SerialNumberNumber = tc_bus_ns.class_("SerialNumberNumber", number.Number, cg.Component)
VolumeHandsetNumber = tc_bus_ns.class_("VolumeHandsetNumber", number.Number, cg.Component)
VolumeRingtoneNumber = tc_bus_ns.class_("VolumeRingtoneNumber", number.Number, cg.Component)

CONF_SERIAL_NUMBER = "serial_number"
CONF_VOLUME_HANDSET = "volume_handset"
CONF_VOLUME_RINGTONE = "volume_ringtone"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TC_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_SERIAL_NUMBER): number.number_schema(
            SerialNumberNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:numeric"
        ).extend({ cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_VOLUME_HANDSET): number.number_schema(
            VolumeHandsetNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:volume-high"
        ),
        cv.Optional(CONF_VOLUME_RINGTONE): number.number_schema(
            VolumeRingtoneNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:volume-high"
        ),
    }
)


async def to_code(config):
    tc_bus_component = await cg.get_variable(config[CONF_TC_ID])

    if serial_number := config.get(CONF_SERIAL_NUMBER):
        n = await number.new_number(
            serial_number, min_value=0, max_value=1048575, step=1
        )
        await cg.register_parented(n, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_serial_number_number(n))

    if volume_handset := config.get(CONF_VOLUME_HANDSET):
        n = await number.new_number(
            volume_handset, min_value=0, max_value=7, step=1
        )
        await cg.register_parented(n, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_volume_handset_number(n))

    if volume_ringtone := config.get(CONF_VOLUME_RINGTONE):
        n = await number.new_number(
            volume_ringtone, min_value=0, max_value=7, step=1
        )
        await cg.register_parented(n, config[CONF_TC_ID])
        cg.add(tc_bus_component.set_volume_ringtone_number(n))