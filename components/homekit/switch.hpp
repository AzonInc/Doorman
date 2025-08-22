#pragma once
#include <esphome/core/defines.h>
#ifdef USE_SWITCH
#include <esphome/core/application.h>
#include <hap.h>
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>
#include <map>
#include <unordered_map>
#include "esphome/components/homekit_bridge/const.h"
#include "esphome/components/homekit_bridge/util.h"
#include "automation.h"

namespace esphome
{
  namespace homekit
  {
    class SwitchEntity
    {
      static std::unordered_map<hap_acc_t*, SwitchEntity*> acc_instance_map;

    private:
      static constexpr const char* TAG = "homekit.switch";
      switch_::Switch* entityPtr;

      static int switch_write(hap_write_data_t write_data[], int count, void* serv_priv, void* write_priv) {
        switch_::Switch* entityPtr = (switch_::Switch*)serv_priv;
        ESP_LOGD(TAG, "Write called for Accessory %s (%s)", std::to_string(entityPtr->get_object_id_hash()).c_str(), entityPtr->get_name().c_str());
        int i, ret = HAP_SUCCESS;
        hap_write_data_t* write;
        for (i = 0; i < count; i++) {
          write = &write_data[i];
          if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_ON)) {
            ESP_LOGD(TAG, "Received Write for switch '%s' -> %s", entityPtr->get_name().c_str(), write->val.b ? "On" : "Off");
            ESP_LOGD(TAG, "[STATE] CURRENT STATE: %d", entityPtr->state);
            write->val.b ? entityPtr->turn_on() : entityPtr->turn_off();
            hap_char_update_val(write->hc, &(write->val));
            *(write->status) = HAP_STATUS_SUCCESS;
          }
          else {
            *(write->status) = HAP_STATUS_RES_ABSENT;
          }
        }
        return ret;
      }

      void on_entity_update(switch_::Switch* obj, bool v) {
        ESP_LOGD(TAG, "%s state: %d", obj->get_name().c_str(), v);
        
        hap_acc_t* acc = hap_acc_get_by_aid(hap_get_unique_aid(std::to_string(obj->get_object_id_hash()).c_str()));
        if (!acc) return;

        hap_serv_t* hs = hap_acc_get_serv_by_uuid(acc, HAP_SERV_UUID_SWITCH);
        if (!hs) return;
          
        hap_char_t* on_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_ON);
        if (!on_char) return;

        hap_val_t state;
        state.b = v;
        hap_char_update_val(on_char, &state);
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
      SwitchEntity(switch_::Switch* entityPtr) : entityPtr(entityPtr) {}

      void register_on_identify_trigger(HKIdentifyTrigger* trig) {
          triggers_identify_.push_back(trig);
      }

      switch_::Switch* getEntity() {
        return entityPtr;
      }

      void setup() {
        ESP_LOGCONFIG(TAG, "Setting up switch '%s'", entityPtr->get_name().c_str());

        hap_acc_cfg_t acc_cfg = {
          .name = strdup_psram(entityPtr->get_name().c_str()),
          .model = (char*)"ESPHome Switch",
          .manufacturer = (char*)"ESPHome",
          .serial_num = strdup_psram(std::to_string(entityPtr->get_object_id_hash()).c_str()),
          .fw_rev = (char*)"1.0.0",
          .hw_rev = NULL,
          .pv = (char*)"1.1.0",
          .cid = HAP_CID_BRIDGE,
          .identify_routine = acc_identify,
        };

        /* Create accessory object */
        hap_acc_t* accessory = hap_acc_create(&acc_cfg);
        acc_instance_map[accessory] = this;
        
        /* Create the switch Service. */
        hap_serv_t* service = hap_serv_switch_create(entityPtr->state);

        ESP_LOGD(TAG, "ID HASH: %lu", entityPtr->get_object_id_hash());
        hap_serv_set_priv(service, entityPtr);

        /* Set the write callback for the service */
        hap_serv_set_write_cb(service, switch_write);

        /* Add the Switch Service to the Accessory Object */
        hap_acc_add_serv(accessory, service);

        /* Add the Accessory to the HomeKit Database */
        hap_add_bridged_accessory(accessory, hap_get_unique_aid(std::to_string(entityPtr->get_object_id_hash()).c_str()));

        if (!entityPtr->is_internal())
          entityPtr->add_on_state_callback([this](bool v) { this->on_entity_update(entityPtr, v); });

        ESP_LOGI(TAG, "Switch '%s' linked to HomeKit", entityPtr->get_name().c_str());
      }
    };

    inline std::unordered_map<hap_acc_t*, SwitchEntity*> SwitchEntity::acc_instance_map;
  }
}
#endif