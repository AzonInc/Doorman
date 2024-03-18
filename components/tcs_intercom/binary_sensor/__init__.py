import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_ICON
from .. import tcs_intercom_ns, TCSIntercom, CONF_TCS_ID

TCSIntercomBinarySensor = tcs_intercom_ns.class_(
    "TCSIntercomBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONF_COMMAND = "command"
CONF_NAME = "name"
CONF_AUTO_OFF = "auto_off"

DEPENDENCIES = ["tcs_intercom"]

CONFIG_SCHEMA = cv.All(
    binary_sensor.binary_sensor_schema(TCSIntercomBinarySensor).extend(
        {
            cv.GenerateID(): cv.declare_id(TCSIntercomBinarySensor),
            cv.GenerateID(CONF_TCS_ID): cv.use_id(TCSIntercom),
            cv.Optional(CONF_COMMAND, default=50): cv.int_,
            cv.Optional(CONF_ICON, default="mdi:doorbell"): cv.icon,
            cv.Optional(CONF_NAME, default="Incoming call"): cv.string,
            cv.Optional(CONF_AUTO_OFF, default="3s"): cv.positive_time_period_seconds
        }
    ),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)
    cg.add(var.set_command(config[CONF_COMMAND]))
    cg.add(var.set_auto_off(config[CONF_AUTO_OFF]))
    tcs_intercom = await cg.get_variable(config[CONF_TCS_ID])
    cg.add(tcs_intercom.register_listener(var))