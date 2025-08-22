#pragma once
#include <esp_log.h>
#include <esphome/core/log.h>
#include <esphome/core/defines.h>
#include <esphome/core/component.h>
#include "esphome/components/homekit_bridge/homekit_bridge.h"
#include "esphome/components/homekit_bridge/const.h"
#include "esphome/components/homekit_bridge/util.h"

#ifdef USE_LIGHT
#include "light.hpp"
#endif

#ifdef USE_LOCK
#include "lock.hpp"
#endif

#ifdef USE_FAN
#include "fan.hpp"
#endif

#ifdef USE_SWITCH
#include "switch.hpp"
#endif

#ifdef USE_SENSOR
#include "sensor.hpp"
#endif

#ifdef USE_BINARY_SENSOR
#include "binary_sensor.hpp"
#endif

#ifdef USE_EVENT
#include "event.hpp"
#include "doorbell.hpp"
#endif

#ifdef USE_BUTTON
#include "button.hpp"
#endif

#ifdef USE_CLIMATE
#include "climate.hpp"
#endif

namespace esphome
{
  namespace homekit
  {
    class HAPAccessory : public Component
    {
    public:
      const char* TAG = "homekit";

      float get_setup_priority() const override { return setup_priority::AFTER_CONNECTION; }
      void setup() override;
      void dump_config() override;

      void set_base_component(homekit::HomeKitBridgeComponent *base_component) { this->base_component_ = base_component; }

      #ifdef USE_LIGHT
      std::vector<LightEntity*> lights;
      LightEntity* add_light(light::LightState* lightPtr);
      #endif

      #ifdef USE_LOCK
      std::vector<LockEntity*> locks;
      LockEntity* add_lock(lock::Lock* lockPtr);
      #endif

      #ifdef USE_FAN
      std::vector<FanEntity*> fans;
      FanEntity* add_fan(fan::Fan* fanPtr);
      #endif

      #ifdef USE_SWITCH
      std::vector<SwitchEntity*> switches;
      SwitchEntity* add_switch(switch_::Switch* switchPtr);
      #endif

      #ifdef USE_SENSOR
      std::vector<SensorEntity*> sensors;
      SensorEntity* add_sensor(sensor::Sensor* sensorPtr, TemperatureUnits units);
      #endif

      #ifdef USE_BINARY_SENSOR
      std::vector<BinarySensorEntity*> binary_sensors;
      BinarySensorEntity* add_binary_sensor(binary_sensor::BinarySensor* binarySensorPtr);
      #endif

      #ifdef USE_EVENT
      std::vector<EventEntity*> events;
      EventEntity* add_event(event::Event* eventPtr);

      std::vector<DoorbellEntity*> doorbells;
      DoorbellEntity* add_doorbell(event::Event* eventPtr, lock::Lock* lockPtr = nullptr, light::LightState* lightPtr = nullptr);
      #endif

      #ifdef USE_BUTTON
      std::vector<ButtonEntity*> buttons;
      ButtonEntity* add_button(button::Button* buttonPtr);
      #endif

      #ifdef USE_CLIMATE
      std::vector<ClimateEntity*> climates;
      ClimateEntity* add_climate(climate::Climate* climatePtr);
      #endif

    protected:
      homekit::HomeKitBridgeComponent *base_component_{nullptr};
    };
  }
}