import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_TYPE, CONF_VALUE
from ..tc_bus import tc_bus_ns, TCBusComponent, CONF_TC_BUS_ID, TELEGRAM_TYPE, TELEGRAM_TYPES

AUTO_LOAD = ["tc_bus"]

CODEOWNERS = ["@azoninc"]

DEPENDENCIES = ["tc_bus"]
MULTI_CONF = True

TCBusVirtualDeviceComponent = tc_bus_ns.class_('TCBusVirtualDeviceComponent', cg.Component)

TelegramData = tc_bus_ns.struct("TelegramData")

TCBusVirtualDeviceSendAction = tc_bus_ns.class_(
    "TCBusVirtualDeviceSendAction",
    automation.Action,
    cg.Parented.template(TCBusVirtualDeviceComponent)
)

TCBusVirtualDeviceAnswerCallAction = tc_bus_ns.class_(
    "TCBusVirtualDeviceAnswerCallAction",
    automation.Action,
    cg.Parented.template(TCBusVirtualDeviceComponent)
)

TCBusVirtualDeviceEndCallAction = tc_bus_ns.class_(
    "TCBusVirtualDeviceEndCallAction",
    automation.Action,
    cg.Parented.template(TCBusVirtualDeviceComponent)
)

TCBusVirtualDeviceCallAction = tc_bus_ns.class_(
    "TCBusVirtualDeviceCallAction",
    automation.Action,
    cg.Parented.template(TCBusVirtualDeviceComponent)
)

IncomingCallTrigger = tc_bus_ns.class_(
    "IncomingCallTrigger", 
    automation.Trigger.template()
)

CallStartedTrigger = tc_bus_ns.class_(
    "CallStartedTrigger", 
    automation.Trigger.template()
)

CallEndedTrigger = tc_bus_ns.class_(
    "CallEndedTrigger", 
    automation.Trigger.template()
)

CallFailedTrigger = tc_bus_ns.class_(
    "CallFailedTrigger", 
    automation.Trigger.template()
)

DEVICE_GROUP = tc_bus_ns.enum("VirtualDeviceGroup")
DEVICE_GROUPS = {
    "indoor_station": DEVICE_GROUP.VIRTUAL_DEVICE_GROUP_INDOOR_STATION,
    "outdoor_station": DEVICE_GROUP.VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION,
}

CONF_TC_BUS_VIRTUAL_DEVICE = "tc_bus_virtual_device"
CONF_TC_BUS_VIRTUAL_DEVICE_ID = "tc_bus_virtual_device_id"

CONF_TELEGRAM = "telegram"
CONF_IS_LONG = "is_long"
CONF_ADDRESS = "address"
CONF_PAYLOAD = "payload"
CONF_INTERNAL = "internal"

CONF_ON_INCOMING_CALL = "on_incoming_call"
CONF_ON_CALL_STARTED = "on_call_started"
CONF_ON_CALL_ENDED = "on_call_ended"
CONF_ON_CALL_FAILED = "on_call_failed"

def validate_config(config):
    return config

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID() : cv.declare_id(TCBusVirtualDeviceComponent),
        cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_TYPE, default="indoor_station"): cv.enum(DEVICE_GROUPS, upper=False),
        cv.Optional(CONF_ON_INCOMING_CALL): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IncomingCallTrigger),
            }
        ),
        cv.Optional(CONF_ON_CALL_STARTED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CallStartedTrigger),
            }
        ),
        cv.Optional(CONF_ON_CALL_ENDED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CallEndedTrigger),
            }
        ),
        cv.Optional(CONF_ON_CALL_FAILED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CallFailedTrigger),
            }
        ),
    }
)

CONFIG_SCHEMA = cv.All(
    CONFIG_SCHEMA.extend(cv.COMPONENT_SCHEMA),
    validate_config,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    tc_bus_component = await cg.get_variable(config[CONF_TC_BUS_ID])
    cg.add(var.set_tc_bus_component(tc_bus_component))
    cg.add(var.set_internal_id(str(config[CONF_ID])))
    cg.add(var.set_device_group(config[CONF_TYPE]))

    for conf in config.get(CONF_ON_INCOMING_CALL, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    for conf in config.get(CONF_ON_CALL_STARTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    for conf in config.get(CONF_ON_CALL_ENDED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    for conf in config.get(CONF_ON_CALL_FAILED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)


TC_BUS_VIRTUAL_DEVICE_SEND_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusVirtualDeviceComponent),
        cv.Required(CONF_TYPE): cv.templatable(cv.enum(TELEGRAM_TYPES, upper=False)),
        cv.Optional(CONF_ADDRESS, default="0"): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_PAYLOAD, default="0"): cv.templatable(cv.hex_uint32_t)
    })
)

@automation.register_action(
    "tc_bus_virtual_device.send",
    TCBusVirtualDeviceSendAction,
    TC_BUS_VIRTUAL_DEVICE_SEND_SCHEMA
)
async def tc_bus_virtual_device_send_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    type_template = await cg.templatable(config[CONF_TYPE], args, TELEGRAM_TYPE)
    cg.add(var.set_type(type_template))

    address_template = await cg.templatable(config[CONF_ADDRESS], args, cg.uint8)
    cg.add(var.set_address(address_template))

    payload_template = await cg.templatable(config[CONF_PAYLOAD], args, cg.uint32)
    cg.add(var.set_payload(payload_template))

    return var


TC_BUS_VIRTUAL_DEVICE_ANSWER_CALL_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusVirtualDeviceComponent),
    })
)

@automation.register_action(
    "tc_bus_virtual_device.answer_call",
    TCBusVirtualDeviceAnswerCallAction,
    TC_BUS_VIRTUAL_DEVICE_ANSWER_CALL_SCHEMA
)
async def tc_bus_virtual_device_answer_call_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    return var


TC_BUS_VIRTUAL_DEVICE_END_CALL_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusVirtualDeviceComponent),
    })
)

@automation.register_action(
    "tc_bus_virtual_device.end_call",
    TCBusVirtualDeviceEndCallAction,
    TC_BUS_VIRTUAL_DEVICE_END_CALL_SCHEMA
)
async def tc_bus_virtual_device_end_call_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    return var


TC_BUS_VIRTUAL_DEVICE_CALL_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusVirtualDeviceComponent),
        cv.Optional(CONF_ADDRESS, default="0"): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_INTERNAL, default="false"): cv.templatable(cv.boolean),
    })
)

@automation.register_action(
    "tc_bus_virtual_device.call",
    TCBusVirtualDeviceCallAction,
    TC_BUS_VIRTUAL_DEVICE_CALL_SCHEMA
)
async def tc_bus_virtual_device_call_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    address_template = await cg.templatable(config[CONF_ADDRESS], args, cg.uint32)
    cg.add(var.set_address(address_template))

    internal_template = await cg.templatable(config[CONF_INTERNAL], args, cg.bool_)
    cg.add(var.set_internal(internal_template))

    return var