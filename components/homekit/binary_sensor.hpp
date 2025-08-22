#pragma once
#include <esphome/core/defines.h>
#ifdef USE_BINARY_SENSOR
#include <map>
#include <esphome/core/application.h>
#include <hap.h>
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>
#include "esphome/components/homekit_bridge/const.h"
#include "esphome/components/homekit_bridge/util.h"
#include "automation.h"

namespace esphome
{
  namespace homekit
  {
    class BinarySensorEntity
    {
      static std::unordered_map<hap_acc_t*, BinarySensorEntity*> acc_instance_map;

    private:
      static constexpr const char* TAG = "homekit.binary_sensor";
      binary_sensor::BinarySensor* entityPtr;

      void on_entity_update(binary_sensor::BinarySensor* obj, bool new_state) {
        ESP_LOGD(TAG, "%s value: %s", obj->get_name().c_str(), ONOFF(new_state));

        hap_acc_t* acc = hap_acc_get_by_aid(hap_get_unique_aid(std::to_string(obj->get_object_id_hash()).c_str()));
        if (!acc) return;

        hap_serv_t* hs = hap_serv_get_next(hap_acc_get_first_serv(acc));
        if (!hs) return;
          
        hap_char_t* on_char = hap_serv_get_first_char(hs);
        if (!on_char) return;

        ESP_LOGD(TAG, "HAP CURRENT VALUE: %s", ONOFF(hap_char_get_val(on_char)->u));

        hap_val_t state;
        state.u = new_state ? 1 : 0;
        hap_char_update_val(on_char, &state);



        hap_char_t* active_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_ON);
        if (!active_char) return;

        hap_val_t state2;
        state2.b = new_state;
        hap_char_update_val(active_char, &state2);
      }

      static int binary_sensor_read(hap_char_t* hc, hap_status_t* status_code, void* serv_priv, void* read_priv) {
        if (serv_priv) {
          binary_sensor::BinarySensor* entityPtr = (binary_sensor::BinarySensor*)serv_priv;
          ESP_LOGD(TAG, "Read called for Accessory %s (%s)", std::to_string(entityPtr->get_object_id_hash()).c_str(), entityPtr->get_name().c_str());
          
          hap_val_t sensorValue;
          sensorValue.u = entityPtr->state ? 1 : 0;
          hap_char_update_val(hc, &sensorValue);
          
          return HAP_SUCCESS;
        }
        return HAP_FAIL;
      }

      static int acc_identify(hap_acc_t* ha) {
        auto it = acc_instance_map.find(ha);
        if (it != acc_instance_map.end()) {
            it->second->on_identify();
        }
        return HAP_SUCCESS;
      }

      void on_identify() {
        ESP_LOGD(TAG, "Accessory identified");
        for (auto* trig : triggers_identify_) {
          if (trig) trig->trigger();
        }
      }

      std::vector<HKIdentifyTrigger *> triggers_identify_;

    public:
      BinarySensorEntity(binary_sensor::BinarySensor* entityPtr) : entityPtr(entityPtr) {}

      binary_sensor::BinarySensor* getEntity() {
        return entityPtr;
      }

      void register_on_identify_trigger(HKIdentifyTrigger* trig) {
          triggers_identify_.push_back(trig);
      }

      void setup() {
        ESP_LOGCONFIG(TAG, "Setting up binary_sensor '%s'", entityPtr->get_name().c_str());

        hap_serv_t* service = nullptr;

        std::string device_class = entityPtr->get_device_class();
        if (device_class == "motion") {
          service = hap_serv_motion_sensor_create(entityPtr->state);
        }
        else if (device_class == "door") {
          service = hap_serv_contact_sensor_create(entityPtr->state);
        }
        else if (device_class == "window") {
          service = hap_serv_contact_sensor_create(entityPtr->state);
        }
        else if (device_class == "occupancy") {
          service = hap_serv_occupancy_sensor_create(entityPtr->state);
        }
        else if (device_class == "moisture") {
          service = hap_serv_leak_sensor_create(entityPtr->state);
        }
        else if (device_class == "carbon_monoxide") {
          service = hap_serv_carbon_dioxide_sensor_create(entityPtr->state);
        }
        else if (device_class == "smoke") {
          service = hap_serv_smoke_sensor_create(entityPtr->state);
        } else {
          service = hap_serv_contact_sensor_create(entityPtr->state);
        }

        if (service) {
          hap_acc_cfg_t acc_cfg = {
            .name = strdup_psram(entityPtr->get_name().c_str()),
            .model = (char*)"ESPHome Binary Sensor",
            .manufacturer = (char*)"ESPHome",
            .serial_num = strdup_psram(std::to_string(entityPtr->get_object_id_hash()).c_str()),
            .fw_rev = (char*)"1.0.0",
            .hw_rev = NULL,
            .pv = (char*)"1.1.0",
            .cid = HAP_CID_BRIDGE,
            .identify_routine = acc_identify,
          };

          hap_acc_t* accessory = hap_acc_create(&acc_cfg);
          acc_instance_map[accessory] = this;

          ESP_LOGD(TAG, "ID HASH: %lu", entityPtr->get_object_id_hash());
          hap_serv_set_priv(service, entityPtr);

          /* Set the read callback for the service */
          hap_serv_set_read_cb(service, binary_sensor_read);

          /* Add the Sensor Service to the Accessory Object */
          hap_acc_add_serv(accessory, service);

          /* Add the Accessory to the HomeKit Database */
          hap_add_bridged_accessory(accessory, hap_get_unique_aid(std::to_string(entityPtr->get_object_id_hash()).c_str()));

          if (!entityPtr->is_internal())
            entityPtr->add_on_state_callback([this](bool b) { this->on_entity_update(entityPtr, b); });

          ESP_LOGI(TAG, "Binary Sensor '%s' linked to HomeKit", entityPtr->get_name().c_str());
        }
      }
    };

    inline std::unordered_map<hap_acc_t*, BinarySensorEntity*> BinarySensorEntity::acc_instance_map;
  }
}
#endif