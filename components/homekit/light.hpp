#include <esphome/core/defines.h>
#ifdef USE_LIGHT
#pragma once
#include <esphome/core/application.h>
#include <hap.h>
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>
#include <map>
#include "esphome/components/homekit_bridge/const.h"
#include "esphome/components/homekit_bridge/util.h"
#include "automation.h"

namespace esphome
{
  namespace homekit
  {
    class LightEntity
    {
      static std::unordered_map<hap_acc_t*, LightEntity*> acc_instance_map;

    private:
      static constexpr const char* TAG = "homekit.light";
      light::LightState* entityPtr;

      static int light_write(hap_write_data_t write_data[], int count, void* serv_priv, void* write_priv) {
        light::LightState* entityPtr = (light::LightState*)serv_priv;
        ESP_LOGD(TAG, "Write called for Accessory %s (%s)", std::to_string(entityPtr->get_object_id_hash()).c_str(), entityPtr->get_name().c_str());
        int i, ret = HAP_SUCCESS;
        hap_write_data_t* write;
        for (i = 0; i < count; i++) {
          write = &write_data[i];
          if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_ON)) {
            ESP_LOGD(TAG, "Received Write for Light '%s' state: %s", entityPtr->get_name().c_str(), write->val.b ? "On" : "Off");
            ESP_LOGD(TAG, "[STATE] CURRENT STATE: %d", (int)(entityPtr->current_values.get_state() * 100));
            write->val.b ? entityPtr->turn_on().set_save(true).perform() : entityPtr->turn_off().set_save(true).perform();
            hap_char_update_val(write->hc, &(write->val));
            *(write->status) = HAP_STATUS_SUCCESS;
          }
          if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_BRIGHTNESS)) {
            ESP_LOGD(TAG, "Received Write for Light '%s' Level: %d", entityPtr->get_name().c_str(), write->val.i);
            ESP_LOGD(TAG, "[LEVEL] CURRENT BRIGHTNESS: %d", (int)(entityPtr->current_values.get_brightness() * 100));
            ESP_LOGD(TAG, "TARGET BRIGHTNESS: %d", (int)write->val.i);
            entityPtr->make_call().set_save(true).set_brightness((float)(write->val.i) / 100).perform();
            hap_char_update_val(write->hc, &(write->val));
            *(write->status) = HAP_STATUS_SUCCESS;
          }
          if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_HUE)) {
            ESP_LOGD(TAG, "Received Write for Light '%s' Hue: %.2f", entityPtr->get_name().c_str(), write->val.f);
            int hue = 0;
            float saturation = 0;
            float colorValue = 0;
            rgb_to_hsv(entityPtr->remote_values.get_red(), entityPtr->remote_values.get_green(), entityPtr->remote_values.get_blue(), hue, saturation, colorValue);
            ESP_LOGD(TAG, "[HUE] CURRENT Hue: %d, Saturation: %.2f, Value: %.2f", hue, saturation, colorValue);
            ESP_LOGD(TAG, "TARGET HUE: %.2f", write->val.f);
            float tR = 0;
            float tG = 0;
            float tB = 0;
            hsv_to_rgb(write->val.f, saturation, colorValue, tR, tG, tB);
            ESP_LOGD(TAG, "TARGET RGB: %.2f %.2f %.2f", tR, tG, tB);
            entityPtr->make_call().set_rgb(tR, tG, tB).set_save(true).perform();
            hap_char_update_val(write->hc, &(write->val));
            *(write->status) = HAP_STATUS_SUCCESS;
          }
          if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_SATURATION)) {
            ESP_LOGD(TAG, "Received Write for Light '%s' Saturation: %.2f", entityPtr->get_name().c_str(), write->val.f);
            int hue = 0;
            float saturation = 0;
            float colorValue = 0;
            rgb_to_hsv(entityPtr->remote_values.get_red(), entityPtr->remote_values.get_green(), entityPtr->remote_values.get_blue(), hue, saturation, colorValue);
            ESP_LOGD(TAG, "[SATURATION] CURRENT Hue: %d, Saturation: %.2f, Value: %.2f", hue, saturation, colorValue);
            ESP_LOGD(TAG, "TARGET SATURATION: %.2f", write->val.f);
            float tR = 0;
            float tG = 0;
            float tB = 0;
            hsv_to_rgb(hue, write->val.f / 100, colorValue, tR, tG, tB);
            ESP_LOGD(TAG, "TARGET RGB: %.2f %.2f %.2f", tR, tG, tB);
            entityPtr->make_call().set_rgb(tR, tG, tB).set_save(true).perform();
            hap_char_update_val(write->hc, &(write->val));
            *(write->status) = HAP_STATUS_SUCCESS;
          }
          if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_COLOR_TEMPERATURE)) {
            ESP_LOGD(TAG, "Received Write for Light '%s' Level: %d", entityPtr->get_name().c_str(), write->val.i);
            ESP_LOGD(TAG, "[LEVEL] CURRENT COLOR TEMPERATURE(mired): %.2f", entityPtr->current_values.get_color_temperature());
            ESP_LOGD(TAG, "TARGET COLOR TEMPERATURE(mired): %lu", write->val.u);
            entityPtr->make_call().set_color_temperature(write->val.u).set_save(true).perform();
            hap_char_update_val(write->hc, &(write->val));
            *(write->status) = HAP_STATUS_SUCCESS;
          }
          else {
            *(write->status) = HAP_STATUS_RES_ABSENT;
          }
        }
        return ret;
      }

      void on_entity_update(light::LightState* obj) {
        bool rgb = obj->current_values.get_color_mode() & light::ColorCapability::RGB;
        bool level = obj->get_traits().supports_color_capability(light::ColorCapability::BRIGHTNESS);
        bool temperature = obj->current_values.get_color_mode() & (light::ColorCapability::COLOR_TEMPERATURE | light::ColorCapability::COLD_WARM_WHITE);
        if (rgb) {
          ESP_LOGD(TAG, "%s RED: %.2f, GREEN: %.2f, BLUE: %.2f", obj->get_name().c_str(), obj->current_values.get_red(), obj->current_values.get_green(), obj->current_values.get_blue());
        }
        ESP_LOGD(TAG, "%s state: %d brightness: %d", obj->get_name().c_str(), (int)(obj->current_values.get_state() * 100), (int)(obj->current_values.get_brightness() * 100));
        hap_acc_t* acc = hap_acc_get_by_aid(hap_get_unique_aid(std::to_string(obj->get_object_id_hash()).c_str()));
        if (acc) {
          hap_serv_t* hs = hap_acc_get_serv_by_uuid(acc, HAP_SERV_UUID_LIGHTBULB);
          hap_char_t* on_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_ON);
          hap_val_t state;
          state.b = obj->current_values.get_state();
          hap_char_update_val(on_char, &state);
          if (level) {
            hap_char_t* level_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_BRIGHTNESS);
            hap_val_t level;
            level.i = (int)(obj->current_values.get_brightness() * 100);
            hap_char_update_val(level_char, &level);
          }
          if (rgb) {
            hap_char_t* hue_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_HUE);
            hap_char_t* saturation_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_SATURATION);
            hap_val_t hue;
            hap_val_t saturation;
            int cHue = 0;
            float cSaturation = 0;
            float colorValue = 0;
            rgb_to_hsv(obj->current_values.get_red(), obj->current_values.get_green(), obj->current_values.get_blue(), cHue, cSaturation, colorValue);
            hue.f = cHue;
            saturation.f = cSaturation * 100;
            hap_char_update_val(hue_char, &hue);
            hap_char_update_val(saturation_char, &saturation);
          }
          if (temperature) {
            hap_char_t* temp_char = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_COLOR_TEMPERATURE);
            hap_val_t temp;
            temp.u = obj->current_values.get_color_temperature();
            hap_char_update_val(temp_char, &temp);
          }
        }
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
      LightEntity(light::LightState* entityPtr) : entityPtr(entityPtr) {}

      light::LightState* getEntity() {
        return entityPtr;
      }

      void register_on_identify_trigger(HKIdentifyTrigger* trig) {
          triggers_identify_.push_back(trig);
      }

      void setup() {
        ESP_LOGCONFIG(TAG, "Setting up light '%s'", entityPtr->get_name().c_str());

        hap_acc_cfg_t acc_cfg = {
          .name = strdup_psram(entityPtr->get_name().c_str()),
          .model = (char*)"ESPHome Light",
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
        
        int hue = 0;
        float saturation = 0;
        float colorValue = 0;

        rgb_to_hsv(entityPtr->current_values.get_red(), entityPtr->current_values.get_green(), entityPtr->current_values.get_blue(), hue, saturation, colorValue);
        
        hap_serv_t* service = hap_serv_lightbulb_create(entityPtr->current_values.get_state());

        if (entityPtr->get_traits().supports_color_capability(light::ColorCapability::BRIGHTNESS)) {
          hap_serv_add_char(service, hap_char_brightness_create(entityPtr->current_values.get_brightness() * 100));
        }

        if (entityPtr->get_traits().supports_color_capability(light::ColorCapability::RGB)) {
          hap_serv_add_char(service, hap_char_hue_create(hue));
          hap_serv_add_char(service, hap_char_saturation_create(saturation * 100));
        }

        if (entityPtr->get_traits().supports_color_capability(light::ColorCapability::COLOR_TEMPERATURE) ||
            entityPtr->get_traits().supports_color_capability(light::ColorCapability::COLD_WARM_WHITE)) {
          hap_serv_add_char(service, hap_char_color_temperature_create(entityPtr->current_values.get_color_temperature()));
        }

        ESP_LOGD(TAG, "ID HASH: %lu", entityPtr->get_object_id_hash());
        hap_serv_set_priv(service, entityPtr);

        /* Set the write callback for the service */
        hap_serv_set_write_cb(service, light_write);

        /* Add the Light Service to the Accessory Object */
        hap_acc_add_serv(accessory, service);

        /* Add the Accessory to the HomeKit Database */
        hap_add_bridged_accessory(accessory, hap_get_unique_aid(std::to_string(entityPtr->get_object_id_hash()).c_str()));

        if (!entityPtr->is_internal())
          entityPtr->add_new_target_state_reached_callback([this]() { this->on_entity_update(entityPtr); });

        ESP_LOGI(TAG, "Light '%s' linked to HomeKit", entityPtr->get_name().c_str());
      }
    };

    inline std::unordered_map<hap_acc_t*, LightEntity*> LightEntity::acc_instance_map;
  } // namespace homekit
} // namespace esphome
#endif