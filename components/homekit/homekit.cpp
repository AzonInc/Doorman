#include "homekit.h"
#include "esphome/components/homekit_bridge/const.h"
#include "esphome/components/homekit_bridge/util.h"

namespace esphome
{
  namespace homekit
  {
    void HAPAccessory::setup() {

      ESP_LOGCONFIG(TAG, "Running setup");

      if(this->base_component_->is_failed()) {
        std::string failed_msg = "HomeKit Base failed to setup!";
        this->mark_failed(failed_msg.c_str());
        return;
      }

      if(!this->base_component_->is_ready()) {
        std::string not_ready_msg = "HomeKit Base is not setup yet!";
        this->mark_failed(not_ready_msg.c_str());
        return;
      }

      this->base_component_->report_heap();

      #ifdef USE_LIGHT
      for (const auto v : lights) {
        v->setup();
      }
      #endif

      #ifdef USE_LOCK
      for (const auto &v : locks) {
        v->setup();
      }
      #endif

      #ifdef USE_FAN
      for (const auto v : fans) {
        v->setup();
      }
      #endif

      #ifdef USE_SWITCH
      for (const auto v : switches) {
        v->setup();
      }
      #endif

      #ifdef USE_SENSOR
      for (const auto v : sensors) {
        v->setup();
      }
      #endif

      #ifdef USE_BINARY_SENSOR
      for (const auto v : binary_sensors) {
        v->setup();
      }
      #endif

      #ifdef USE_EVENT
      for (const auto v : events) {
        v->setup();
      }

      for (const auto &v : doorbells) {
        v->setup();
      }
      #endif

      #ifdef USE_BUTTON
      for (const auto v : buttons) {
        v->setup();
      }
      #endif

      #ifdef USE_CLIMATE
      for (const auto v : climates) {
        v->setup();
      }
      #endif

      this->base_component_->report_heap();
    }
    void HAPAccessory::dump_config() {
      ESP_LOGCONFIG(TAG, "homekit:");

      #ifdef USE_LOCK
      if (!locks.empty()) {
        ESP_LOGCONFIG(TAG, "  Locks:");
        for (const auto v : locks) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif

      #ifdef USE_LIGHT
      if (!lights.empty()) {
        ESP_LOGCONFIG(TAG, "  Lights:");
        for (const auto v : lights) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif

      #ifdef USE_SENSOR
      if (!sensors.empty()) {
        ESP_LOGCONFIG(TAG, "  Sensors:");
        for (const auto v : sensors) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif

      #ifdef USE_BINARY_SENSOR
      if (!binary_sensors.empty()) {
        ESP_LOGCONFIG(TAG, "  Binary Sensors:");
        for (const auto v : binary_sensors) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif

      #ifdef USE_EVENT
      if (!events.empty()) {
        ESP_LOGCONFIG(TAG, "  Events:");
        for (const auto v : events) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }

      if (!doorbells.empty()) {
        ESP_LOGCONFIG(TAG, "  Doorbells:");
        for (const auto v : doorbells) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEventEntity()->get_name().c_str());
          if (v->getLockEntity() != nullptr) {
            ESP_LOGCONFIG(TAG, "      '%s'", v->getLockEntity()->get_name().c_str());
          }
          if (v->getLightEntity() != nullptr) {
            ESP_LOGCONFIG(TAG, "      '%s'", v->getLightEntity()->get_name().c_str());
          }
        }
      }
      #endif

      #ifdef USE_BUTTON
      if (!buttons.empty()) {
        ESP_LOGCONFIG(TAG, "  Buttons:");
        for (const auto v : buttons) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif

      #ifdef USE_FAN
      if (!fans.empty()) {
        ESP_LOGCONFIG(TAG, "  Fans:");
        for (const auto v : fans) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif

      #ifdef USE_SWITCH
      if (!switches.empty()) {
        ESP_LOGCONFIG(TAG, "  Switches:");
        for (const auto v : switches) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif

      #ifdef USE_CLIMATE
      if (!climates.empty()) {
        ESP_LOGCONFIG(TAG, "  Climates:");
        for (const auto v : climates) {
          ESP_LOGCONFIG(TAG, "    '%s'", v->getEntity()->get_name().c_str());
        }
      }
      #endif
    }

    #ifdef USE_LIGHT
    LightEntity* HAPAccessory::add_light(light::LightState* lightPtr) {
      lights.push_back(new LightEntity(lightPtr));
      return lights.back();
    }
    #endif

    #ifdef USE_EVENT
    EventEntity* HAPAccessory::add_event(event::Event* eventPtr) {
      events.push_back(new EventEntity(eventPtr));
      return events.back();
    }

    DoorbellEntity* HAPAccessory::add_doorbell(event::Event* eventPtr, lock::Lock* lockPtr, light::LightState* lightPtr) {
      doorbells.push_back(new DoorbellEntity(eventPtr, lockPtr, lightPtr));
      return doorbells.back();
    }
    #endif

    #ifdef USE_LOCK
    LockEntity* HAPAccessory::add_lock(lock::Lock* lockPtr) {
      locks.push_back(new LockEntity(lockPtr));
      return locks.back();
    }
    #endif

    #ifdef USE_FAN
    FanEntity* HAPAccessory::add_fan(fan::Fan* fanPtr) {
      fans.push_back(new FanEntity(fanPtr));
      return fans.back();
    }
    #endif

    #ifdef USE_SWITCH
    SwitchEntity* HAPAccessory::add_switch(switch_::Switch* switchPtr) {
      switches.push_back(new SwitchEntity(switchPtr));
      return switches.back();
    }
    #endif

    #ifdef USE_SENSOR
    SensorEntity* HAPAccessory::add_sensor(sensor::Sensor* sensorPtr, TemperatureUnits units) {
      sensors.push_back(new SensorEntity(sensorPtr));
      return sensors.back();
    }
    #endif

    #ifdef USE_BINARY_SENSOR
    BinarySensorEntity* HAPAccessory::add_binary_sensor(binary_sensor::BinarySensor* binarySensorPtr) {
      binary_sensors.push_back(new BinarySensorEntity(binarySensorPtr));
      return binary_sensors.back();
    }
    #endif

    #ifdef USE_BUTTON
    ButtonEntity* HAPAccessory::add_button(button::Button* buttonPtr) {
      buttons.push_back(new ButtonEntity(buttonPtr));
      return buttons.back();
    }
    #endif

    #ifdef USE_CLIMATE
    ClimateEntity* HAPAccessory::add_climate(climate::Climate* climatePtr) {
      climates.push_back(new ClimateEntity(climatePtr));
      return climates.back();
    }
    #endif
  }
}