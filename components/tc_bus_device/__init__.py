from logging import config
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_TYPE, CONF_VALUE, ENTITY_CATEGORY_DIAGNOSTIC
from ..tc_bus import tc_bus_ns, TCBusComponent, CONF_TC_BUS_ID, TELEGRAM_TYPE, TELEGRAM_TYPES

AUTO_LOAD = ["tc_bus"]

CODEOWNERS = ["@azoninc"]

DEPENDENCIES = ["tc_bus"]
MULTI_CONF = True

TCBusDeviceComponent = tc_bus_ns.class_('TCBusDeviceComponent', cg.Component)

TelegramData = tc_bus_ns.struct("TelegramData")
SettingData = tc_bus_ns.struct("SettingData")
ModelData = tc_bus_ns.struct("ModelData")

TCBusDeviceSendAction = tc_bus_ns.class_(
    "TCBusDeviceSendAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceUpdateSettingAction = tc_bus_ns.class_(
    "TCBusDeviceUpdateSettingAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceUpdateDoorbellButtonAction = tc_bus_ns.class_(
    "TCBusDeviceUpdateDoorbellButtonAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceReadMemoryAction = tc_bus_ns.class_(
    "TCBusDeviceReadMemoryAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceIdentifyAction = tc_bus_ns.class_(
    "TCBusDeviceIdentifyAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceOpenDoorAction = tc_bus_ns.class_(
    "TCBusDeviceOpenDoorAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceAnswerCallAction = tc_bus_ns.class_(
    "TCBusDeviceAnswerCallAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceEndCallAction = tc_bus_ns.class_(
    "TCBusDeviceEndCallAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)

TCBusDeviceCallAction = tc_bus_ns.class_(
    "TCBusDeviceCallAction",
    automation.Action,
    cg.Parented.template(TCBusDeviceComponent)
)


IdentifyCompleteTrigger = tc_bus_ns.class_(
    "IdentifyCompleteTrigger",
    automation.Trigger.template()
)

IdentifyUnknownTrigger = tc_bus_ns.class_(
    "IdentifyUnknownTrigger", 
    automation.Trigger.template()
)

IdentifyTimeoutTrigger = tc_bus_ns.class_(
    "IdentifyTimeoutTrigger", 
    automation.Trigger.template()
)

ReadMemoryCompleteTrigger = tc_bus_ns.class_(
    "ReadMemoryCompleteTrigger", 
    automation.Trigger.template()
)

ReadMemoryTimeoutTrigger = tc_bus_ns.class_(
    "ReadMemoryTimeoutTrigger", 
    automation.Trigger.template()
)

ReceivedDeviceTelegramTrigger = tc_bus_ns.class_(
    "ReceivedDeviceTelegramTrigger", 
    automation.Trigger.template()
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

SETTING_TYPE = tc_bus_ns.enum("SettingType")
SETTING_TYPES = {
    "ringtone_floor_call": SETTING_TYPE.SETTING_RINGTONE_FLOOR_CALL,
    "ringtone_entrance_door_call": SETTING_TYPE.SETTING_RINGTONE_ENTRANCE_DOOR_CALL,
    "ringtone_second_entrance_door_call": SETTING_TYPE.SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL,
    "ringtone_internal_call": SETTING_TYPE.SETTING_RINGTONE_INTERNAL_CALL,
    "volume_ringtone": SETTING_TYPE.SETTING_VOLUME_RINGTONE,
    "volume_handset_door": SETTING_TYPE.SETTING_VOLUME_HANDSET_DOOR,
    "volume_handset_internal": SETTING_TYPE.SETTING_VOLUME_HANDSET_INTERNAL,
    "as_address_divider": SETTING_TYPE.SETTING_AS_ADDRESS_DIVIDER,
    "vas_address_divider": SETTING_TYPE.SETTING_VAS_ADDRESS_DIVIDER,
    "use_long_door_opener_protocol": SETTING_TYPE.SETTING_USE_LONG_DOOR_OPENER_PROTOCOL,
    "ambient_light_in_standby": SETTING_TYPE.SETTING_AMBIENT_LIGHT_IN_STANDBY,
    "ringtone_mute": SETTING_TYPE.SETTING_RINGTONE_MUTE,
    "door_opener_duration": SETTING_TYPE.SETTING_DOOR_OPENER_DURATION,
    "address": SETTING_TYPE.SETTING_ADDRESS,
    "address_lock": SETTING_TYPE.SETTING_ADDRESS_LOCK,
    "calling_requires_door_readiness": SETTING_TYPE.SETTING_CALLING_REQUIRES_DOOR_READINESS,
    "door_opener_requires_door_readiness": SETTING_TYPE.SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS,
    "door_opener_requires_active_call": SETTING_TYPE.SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL,
    "door_readiness_duration": SETTING_TYPE.SETTING_DOOR_READINESS_DURATION,
    "call_time_duration": SETTING_TYPE.SETTING_CALL_TIME_DURATION,
    "auto_answer_call": SETTING_TYPE.SETTING_AUTO_ANSWER_CALL,
    "call_time_unlimited": SETTING_TYPE.CALL_TIME_UNLIMITED,
    "button_rows": SETTING_TYPE.SETTING_BUTTON_ROWS,
    "has_code_lock": SETTING_TYPE.SETTING_HAS_CODE_LOCK,
}

DOORBELL_BUTTON_ACTION = tc_bus_ns.enum("DoorbellButtonAction")
DOORBELL_BUTTON_ACTIONS = {
    "none": DOORBELL_BUTTON_ACTION.DOORBELL_BUTTON_ACTION_NONE,
    "light": DOORBELL_BUTTON_ACTION.DOORBELL_BUTTON_ACTION_LIGHT,
    "door_call": DOORBELL_BUTTON_ACTION.DOORBELL_BUTTON_ACTION_DOOR_CALL,
    "control_function": DOORBELL_BUTTON_ACTION.DOORBELL_BUTTON_ACTION_CONTROL_FUNCTION,
}

DEVICE_GROUP = tc_bus_ns.enum("DeviceGroup")
DEVICE_GROUPS = {
    "indoor_station": DEVICE_GROUP.DEVICE_GROUP_INDOOR_STATION_CLASSIC,
    "outdoor_station": DEVICE_GROUP.DEVICE_GROUP_OUTDOOR_STATION,
    "controller": DEVICE_GROUP.DEVICE_GROUP_CONTROLLER,
    "extension": DEVICE_GROUP.DEVICE_GROUP_EXTENSION,
}

CONF_MODEL_IS = [
    "TCS ISW3030 / Koch TC50 / Scantron Stilux",
    "TCS ISW3130 / Koch TC50P",
    "TCS ISW3230 / Koch TC50 GFA",
    "TCS ISW3330 / Koch TC50 BW",
    "TCS ISW3340",
    "TCS TASTA Audio / Koch 60 Series Audio",
    "TCS TASTA PRO Audio",
    "TCS TASTA Video / Koch 60 Series Video / Scantron VIVO",
    "TCS TASTA PRO Video",
    "TCS SENSO PRO Audio / Koch 70 Series Audio",
    "TCS SENSO PRO Video / Koch 70 Series Video",
    "TCS ECOOS / Koch ecoos / Scantron SLIM60T",
    "TCS ISW4100 / Koch TC31",
    "TCS IMM2100 / Koch TCE31",
    "TCS VTC42V2",
    "TCS TC40V2",
    "TCS VTC40",
    "TCS TC40",
    "TCS TC2000",
    "TCS TC20P",
    "TCS TC20F",
    "TCS ISH3022 / Koch TCH50P",
    "TCS ISH3130 / Koch TCH50P / Scantron LuxPlus",
    "TCS ISH3230 / Koch TCH50 GFA",
    "TCS ISH3030 / Koch TCH50 / Scantron Lux2",
    "TCS ISH1030 / Koch TTS25",
    "TCS TTC-XX",
    "TCS TTS-XX",
    "TCS IMM1000 / Koch TCH30",
    "TCS IMM1100 / Koch TCHE30",
    "TCS IMM1300 / Koch VTCH30",
    "TCS IMM1500",
    "TCS IMM1310 / Koch VTCHE30",
    "TCS IMM1110 / Koch TCHEE30",
    "TCS IVH3222 / Koch VTCH50 / Scantron VLux",
    "TCS IVH4222 / Koch VTCH50/2D",
    "TCS IVW2220 / Koch Sky",
    "TCS IVW2221 / Koch Sky R1.00",
    "TCS IVW3011 / Koch Skyline Plus",
    "TCS IVW3012 / Koch Skyline/Aldup",
    "TCS VMH / Koch VMH",
    "TCS VML / Koch VML",
    "TCS VMF / Koch VMF",
    "Jung TKM IS",
    "Jung TKM ISV",
    "TCS CAIXXXX / Koch CAIXXXX",
    "TCS CAI2000 / Koch Carus",
    "TCS ISW42X0",
    "TCS IVW9010",
    "TCS IVW9011 / Koch VTP10",
    "TCS IVW9110",
    "TCS IVW9030 / Scantron SLIM50T",
    "TCS IVE70",
    "DEBUG IS Classic",
    "DEBUG IS Handsfree",
]

CONF_MODEL_AS = [
    "DEBUG AS",
    "TCS PUK",
    "TCS PUK-DSP",
    "TCS PAKV2",
    "TCS PAKV3",
    "TCS PDS0X",
    "TCS PDS0X/04",
    "TCS PES",
    "TCS TCU2",
    "TCS TCU3",
    "TCS TCU3 + TCKE3 (1)",
    "TCS TCU3 + TCKE3 (2)",
    "TCS TCU3 + TCKE3 (3)",
    "TCS TCU3 + TCKE3 (4)",
    "TCS TCU3 + TCKE3 (5)",
    "TCS TCU3 + TCKE3 (6)",
    "TCS TCU4",
    "TCS TCU4 + TCKE3 (1)",
    "TCS TCU4 + TCKE3 (2)",
    "TCS TCU4 + TCKE3 (3)",
    "TCS TCU4 + TCKE3 (4)",
    "TCS TCU4 + TCKE3 (5)",
    "TCS TCU4 + TCKE3 (6)",
    "Jung TKM AS",
    "Jung TKM ASV"
]

CONF_MODEL_CTRL = [
    "TCS BVS20",
    "TCS BVS30",
    "TCS NBV3210",
    "TCS VBVS30",
    "TCS NBV2600",
    "TCS VBVS05",
    "DEBUG CONTROLLER",
]

CONF_MODEL_EXT = [
    "TCS BRE2",
    "TCS BRE2-EB",
    "TCS TRE2",
    "TCS TOER2-EB",
    "TCS FFL1000",
    "TCS FAA1200",
    "DEBUG EXTENSION",
]

CONF_RINGTONES = [
    "Ringtone 1",
    "Ringtone 2",
    "Ringtone 3",
    "Ringtone 4",
    "Ringtone 5",
    "Ringtone 6",
    "Ringtone 7",
    "Ringtone 8",
    "Ringtone 9",
    "Ringtone 10",
    "Ringtone 11",
    "Ringtone 12",
    "Ringtone 13"
]

CONF_TC_BUS_DEVICE = "tc_bus_device"
CONF_TC_BUS_DEVICE_ID = "tc_bus_device_id"

CONF_AUTO_CONFIGURATION = "auto_configuration"
CONF_VIRTUAL = "virtual"

CONF_TELEGRAM = "telegram"
CONF_IS_LONG = "is_long"
CONF_ADDRESS = "address"
CONF_PAYLOAD = "payload"

CONF_INTERNAL = "internal"

CONF_BUTTON_ROW = "button_row"
CONF_BUTTON_COL = "button_col"
CONF_PRIMARY_ACTION = "primary_action"
CONF_PRIMARY_PAYLOAD = "primary_payload"
CONF_SECONDARY_ACTION = "secondary_action"
CONF_SECONDARY_PAYLOAD = "secondary_payload"

CONF_ON_READ_MEMORY_COMPLETE = "on_read_memory_complete"
CONF_ON_READ_MEMORY_FAILED = "on_read_memory_failed"
CONF_ON_IDENTIFY_COMPLETE = "on_identify_complete"
CONF_ON_IDENTIFY_UNKNOWN = "on_identify_unknown"
CONF_ON_IDENTIFY_FAILED = "on_identify_failed"

CONF_ON_INCOMING_CALL = "on_incoming_call"
CONF_ON_CALL_STARTED = "on_call_started"
CONF_ON_CALL_ENDED = "on_call_ended"
CONF_ON_CALL_FAILED = "on_call_failed"

def validate_config(config):
    device_group = config.get(CONF_TYPE)
    is_virtual = config.get(CONF_VIRTUAL, False)

    # Keys that are only valid for virtual devices
    virtual_only_keys = [
        CONF_ON_INCOMING_CALL,
        CONF_ON_CALL_STARTED,
        CONF_ON_CALL_ENDED,
        CONF_ON_CALL_FAILED
    ]
    for key in virtual_only_keys:
        if key in config and not is_virtual:
            raise cv.Invalid(f"'{key}' is only compatible with virtual devices.", path=[key])

    if not is_virtual:
        return config
    
    # From here on: virtual devices only
    if device_group not in ("indoor_station", "outdoor_station"):
        raise cv.Invalid(
            "Only virtual 'indoor_station' and 'outdoor_station' devices are supported.",
            path=[CONF_VIRTUAL]
        )

    for key in [CONF_AUTO_CONFIGURATION, CONF_ON_READ_MEMORY_COMPLETE, CONF_ON_READ_MEMORY_FAILED,
                CONF_ON_IDENTIFY_COMPLETE, CONF_ON_IDENTIFY_UNKNOWN, CONF_ON_IDENTIFY_FAILED]:
        if key in config:
            raise cv.Invalid(f"'{key}' is not compatible with virtual devices.", path=[key])

    for key in [CONF_ON_INCOMING_CALL, CONF_ON_CALL_STARTED, CONF_ON_CALL_ENDED, CONF_ON_CALL_FAILED]:
        if key in config and device_group not in ("indoor_station", "outdoor_station"):
            raise cv.Invalid(f"'{key}' is only compatible with virtual 'indoor_station' and 'outdoor_station' devices.", path=[key])

    return config

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID() : cv.declare_id(TCBusDeviceComponent),
        cv.GenerateID(CONF_TC_BUS_ID): cv.use_id(TCBusComponent),
        cv.Optional(CONF_VIRTUAL, default="false"): cv.boolean,
        cv.Optional(CONF_TYPE, default="indoor_station"): cv.enum(DEVICE_GROUPS, upper=False),
        cv.Optional(CONF_AUTO_CONFIGURATION): cv.boolean,

        cv.Optional(CONF_ON_READ_MEMORY_COMPLETE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadMemoryCompleteTrigger),
            }
        ),
        cv.Optional(CONF_ON_READ_MEMORY_FAILED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadMemoryTimeoutTrigger),
            }
        ),
        cv.Optional(CONF_ON_IDENTIFY_COMPLETE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IdentifyCompleteTrigger),
            }
        ),
        cv.Optional(CONF_ON_IDENTIFY_UNKNOWN): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IdentifyUnknownTrigger),
            }
        ),
        cv.Optional(CONF_ON_IDENTIFY_FAILED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(IdentifyTimeoutTrigger),
            }
        ),

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
        )
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

    if CONF_TYPE in config:
        cg.add(var.set_device_group(config[CONF_TYPE]))

    if CONF_VIRTUAL in config:
        cg.add(var.set_virtual(config[CONF_VIRTUAL]))

    if CONF_AUTO_CONFIGURATION in config:
        cg.add(var.set_auto_configuration(config[CONF_AUTO_CONFIGURATION]))

    if CONF_ON_READ_MEMORY_COMPLETE in config:
        cg.add_define("USE_READ_MEMORY_COMPLETE_CALLBACK")
        for conf in config.get(CONF_ON_READ_MEMORY_COMPLETE, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [], conf)

    if CONF_ON_READ_MEMORY_FAILED in config:
        cg.add_define("USE_READ_MEMORY_FAILED_CALLBACK")
        for conf in config.get(CONF_ON_READ_MEMORY_FAILED, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [], conf)

    if CONF_ON_IDENTIFY_COMPLETE in config:
        cg.add_define("USE_IDENTIFY_COMPLETE_CALLBACK")
        for conf in config.get(CONF_ON_IDENTIFY_COMPLETE, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [(ModelData, "x")], conf)

    if CONF_ON_IDENTIFY_UNKNOWN in config:
        cg.add_define("USE_IDENTIFY_UNKNOWN_CALLBACK")
        for conf in config.get(CONF_ON_IDENTIFY_UNKNOWN, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [], conf)

    if CONF_ON_IDENTIFY_FAILED in config:
        cg.add_define("USE_IDENTIFY_FAILED_CALLBACK")
        for conf in config.get(CONF_ON_IDENTIFY_FAILED, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [], conf)

    if CONF_ON_INCOMING_CALL in config:
        cg.add_define("USE_INCOMING_CALL_CALLBACK")
        for conf in config.get(CONF_ON_INCOMING_CALL, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    if CONF_ON_CALL_STARTED in config:
        cg.add_define("USE_CALL_STARTED_CALLBACK")
        for conf in config.get(CONF_ON_CALL_STARTED, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    if CONF_ON_CALL_ENDED in config:
        cg.add_define("USE_CALL_ENDED_CALLBACK")
        for conf in config.get(CONF_ON_CALL_ENDED, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [(TelegramData, "x")], conf)

    if CONF_ON_CALL_FAILED in config:
        cg.add_define("USE_CALL_FAILED_CALLBACK")
        for conf in config.get(CONF_ON_CALL_FAILED, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [], conf)


TC_BUS_DEVICE_SEND_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
        cv.Required(CONF_TYPE): cv.templatable(cv.enum(TELEGRAM_TYPES, upper=False)),
        cv.Optional(CONF_ADDRESS, default="0"): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_PAYLOAD, default="0"): cv.templatable(cv.hex_uint32_t)
    })
)

