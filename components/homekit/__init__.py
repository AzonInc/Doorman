from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, lock, sensor, binary_sensor, event, button, switch, climate, fan
from esphome.const import PLATFORM_ESP32, CONF_ID, CONF_TRIGGER_ID, CONF_LIGHT, CONF_SENSOR, CONF_BINARY_SENSOR, CONF_BUTTON, CONF_EVENT
from esphome.core import ID
from .. import homekit_bridge

AUTO_LOAD = ["homekit_bridge"]
DEPENDENCIES = ['esp32', 'network', 'homekit_bridge']
CODEOWNERS = ["@rednblkx"]

homekit_ns = homekit_bridge.homekit_ns
HomeKitBridgeComponent = homekit_bridge.HomeKitBridgeComponent

TemperatureUnits = homekit_ns.enum("TemperatureUnits")
AInfo = homekit_ns.enum("AInfo")
HKFinish = homekit_ns.enum("HKFinish")
HAPAccessory = homekit_ns.class_('HAPAccessory', cg.Component)
LightEntity = homekit_ns.class_('LightEntity')
SensorEntity = homekit_ns.class_('SensorEntity')
BinarySensorEntity = homekit_ns.class_('BinarySensorEntity')
EventEntity = homekit_ns.class_('EventEntity')
ButtonEntity = homekit_ns.class_('ButtonEntity')
SwitchEntity = homekit_ns.class_('SwitchEntity')
ClimateEntity = homekit_ns.class_('ClimateEntity')
LockEntity = homekit_ns.class_('LockEntity')
FanEntity = homekit_ns.class_('FanEntity')
DoorbellEntity = homekit_ns.class_('DoorbellEntity')

OnHKIdentifyTrigger = homekit_ns.class_(
    "HKIdentifyTrigger", automation.Trigger.template()
)

CONF_IDENTIFY = "on_identify"
CONF_HK_SUCCESS = "on_hk_success"
CONF_HK_FAIL = "on_hk_fail"

CONF_LOCK = "lock"
CONF_FAN = "fan"
CONF_SWITCH = "switch"
CONF_DOORBELL = "doorbell"
CONF_LOCK_ID = "lock_id"
CONF_LIGHT_ID = "light_id"
CONF_CLIMATE = "climate"
CONF_TEMP_UNITS = "temp_units"

CONF_HOMEKIT_BRIDGE = "homekit_bridge_id"

TEMP_UNITS = {
    "CELSIUS": TemperatureUnits.CELSIUS,
    "FAHRENHEIT": TemperatureUnits.FAHRENHEIT
}

CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.GenerateID() : cv.declare_id(HAPAccessory),
    cv.GenerateID(CONF_HOMEKIT_BRIDGE): cv.use_id(
        homekit_bridge.HomeKitBridgeComponent
    ),
    cv.Optional(CONF_LIGHT): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(light.LightState),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_LOCK): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(lock.Lock),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_DOORBELL): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(event.Event),
        cv.Optional(CONF_LOCK_ID): cv.use_id(lock.Lock),
        cv.Optional(CONF_LIGHT_ID): cv.use_id(light.LightState),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_SENSOR): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_TEMP_UNITS, default="CELSIUS") : cv.enum(TEMP_UNITS),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_BINARY_SENSOR): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_EVENT): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(event.Event),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_BUTTON): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(button.Button),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_FAN): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(fan.Fan),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_SWITCH): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(switch.Switch),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
    cv.Optional(CONF_CLIMATE): cv.ensure_list({
        cv.Required(CONF_ID): cv.use_id(climate.Climate),
        cv.Optional(CONF_IDENTIFY): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnHKIdentifyTrigger),
        }),
    }),
}).extend(cv.COMPONENT_SCHEMA),
cv.only_on([PLATFORM_ESP32]),
cv.only_with_esp_idf)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    homekit_bridge_component = await cg.get_variable(config[CONF_HOMEKIT_BRIDGE])
    cg.add(var.set_base_component(homekit_bridge_component))

    if 'light' in config:
        for l in config["light"]:
            light_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_light_entity", type=LightEntity), var.add_light(await cg.get_variable(l['id'])))

            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(light_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if 'sensor' in config:
        for l in config["sensor"]:
            sensor_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_sensor_entity", type=SensorEntity), var.add_sensor(await cg.get_variable(l['id']), l['temp_units']))

            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(sensor_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if 'binary_sensor' in config:
        for l in config["binary_sensor"]:
            binary_sensor_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_binary_sensor_entity", type=BinarySensorEntity), var.add_binary_sensor(await cg.get_variable(l['id'])))
            
            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(binary_sensor_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if 'event' in config:
        for l in config["event"]:
            event_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_event_entity", type=EventEntity), var.add_event(await cg.get_variable(l['id'])))
            
            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(event_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if 'button' in config:
        for l in config["button"]:
            button_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_button_entity", type=ButtonEntity), var.add_button(await cg.get_variable(l['id'])))

            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(button_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if 'lock' in config:
        for l in config["lock"]:
            lock_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_lock_entity", type=LockEntity), var.add_lock(await cg.get_variable(l['id'])))

            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(lock_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if 'doorbell' in config:
        for l in config["doorbell"]:
            event_ptr = await cg.get_variable(l['id'])
            lock_ptr = await cg.get_variable(l[CONF_LOCK_ID]) if CONF_LOCK_ID in l else cg.nullptr
            light_ptr = await cg.get_variable(l[CONF_LIGHT_ID]) if CONF_LIGHT_ID in l else cg.nullptr

            doorbell_entity = cg.Pvariable(
                ID(f"{l['id'].id}_hk_doorbell_entity", type=DoorbellEntity),
                var.add_doorbell(event_ptr, lock_ptr, light_ptr)
            )

            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(doorbell_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if "fan" in config:
        for l in config["fan"]:
            fan_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_fan_entity", type=FanEntity), var.add_fan(await cg.get_variable(l['id'])))

            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(fan_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if "switch" in config:
        for l in config["switch"]:
            switch_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_switch_entity", type=SwitchEntity), var.add_switch(await cg.get_variable(l['id'])))
            
            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(switch_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)

    if "climate" in config:
        for l in config["climate"]:
            climate_entity = cg.Pvariable(ID(f"{l['id'].id}_hk_climate_entity", type=ClimateEntity), var.add_climate(await cg.get_variable(l['id'])))

            for conf in l.get(CONF_IDENTIFY, []):
                trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
                cg.add(climate_entity.register_on_identify_trigger(trigger))
                await automation.build_automation(trigger, [], conf)
