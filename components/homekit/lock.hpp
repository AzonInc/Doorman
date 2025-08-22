#pragma once
#include <esphome/core/defines.h>
#ifdef USE_LOCK
#include <map>
#include <esphome/core/application.h>
#include <hap.h>
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>
#include "esphome/components/homekit_bridge/const.h"
#include "esphome/components/homekit_bridge/util.h"
#include "automation.h"

namespace esphome {
    namespace homekit {

        class LockEntity
        {
            static std::unordered_map<hap_acc_t*, LockEntity*> acc_instance_map;
            
        private:
            static constexpr const char* TAG = "homekit.lock";
            lock::Lock* entityPtr;

            void on_entity_update(lock::Lock* obj) {
                ESP_LOGD("on_entity_update", "%s state: %s", obj->get_name().c_str(), lock_state_to_string(obj->state));
                hap_acc_t* acc = hap_acc_get_by_aid(hap_get_unique_aid(std::to_string(obj->get_object_id_hash()).c_str()));
                hap_serv_t* hs = hap_acc_get_serv_by_uuid(acc, HAP_SERV_UUID_LOCK_MECHANISM);
                hap_char_t* current_state = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_LOCK_CURRENT_STATE);
                hap_char_t* target_state = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_LOCK_TARGET_STATE);
                hap_val_t c;
                hap_val_t t;
                
                if (obj->state == lock::LockState::LOCK_STATE_LOCKED || obj->state == lock::LockState::LOCK_STATE_UNLOCKED) {
                    c.i = obj->state % 2;
                    t.i = obj->state % 2;
                    hap_char_update_val(current_state, &c);
                    hap_char_update_val(target_state, &t);
                }
                else if (obj->state == lock::LockState::LOCK_STATE_LOCKING || obj->state == lock::LockState::LOCK_STATE_UNLOCKING) {
                    t.i = (obj->state % 5) % 3;
                    hap_char_update_val(target_state, &t);
                }
                else if (obj->state == lock::LockState::LOCK_STATE_JAMMED) {
                    c.i = obj->state;
                    hap_char_update_val(current_state, &c);
                }
            }

            static int lock_write(hap_write_data_t write_data[], int count, void* serv_priv, void* write_priv) {
                lock::Lock* entityPtr = (lock::Lock*)serv_priv;
                ESP_LOGD("lock_write", "Write called for Accessory '%s'(%s)", entityPtr->get_name().c_str(), 
                        std::to_string(entityPtr->get_object_id_hash()).c_str());
                
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
                        write->val.i ? entityPtr->lock() : entityPtr->unlock();
                        *(write->status) = HAP_STATUS_SUCCESS;
                    } else {
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
            LockEntity(lock::Lock* entityPtr) : entityPtr(entityPtr) {}

            lock::Lock* getEntity() {
                return entityPtr;
            }

            void register_on_identify_trigger(HKIdentifyTrigger* trig) {
                triggers_identify_.push_back(trig);
            }

            void setup() {
                ESP_LOGCONFIG(TAG, "Setting up lock '%s'", entityPtr->get_name().c_str());

                hap_serv_t* service = hap_serv_lock_mechanism_create(entityPtr->state, entityPtr->state);

                if (service) {
                    hap_acc_cfg_t acc_cfg = {
                        .name = strdup_psram(entityPtr->get_name().c_str()),
                        .model = (char*)"ESPHome Lock",
                        .manufacturer = (char*)"ESPHome",
                        .serial_num = strdup_psram(std::to_string(entityPtr->get_object_id_hash()).c_str()),
                        .fw_rev = (char*)"1.0.0",
                        .hw_rev = NULL,
                        .hw_finish = NULL,
                        .pv = (char*)"1.1.0",
                        .cid = HAP_CID_BRIDGE,
                        .identify_routine = acc_identify,
                    };

                    hap_acc_t* accessory = hap_acc_create(&acc_cfg);
                    acc_instance_map[accessory] = this;

                    ESP_LOGD("homekit", "ID HASH: %lu", entityPtr->get_object_id_hash());
                    hap_serv_set_priv(service, entityPtr);
                    hap_serv_set_write_cb(service, lock_write);
                    hap_acc_add_serv(accessory, service);

                    hap_add_bridged_accessory(accessory, 
                                            hap_get_unique_aid(std::to_string(entityPtr->get_object_id_hash()).c_str()));
                    
                    if (!entityPtr->is_internal())
                        entityPtr->add_on_state_callback([this]() { this->on_entity_update(entityPtr); });

                    ESP_LOGI("homekit", "Lock '%s' linked to HomeKit", entityPtr->get_name().c_str());
                }
            }
        };

        inline std::unordered_map<hap_acc_t*, LockEntity*> LockEntity::acc_instance_map;

    } // namespace homekit
} // namespace esphome
#endif