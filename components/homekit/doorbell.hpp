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
    class DoorbellEntity
    {
      static std::unordered_map<hap_acc_t*, DoorbellEntity*> acc_instance_map;

    private:
      static constexpr const char* TAG = "homekit.doorbell";
      event::Event* eventPtr;
      lock::Lock* lockPtr;
      light::LightState* lightPtr;
      hap_acc_t* accessory_ = nullptr;

      void on_event_update(event::Event* event_entity, std::string event_type) {
        ESP_LOGD(TAG, "%s value: %s", event_entity->get_name().c_str(), event_type.c_str());

        hap_acc_t* acc = accessory_;
        if (!acc) return;

        hap_serv_t* hs = hap_acc_get_serv_by_uuid(acc, HAP_SERV_UUID_DOORBELL);
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

      void on_lock_update(lock::Lock* lock_entity) {
        ESP_LOGD("on_lock_update", "%s state: %s", lock_entity->get_name().c_str(), lock_state_to_string(lock_entity->state));
        
        hap_acc_t* acc = accessory_;
        if (!acc) return;

        hap_serv_t* hs = hap_acc_get_serv_by_uuid(acc, HAP_SERV_UUID_LOCK_MECHANISM);
        if (!hs) return;

        hap_char_t* current_state_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_LOCK_CURRENT_STATE);
        if (!current_state_char) return;

        hap_char_t* target_state_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_LOCK_TARGET_STATE);
        if (!target_state_char) return;

        hap_val_t current_state;
        hap_val_t target_state;
        
        if (lock_entity->state == lock::LockState::LOCK_STATE_LOCKED || lock_entity->state == lock::LockState::LOCK_STATE_UNLOCKED) {
            current_state.i = lock_entity->state % 2;
            target_state.i = lock_entity->state % 2;
            hap_char_update_val(current_state_char, &current_state);
            hap_char_update_val(target_state_char, &target_state);
        } else if (lock_entity->state == lock::LockState::LOCK_STATE_LOCKING || lock_entity->state == lock::LockState::LOCK_STATE_UNLOCKING) {
            target_state.i = (lock_entity->state % 5) % 3;
            hap_char_update_val(target_state_char, &target_state);
        } else if (lock_entity->state == lock::LockState::LOCK_STATE_JAMMED) {
            current_state.i = lock_entity->state;
            hap_char_update_val(current_state_char, &current_state);
        }
      }

      void on_light_update(light::LightState* light_entity) {
        ESP_LOGD(TAG, "%s value: %s", light_entity->get_name().c_str(), ONOFF(light_entity->current_values.is_on()));

        hap_acc_t* acc = accessory_;
        if (!acc) return;

        hap_serv_t* hs = hap_acc_get_serv_by_uuid(acc, HAP_SERV_UUID_LIGHTBULB);
        if (!hs) return;

        hap_char_t* on_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_ON);
        if (!on_char) return;

        hap_val_t state;
        state.b = light_entity->current_values.get_state();
        hap_char_update_val(on_char, &state);
      }


      static int lock_write(hap_write_data_t write_data[], int count, void* serv_priv, void* write_priv) {
        lock::Lock* lockPtr = (lock::Lock*)serv_priv;
        ESP_LOGD("lock_write", "Write called for Accessory '%s'(%s)", lockPtr->get_name().c_str(), std::to_string(lockPtr->get_object_id_hash()).c_str());
        
        int i, ret = HAP_SUCCESS;
        hap_write_data_t* write;
        
        for (i = 0; i < count; i++) {
            write = &write_data[i];
            if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_LOCK_TARGET_STATE)) {
                ESP_LOGD("lock_write", "Target State req: %d", write->val.i);
                hap_char_update_val(write->hc, &(write->val));
                hap_char_t* c = hap_serv_get_char_by_uuid(hap_char_get_parent(write->hc), HAP_CHAR_UUID_LOCK_CURRENT_STATE);
                
                ESP_LOGD("lock_write", "Current State: %d", hap_char_get_val(c)->i);
                hap_char_update_val(c, &(write->val));
                write->val.i ? lockPtr->lock() : lockPtr->unlock();

                *(write->status) = HAP_STATUS_SUCCESS;
            } else {
                *(write->status) = HAP_STATUS_RES_ABSENT;
            }
        }
        return ret;
      }

      static int light_write(hap_write_data_t write_data[], int count, void* serv_priv, void* write_priv) {
        light::LightState* lightPtr = (light::LightState*)serv_priv;
        ESP_LOGD(TAG, "Write called for Accessory %s (%s)", std::to_string(lightPtr->get_object_id_hash()).c_str(), lightPtr->get_name().c_str());
        int i, ret = HAP_SUCCESS;
        hap_write_data_t* write;
        for (i = 0; i < count; i++) {
          write = &write_data[i];
          if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_ON)) {
            ESP_LOGD(TAG, "Received Write for Light '%s' state: %s", lightPtr->get_name().c_str(), write->val.b ? "On" : "Off");
            ESP_LOGD(TAG, "[STATE] CURRENT STATE: %d", (int)(lightPtr->current_values.get_state() * 100));
            write->val.b ? lightPtr->turn_on().set_save(true).perform() : lightPtr->turn_off().set_save(true).perform();
            hap_char_update_val(write->hc, &(write->val));
            *(write->status) = HAP_STATUS_SUCCESS;
          }
          else {
            *(write->status) = HAP_STATUS_RES_ABSENT;
          }
        }
        return ret;
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
      DoorbellEntity(event::Event* eventPtr, lock::Lock* lockPtr = nullptr, light::LightState* lightPtr = nullptr): eventPtr(eventPtr), lockPtr(lockPtr), lightPtr(lightPtr) {}

      event::Event* getEventEntity() {
        return eventPtr;
      }

      lock::Lock* getLockEntity() {
        return lockPtr;
      }

      light::LightState* getLightEntity() {
        return lightPtr;
      }

      void register_on_identify_trigger(HKIdentifyTrigger* trig) {
          triggers_identify_.push_back(trig);
      }

      void setup() {
        ESP_LOGCONFIG(TAG, "Setting up doorbell event '%s'", eventPtr->get_name().c_str());

        if(lockPtr != nullptr) {
          ESP_LOGCONFIG(TAG, "Setting up doorbell lock '%s'", lockPtr->get_name().c_str());
        }

        if(lightPtr != nullptr) {
          ESP_LOGCONFIG(TAG, "Setting up doorbell light '%s'", lightPtr->get_name().c_str());
        }

        // Doorbell Service
        hap_serv_t* doorbellService = hap_serv_create(HAP_SERV_UUID_DOORBELL);

        if (doorbellService) {
          hap_acc_cfg_t acc_cfg = {
            .name = strdup_psram(eventPtr->get_name().c_str()),
            .model = (char*)"ESPHome Doorbell",
            .manufacturer = (char*)"ESPHome",
            .serial_num = strdup_psram(std::to_string(eventPtr->get_object_id_hash()).c_str()),
            .fw_rev = (char*)"1.0.0",
            .hw_rev = NULL,
            .pv = (char*)"1.1.0",
            .cid = HAP_CID_BRIDGE,
            .identify_routine = acc_identify,
          };

          hap_acc_t* accessory = hap_acc_create(&acc_cfg);
          accessory_ = accessory;
          acc_instance_map[accessory] = this;

          // Doorbell Service
          hap_serv_add_char(doorbellService, hap_char_programmable_switch_event_create(0));
          const uint8_t _validVals[] = {0,1,2};
          hap_char_add_valid_vals(hap_serv_get_char_by_uuid(doorbellService, HAP_CHAR_UUID_PROGRAMMABLE_SWITCH_EVENT), _validVals, 3);
          hap_serv_set_priv(doorbellService, eventPtr);
          hap_serv_mark_primary(doorbellService); // primary service
          hap_acc_add_serv(accessory, doorbellService);

          ESP_LOGD(TAG, "ID HASH: %lu", eventPtr->get_object_id_hash());

          // Lock Service
          if (lockPtr != nullptr) {
            hap_serv_t* lockService = hap_serv_lock_mechanism_create(lockPtr->state, lockPtr->state);
            hap_serv_add_char(lockService, hap_char_name_create(strdup_psram(lockPtr->get_name().c_str())));
            hap_serv_set_priv(lockService, lockPtr);
            hap_serv_set_write_cb(lockService, lock_write);
            hap_serv_link_serv(doorbellService, lockService); // Link the Lock Service to the Doorbell Service
            hap_acc_add_serv(accessory, lockService);
          }

          // Light Service
          if (lightPtr != nullptr) {
            hap_serv_t* lightService = hap_serv_lightbulb_create(false);
            hap_serv_add_char(lightService, hap_char_name_create(strdup_psram(lightPtr->get_name().c_str())));
            hap_serv_set_priv(lightService, lightPtr);
            hap_serv_set_write_cb(lightService, light_write);
            hap_serv_link_serv(doorbellService, lightService); // Link the Lock Service to the Doorbell Service
            hap_acc_add_serv(accessory, lightService);
          }

          /* Add the Accessory to the HomeKit Database */
          uint32_t combined_id = eventPtr->get_object_id_hash() ^ (lockPtr ? lockPtr->get_object_id_hash() : 0);
          hap_add_bridged_accessory(accessory, hap_get_unique_aid(std::to_string(combined_id).c_str()));

          if (!eventPtr->is_internal())
              eventPtr->add_on_event_callback([this](std::string event_type) { this->on_event_update(eventPtr, event_type); });

          if (lockPtr != nullptr && !lockPtr->is_internal())
              lockPtr->add_on_state_callback([this]() { this->on_lock_update(lockPtr); });

          if (lightPtr != nullptr && !lightPtr->is_internal())
              lightPtr->add_new_target_state_reached_callback([this]() { this->on_light_update(lightPtr); });

          ESP_LOGI(TAG, "Doorbell Event '%s' linked to HomeKit", eventPtr->get_name().c_str());
          
          if(lockPtr != nullptr)
            ESP_LOGI(TAG, "  Lock '%s' linked to Doorbell '%s'", lockPtr->get_name().c_str(), eventPtr->get_name().c_str());

          if(lightPtr != nullptr)
            ESP_LOGI(TAG, "  Light '%s' linked to Doorbell '%s'", lightPtr->get_name().c_str(), eventPtr->get_name().c_str());
          
        }
      }
    };

    inline std::unordered_map<hap_acc_t*, DoorbellEntity*> DoorbellEntity::acc_instance_map;
  }
}
#endif