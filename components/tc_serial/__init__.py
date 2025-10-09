from esphome.components.esp32 import get_esp32_variant
from esphome.components.esp32.const import (
    VARIANT_ESP32S3,
)
from esphome.components.logger import USB_CDC
from esphome.const import CONF_BAUD_RATE, CONF_HARDWARE_UART, CONF_ID, CONF_TRIGGER_ID, CONF_LOGGER
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.core import CORE
import esphome.final_validate as fv
from esphome import automation

CODEOWNERS = ["@azoninc"]
DEPENDENCIES = ["logger", "tc_bus"]

MULTI_CONF = False

CONF_CONFIGO_ID = "tc_serial"
CONF_TC_BUS_ID = "tc_bus"

tc_serial_ns = cg.esphome_ns.namespace("tc_serial")
TCSerialComponent = tc_serial_ns.class_("TCSerialComponent", cg.Component)

tc_bus_ns = cg.esphome_ns.namespace("tc_bus")
TCBusComponent = tc_bus_ns.class_("TCBusComponent", cg.Component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TCSerialComponent),
            cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


def validate_logger(config):
    logger_conf = fv.full_config.get()[CONF_LOGGER]
    if logger_conf[CONF_BAUD_RATE] == 0:
        raise cv.Invalid("TC:Serial requires the logger baud_rate to be not 0")
    if CORE.using_esp_idf:
        if (
            logger_conf[CONF_HARDWARE_UART] == USB_CDC
            and get_esp32_variant() == VARIANT_ESP32S3
        ):
            raise cv.Invalid(
                "TC:Serial does not support the selected logger hardware_uart"
            )
    return config


FINAL_VALIDATE_SCHEMA = validate_logger


async def to_code(config):
    cg.add_global(tc_serial_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])

    tc_bus_component = await cg.get_variable(config[CONF_TC_BUS_ID])
    cg.add(var.set_base_component(tc_bus_component))

    await cg.register_component(var, config)