@automation.register_action(
    "tc_bus_device.send",
    TCBusDeviceSendAction,
    TC_BUS_DEVICE_SEND_SCHEMA,
    synchronous=True
)
async def tc_bus_device_send_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    type_template = await cg.templatable(config[CONF_TYPE], args, TELEGRAM_TYPE)
    cg.add(var.set_type(type_template))

    address_template = await cg.templatable(config[CONF_ADDRESS], args, cg.uint8)
    cg.add(var.set_address(address_template))

    payload_template = await cg.templatable(config[CONF_PAYLOAD], args, cg.uint32)
    cg.add(var.set_payload(payload_template))

    return var

TC_BUS_DEVICE_UPDATE_SETTING_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
        cv.Required(CONF_TYPE): cv.templatable(cv.enum(SETTING_TYPES, upper=False)),
        cv.Required(CONF_VALUE): cv.templatable(cv.hex_uint32_t)
    })
)

@automation.register_action(
    "tc_bus_device.update_setting",
    TCBusDeviceUpdateSettingAction,
    TC_BUS_DEVICE_UPDATE_SETTING_SCHEMA,
    synchronous=True
)
async def tc_bus_device_update_setting_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    type_template = await cg.templatable(config[CONF_TYPE], args, SETTING_TYPE)
    cg.add(var.set_type(type_template))

    value_template = await cg.templatable(config[CONF_VALUE], args, cg.uint32)
    cg.add(var.set_value(value_template))

    return var

