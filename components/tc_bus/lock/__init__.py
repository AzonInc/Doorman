import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import lock
from esphome.const import CONF_ID, CONF_ICON, CONF_TRIGGER_ID
from .. import tc_bus_ns, TCBusComponent, CONF_TC_BUS_ID

BusTelegramListenerLock = tc_bus_ns.class_("BusTelegramListenerLock", lock.Lock, cg.Component)

CONF_ADDRESS = "address"
CONF_ADDRESS_LAMBDA = "address_lambda"

CONF_NAME = "name"
CONF_AUTO_LOCK = "auto_lock"

CONF_ON_BEFORE_UNLOCK = "before_unlock_action"
CONF_ON_AFTER_UNLOCK = "after_unlock_action"
CONF_ON_LOCK = "lock_action"

DEPENDENCIES = ["tc_bus"]

BeforeUnlockTrigger = tc_bus_ns.class_("BeforeUnlockTrigger", automation.Trigger.template())
AfterUnlockTrigger = tc_bus_ns.class_("AfterUnlockTrigger", automation.Trigger.template())
LockTrigger = tc_bus_ns.class_("LockTrigger", automation.Trigger.template())


def validate(config):
    config = config.copy()

    if CONF_ADDRESS in config and CONF_ADDRESS_LAMBDA in config:
        raise cv.Invalid("You can either set ADDRESS or ADDRESS_LAMBDA, not both.")

    return config

CONFIG_SCHEMA = cv.All(
    lock.lock_schema(BusTelegramListenerLock).extend(
        {
            cv.GenerateID(): cv.declare_id(BusTelegramListenerLock),
            cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),

            cv.Optional(CONF_ADDRESS): cv.hex_uint8_t,
            cv.Optional(CONF_ADDRESS_LAMBDA): cv.returning_lambda,

            cv.Optional(CONF_ICON, default="mdi:door"): cv.icon,
            cv.Optional(CONF_NAME, default="Entrance Door"): cv.string,
            cv.Optional(CONF_AUTO_LOCK, default="5s"): cv.positive_time_period_milliseconds,

            cv.Optional(CONF_ON_BEFORE_UNLOCK): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(BeforeUnlockTrigger),
            }),
            cv.Optional(CONF_ON_AFTER_UNLOCK): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(AfterUnlockTrigger),
            }),
            cv.Optional(CONF_ON_LOCK): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LockTrigger),
            }),
        }
    ),
    validate,
)

async def new_lock(config, *args):
    var = cg.new_Pvariable(config[CONF_ID], *args)
    await lock.register_lock(var, config)
    return var

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await lock.register_lock(var, config)

    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))

    if CONF_ADDRESS_LAMBDA in config:
        address_template_ = await cg.process_lambda(
            config[CONF_ADDRESS_LAMBDA], [], return_type=cg.optional.template(cg.uint8)
        )
        cg.add(var.set_address_lambda(address_template_))

    cg.add(var.set_auto_lock(config[CONF_AUTO_LOCK]))

    for conf in config.get(CONF_ON_BEFORE_UNLOCK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_AFTER_UNLOCK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_LOCK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    tc_bus = await cg.get_variable(config[CONF_TC_BUS_ID])
    cg.add(tc_bus.register_lock_listener(var))

    await cg.register_parented(var, config[CONF_TC_BUS_ID])
