#pragma once
#include <esphome/core/defines.h>
#ifdef USE_EVENT
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
    class EventEntity
    {
      static std::unordered_map<hap_acc_t*, EventEntity*> acc_instance_map;

    private:
      static constexpr const char* TAG = "homekit.event";
      event::Event* entityPtr;

      void on_entity_update(event::Event* obj, std::string event_type) {
        ESP_LOGD(TAG, "%s value: %s", obj->get_name().c_str(), event_type.c_str());

        hap_acc_t* acc = hap_acc_get_by_aid(hap_get_unique_aid(std::to_string(obj->get_object_id_hash()).c_str()));
        if (!acc) return;

        hap_serv_t* hs = hap_acc_get_serv_by_uuid(acc, HAP_SERV_UUID_STATLESS_PROGRAMMABLE_SWITCH);
        if (!hs) return;
          
        hap_char_t* event_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_PROGRAMMABLE_SWITCH_EVENT);
        if (!event_char) return;

        hap_val_t val = {.u = 0};

        // Map internal event_type to HomeKit event
        if (event_type == "single") {
          val.u = 0; // SINGLE_PRESS
        } else if (event_type == "double") {
          val.u = 1; // DOUBLE_PRESS
        } else if (event_type == "long") {
          val.u = 2; // LONG_PRESS
        } else {
          ESP_LOGW(TAG, "Unknown event_type '%s', fallback to 'long_press'", event_type.c_str());
          val.u = 2; // LONG_PRESS
        }
        
        int ret = hap_char_update_val(event_char, &val);
        ESP_LOGI(TAG, "HomeKit Stateless Switch Event sent: %d, result: %d", val.u, ret);
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
      EventEntity(event::Event* entityPtr) : entityPtr(entityPtr) {}

      event::Event* getEntity() {
        return entityPtr;
      }

      void register_on_identify_trigger(HKIdentifyTrigger* trig) {
          triggers_identify_.push_back(trig);
      }

      void setup() {
        ESP_LOGCONFIG(TAG, "Setting up event '%s'", entityPtr->get_name().c_str());

        hap_serv_t* service = nullptr;
        
        std::string device_class = entityPtr->get_device_class();
        if (device_class == "doorbell") {
          service = hap_serv_create("121");
          hap_serv_add_char(service, hap_char_programmable_switch_event_create(0));
        } else {
          service = hap_serv_stateless_programmable_switch_create(0);
        }
        
        if (service) {
          const uint8_t _validVals[] = {0,1,2};
          hap_char_add_valid_vals(hap_serv_get_char_by_uuid(service, HAP_CHAR_UUID_PROGRAMMABLE_SWITCH_EVENT), _validVals, 3);

          hap_acc_cfg_t acc_cfg = {
            .name = strdup_psram(entityPtr->get_name().c_str()),
            .model = (char*)"ESPHome Event",
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
          hap_acc_add_serv(accessory, service);
          
          /* Add the Accessory to the HomeKit Database */
          hap_add_bridged_accessory(accessory, hap_get_unique_aid(std::to_string(entityPtr->get_object_id_hash()).c_str()));

          if (!entityPtr->is_internal())
              entityPtr->add_on_event_callback([this](std::string event_type) { this->on_entity_update(entityPtr, event_type); });

          ESP_LOGI(TAG, "Event '%s' linked to HomeKit", entityPtr->get_name().c_str());
        }
      }
    };

    inline std::unordered_map<hap_acc_t*, EventEntity*> EventEntity::acc_instance_map;
  }
}
#endif