TC_BUS_DEVICE_UPDATE_DOORBELL_BUTTON_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
        cv.Required(CONF_BUTTON_ROW): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_BUTTON_COL, default="1"): cv.templatable(cv.hex_uint8_t),
        cv.Optional(CONF_PRIMARY_ACTION): cv.templatable(cv.enum(DOORBELL_BUTTON_ACTIONS, upper=False)),
        cv.Optional(CONF_PRIMARY_PAYLOAD): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_SECONDARY_ACTION): cv.templatable(cv.enum(DOORBELL_BUTTON_ACTIONS, upper=False)),
        cv.Optional(CONF_SECONDARY_PAYLOAD): cv.templatable(cv.hex_uint32_t),
    })
)

@automation.register_action(
    "tc_bus_device.update_doorbell_button",
    TCBusDeviceUpdateDoorbellButtonAction,
    TC_BUS_DEVICE_UPDATE_DOORBELL_BUTTON_SCHEMA,
    synchronous=True
)
async def tc_bus_device_update_doorbell_button_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    button_row_template = await cg.templatable(config[CONF_BUTTON_ROW], args, cg.uint8)
    cg.add(var.set_button_row(button_row_template))

    button_col_template = await cg.templatable(config[CONF_BUTTON_COL], args, cg.uint8)
    cg.add(var.set_button_col(button_col_template))

    if CONF_PRIMARY_ACTION in config:
        primary_action_template = await cg.templatable(config[CONF_PRIMARY_ACTION], args, DOORBELL_BUTTON_ACTION)
        cg.add(var.set_primary_action(primary_action_template))

    if CONF_PRIMARY_PAYLOAD in config:
        primary_payload_template = await cg.templatable(config[CONF_PRIMARY_PAYLOAD], args, cg.uint32)
        cg.add(var.set_primary_payload(primary_payload_template))

    if CONF_SECONDARY_ACTION in config:
        secondary_action_template = await cg.templatable(config[CONF_SECONDARY_ACTION], args, DOORBELL_BUTTON_ACTION)
        cg.add(var.set_secondary_action(secondary_action_template))

    if CONF_SECONDARY_PAYLOAD in config:
        secondary_payload_template = await cg.templatable(config[CONF_SECONDARY_PAYLOAD], args, cg.uint32)
        cg.add(var.set_secondary_payload(secondary_payload_template))

    return var

