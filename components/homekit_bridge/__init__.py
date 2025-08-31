import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.automation import Condition
from esphome.const import CONF_PORT, PLATFORM_ESP32, CONF_ID, CONF_TRIGGER_ID
from esphome.components.esp32 import add_idf_component, add_idf_sdkconfig_option
import re

DEPENDENCIES = ['esp32', 'network']
CODEOWNERS = ["@rednblkx"]
MULTI_CONF = False

homekit_ns = cg.esphome_ns.namespace('homekit')
HomeKitBridgeComponent = homekit_ns.class_('HomeKitBridgeComponent', cg.Component)

AInfo = homekit_ns.enum("AInfo")

PairingStartedTrigger = homekit_ns.class_("PairingStartedTrigger", automation.Trigger.template())
PairingAbortedTrigger = homekit_ns.class_("PairingAbortedTrigger", automation.Trigger.template())
PairingTimeoutTrigger = homekit_ns.class_("PairingTimeoutTrigger", automation.Trigger.template())
PairingCompletedTrigger = homekit_ns.class_("PairingCompletedTrigger", automation.Trigger.template(cg.std_string))
IdentifyTrigger = homekit_ns.class_("IdentifyTrigger", automation.Trigger.template())
ControllerConnectedTrigger = homekit_ns.class_("ControllerConnectedTrigger", automation.Trigger.template(cg.std_string))
ControllerDisconnectedTrigger = homekit_ns.class_("ControllerDisconnectedTrigger", automation.Trigger.template(cg.std_string))
PairedCondition = homekit_ns.class_("PairedCondition", Condition)
ConnectedCondition = homekit_ns.class_("ConnectedCondition", Condition)

CONF_HAP_ID = "hap_id"
CONF_TASK_STACK_SIZE = "task_stack_size"
CONF_SETUP_CODE = "setup_code"
CONF_SETUP_ID = "setup_id"
CONF_NAME = "name"
CONF_MODEL = "model"
CONF_MANUFACTURER = "manufacturer"

CONF_ON_PAIRING_STARTED = "on_pairing_started"
CONF_ON_PAIRING_ABORTED = "on_pairing_aborted"
CONF_ON_PAIRING_TIMEOUT = "on_pairing_timeout"
CONF_ON_PAIRING_COMPLETED = "on_pairing_completed"
CONF_ON_IDENTIFY = "on_identify"
CONF_ON_CONTROLLER_CONNECTED = "on_controller_connected"
CONF_ON_CONTROLLER_DISCONNECTED = "on_controller_disconnected"

def hk_setup_code(value):
    """Validate that a given config value is a valid icon."""
    value = cv.string_strict(value)
    if not value:
        return value
    if re.match("^[\\d]{3}-[\\d]{2}-[\\d]{3}$", value):
        return value
    raise cv.Invalid(
        'Setup code must match the format XXX-XX-XXX'
    )

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(HomeKitBridgeComponent),
        cv.Optional(CONF_PORT, default=32042): cv.port,
        cv.Optional(CONF_SETUP_CODE): hk_setup_code,
        cv.Optional(CONF_NAME): cv.string_strict,
        cv.Optional(CONF_MODEL): cv.string_strict,
        cv.Optional(CONF_MANUFACTURER): cv.string_strict,
        cv.Optional(CONF_TASK_STACK_SIZE): cv.uint32_t,
        cv.Optional(CONF_SETUP_ID): cv.All(
            cv.string_strict,
            cv.Upper,
            cv.Length(
                min=4,
                max=4,
                msg="Setup ID has to be a 4 character long alpha numeric string (with capital letters)"
            )
        ),
        cv.Optional(CONF_ON_PAIRING_STARTED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PairingStartedTrigger),
            }
        ),
        cv.Optional(CONF_ON_PAIRING_ABORTED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PairingAbortedTrigger),
            }
        ),
        cv.Optional(CONF_ON_PAIRING_TIMEOUT): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PairingTimeoutTrigger),
            }
        ),
        cv.Optional(CONF_ON_PAIRING_COMPLETED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PairingCompletedTrigger),
            }
        ),
        cv.Optional(CONF_ON_IDENTIFY): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IdentifyTrigger),
            }
        ),
        cv.Optional(CONF_ON_CONTROLLER_CONNECTED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ControllerConnectedTrigger),
            }
        ),
        cv.Optional(CONF_ON_CONTROLLER_DISCONNECTED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ControllerDisconnectedTrigger),
            }
        ),
    }).extend(cv.COMPONENT_SCHEMA),
    cv.only_on([PLATFORM_ESP32]),
    cv.only_with_esp_idf
)

async def to_code(config):
    add_idf_component(
        name="esp_hap_core",
        repo="https://github.com/rednblkx/esp-homekit-sdk",
        ref="upstream",
        path="components/homekit/esp_hap_core"
    )
    add_idf_component(
        name="esp_hap_apple_profiles",
        repo="https://github.com/rednblkx/esp-homekit-sdk",
        ref="upstream",
        path="components/homekit/esp_hap_apple_profiles"
    )
    add_idf_component(
        name="esp_hap_extras",
        repo="https://github.com/rednblkx/esp-homekit-sdk",
        ref="upstream",
        path="components/homekit/esp_hap_extras"
    )
    add_idf_component(
        name="esp_hap_platform",
        repo="https://github.com/rednblkx/esp-homekit-sdk",
        ref="upstream",
        path="components/homekit/esp_hap_platform"
    )
    add_idf_component(
        name="hkdf-sha",
        repo="https://github.com/rednblkx/esp-homekit-sdk",
        ref="upstream",
        path="components/homekit/hkdf-sha"
    )
    add_idf_component(
        name="mu_srp",
        repo="https://github.com/rednblkx/esp-homekit-sdk",
        ref="upstream",
        path="components/homekit/mu_srp"
    )

    # cg.add_define("CONFIG_ESP_MFI_DEBUG_ENABLE")

    if CONF_PORT in config:
        add_idf_sdkconfig_option("CONFIG_HAP_HTTP_SERVER_PORT", config[CONF_PORT])

    var = cg.new_Pvariable(config[CONF_ID])

    if CONF_TASK_STACK_SIZE in config:
        cg.add(var.set_task_stack_size(config[CONF_TASK_STACK_SIZE]))

    if CONF_SETUP_CODE in config:
        cg.add(var.set_setup_code(config[CONF_SETUP_CODE]))
    
    if CONF_SETUP_ID in config:
        cg.add(var.set_setup_id(config[CONF_SETUP_ID]))

    if CONF_NAME in config:
        cg.add(var.set_name(config[CONF_NAME]))

    if CONF_MODEL in config:
        cg.add(var.set_model(config[CONF_MODEL]))

    if CONF_MANUFACTURER in config:
        cg.add(var.set_manufacturer(config[CONF_MANUFACTURER]))

    await cg.register_component(var, config)

    for conf in config.get(CONF_ON_PAIRING_STARTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_PAIRING_ABORTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_PAIRING_TIMEOUT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_PAIRING_COMPLETED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)

    for conf in config.get(CONF_ON_IDENTIFY, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_CONTROLLER_CONNECTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)

    for conf in config.get(CONF_ON_CONTROLLER_DISCONNECTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)


@automation.register_condition("homekit.paired", PairedCondition, cv.Schema({}))
async def homekit_paired_to_code(config, condition_id, template_arg, args):
    return cg.new_Pvariable(condition_id, template_arg)

@automation.register_condition("homekit.connected", ConnectedCondition, cv.Schema({}))
async def homekit_connected_to_code(config, condition_id, template_arg, args):
    return cg.new_Pvariable(condition_id, template_arg)