@automation.register_action(
    "tc_bus_device.read_memory",
    TCBusDeviceReadMemoryAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent)
        }
    ),
    synchronous=True
)
async def tc_bus_device_read_memory_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])
    
    return var


@automation.register_action(
    "tc_bus_device.identify",
    TCBusDeviceIdentifyAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent)
        }
    ),
    synchronous=True
)
async def tc_bus_device_request_version_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])
    
    return var


@automation.register_action(
    "tc_bus_device.open_door",
    TCBusDeviceOpenDoorAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent)
        }
    ),
    synchronous=True
)
async def tc_bus_device_open_door_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])
    
    return var



TC_BUS_DEVICE_ANSWER_CALL_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
    })
)

@automation.register_action(
    "tc_bus_device.answer_call",
    TCBusDeviceAnswerCallAction,
    TC_BUS_DEVICE_ANSWER_CALL_SCHEMA,
    synchronous=True
)
async def tc_bus_device_answer_call_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    return var


TC_BUS_DEVICE_END_CALL_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
    })
)

@automation.register_action(
    "tc_bus_device.end_call",
    TCBusDeviceEndCallAction,
    TC_BUS_DEVICE_END_CALL_SCHEMA,
    synchronous=True
)
async def tc_bus_device_end_call_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    return var


TC_BUS_DEVICE_CALL_SCHEMA = cv.All(
    cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(TCBusDeviceComponent),
        cv.Optional(CONF_ADDRESS, default="0"): cv.templatable(cv.hex_uint32_t),
        cv.Optional(CONF_INTERNAL, default="false"): cv.templatable(cv.boolean),
    })
)

@automation.register_action(
    "tc_bus_device.call",
    TCBusDeviceCallAction,
    TC_BUS_DEVICE_CALL_SCHEMA,
    synchronous=True
)
async def tc_bus_device_call_to_code(config, action_id, template_args, args):
    var = cg.new_Pvariable(action_id, template_args)
    await cg.register_parented(var, config[CONF_ID])

    address_template = await cg.templatable(config[CONF_ADDRESS], args, cg.uint32)
    cg.add(var.set_address(address_template))

    internal_template = await cg.templatable(config[CONF_INTERNAL], args, cg.bool_)
    cg.add(var.set_internal(internal_template))

    return var