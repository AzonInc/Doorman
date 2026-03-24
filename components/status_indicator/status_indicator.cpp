#include "status_indicator.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

#ifdef USE_ETHERNET
#include "esphome/components/ethernet/ethernet_component.h"
#endif
#ifdef USE_MODEM
#include "esphome/components/modem/modem_component.h"
#endif
#ifdef USE_WIFI
#include "esphome/components/wifi/wifi_component.h"
#endif
#ifdef USE_IMPROV
#include "esphome/components/esp32_improv/esp32_improv_component.h"
#endif
#ifdef USE_MQTT
#include "esphome/components/mqtt/mqtt_client.h"
#endif
#ifdef USE_API
#include "esphome/components/api/api_server.h"
#endif

namespace esphome::status_indicator
{

  static constexpr const char *S_APP_ERROR         = "on_app_error";
  static constexpr const char *S_CLEAR_APP_ERROR   = "on_clear_app_error";
  static constexpr const char *S_APP_WARNING       = "on_app_warning";
  static constexpr const char *S_CLEAR_APP_WARNING = "on_clear_app_warning";
  static constexpr const char *S_NET_CONNECTED     = "on_network_connected";
  static constexpr const char *S_NET_DISCONNECTED  = "on_network_disconnected";
  static constexpr const char *S_WIFI_AP_ENABLED   = "on_wifi_ap_enabled";
  static constexpr const char *S_WIFI_AP_DISABLED  = "on_wifi_ap_disabled";
  static constexpr const char *S_IMPROV_ENABLED    = "on_improv_enabled";
  static constexpr const char *S_IMPROV_DISABLED   = "on_improv_disabled";
  static constexpr const char *S_API_CONNECTED     = "on_api_connected";
  static constexpr const char *S_API_DISCONNECTED  = "on_api_disconnected";
  static constexpr const char *S_MQTT_CONNECTED    = "on_mqtt_connected";
  static constexpr const char *S_MQTT_DISCONNECTED = "on_mqtt_disconnected";
  static constexpr const char *S_CUSTOM            = "on_custom_status";
  static constexpr const char *S_TURN_OFF          = "on_turn_off";
  static constexpr const char *S_EMPTY             = "";
  // Sentinel used by push/pop to force a loop() re-evaluation
  static constexpr const char *S_UPDATE_ME         = "update me";

  // ── Network helpers ────────────────────────────────────────────────────────────
  static bool has_network()
  {
    #ifdef USE_ETHERNET
    if (ethernet::global_eth_component != nullptr) return true;
    #endif
    #ifdef USE_MODEM
    if (modem::global_modem_component != nullptr) return true;
    #endif
    #ifdef USE_WIFI
    if (wifi::global_wifi_component != nullptr) return true;
    #endif
    #ifdef USE_HOST
    return true;
    #endif
    return false;
  }

  ESPHOME_ALWAYS_INLINE static bool is_connected()
  {
    #ifdef USE_ETHERNET
    if (ethernet::global_eth_component != nullptr && ethernet::global_eth_component->is_connected()) return true;
    #endif
    #ifdef USE_MODEM
    if (modem::global_modem_component != nullptr)
      return modem::global_modem_component->is_connected();
    #endif
    #ifdef USE_WIFI
    if (wifi::global_wifi_component != nullptr)
      return wifi::global_wifi_component->is_connected();
    #endif
    #ifdef USE_HOST
    return true;
    #endif
    return false;
  }

  static const char *const TAG = "status_indicator";

  void StatusIndicator::dump_config()
  {
    ESP_LOGCONFIG(TAG, "Status Indicator supports:");
    for (auto &kv : this->triggers_)
    {
      ESP_LOGCONFIG(TAG, " * %s: %s", kv.first, kv.second->get_info().c_str());
    }
  }

  void StatusIndicator::loop()
  {
    const char *status = S_EMPTY;

    if ((App.get_app_state() & STATUS_LED_ERROR) != 0u)
    {
      if (has_trigger(S_APP_ERROR))
      {
        status = S_APP_ERROR;
      }
      this->status_.on_error = 1;
    }
    else if (this->status_.on_error)
    {
      if (has_trigger(S_CLEAR_APP_ERROR))
      {
        status = S_CLEAR_APP_ERROR;
      }
      this->status_.on_error = 0;
    }

    if (has_network())
    {
      #ifdef USE_WIFI
      if (status[0] == '\0' && wifi::global_wifi_component->is_ap_active())
      {
        if (has_trigger(S_WIFI_AP_ENABLED))
        {
          status = S_WIFI_AP_ENABLED;
        }
        this->status_.on_wifi_ap = 1;
      }
      else if (this->status_.on_wifi_ap)
      {
        if (has_trigger(S_WIFI_AP_DISABLED))
        {
          status = S_WIFI_AP_DISABLED;
        }
        this->status_.on_wifi_ap = 0;
      }

      #ifdef USE_IMPROV
      if (status[0] == '\0' && esp32_improv::global_improv_component != nullptr && esp32_improv::global_improv_component->is_active())
      {
        if (has_trigger(S_IMPROV_ENABLED))
        {
          status = S_IMPROV_ENABLED;
        }
        this->status_.on_improv = 1;
      }
      else if (this->status_.on_improv)
      {
        if (has_trigger(S_IMPROV_DISABLED))
        {
          status = S_IMPROV_DISABLED;
        }
        this->status_.on_improv = 0;
      }
      #endif
      #endif

      if (status[0] == '\0' && !is_connected())
      {
        if (has_trigger(S_NET_DISCONNECTED))
        {
          status = S_NET_DISCONNECTED;
        }
        this->status_.on_network = 1;
      }
      else if (this->status_.on_network)
      {
        if (has_trigger(S_NET_CONNECTED))
        {
          status = S_NET_CONNECTED;
        }
        this->status_.on_network = 0;
      }

      #ifdef USE_API
      if (status[0] == '\0' && api::global_api_server != nullptr && !api::global_api_server->is_connected_with_state_subscription())
      {
        if (has_trigger(S_API_DISCONNECTED))
          status = S_API_DISCONNECTED;
        this->status_.on_api = 1;
      }
      else if (this->status_.on_api)
      {
        if (has_trigger(S_API_CONNECTED))
        {
          status = S_API_CONNECTED;
        }
        this->status_.on_api = 0;
      }
      #endif

      #ifdef USE_MQTT
      if (status[0] == '\0' && mqtt::global_mqtt_client != nullptr && !mqtt::global_mqtt_client->is_connected())
      {
        if (has_trigger(S_MQTT_DISCONNECTED))
        {
          status = S_MQTT_DISCONNECTED;
        }
        this->status_.on_mqtt = 1;
      }
      else if (this->status_.on_mqtt)
      {
        if (has_trigger(S_MQTT_CONNECTED))
        {
          status = S_MQTT_CONNECTED;
        }
        this->status_.on_mqtt = 0;
      }
      #endif
    }

    if (status[0] == '\0' && (App.get_app_state() & STATUS_LED_WARNING) != 0u)
    {
      if (has_trigger(S_APP_WARNING))
      {
        status = S_APP_WARNING;
      }
      this->status_.on_warning = 1;
    }
    else if (this->status_.on_warning)
    {
      if (has_trigger(S_CLEAR_APP_WARNING))
      {
        status = S_CLEAR_APP_WARNING;
      }
      this->status_.on_warning = 0;
    }

    if (this->current_status_ != status)
    {
      if (this->current_trigger_ != nullptr && this->current_trigger_->is_action_running() && status[0] != '\0')
      {
        this->current_trigger_->stop_action();
      }

      StatusTrigger *old_trigger = this->current_trigger_;

      if (status[0] != '\0' && this->triggers_.count(status) != 0)
      {
        // Known priority status
        this->current_trigger_ = get_trigger(status);
      }
      else if (!this->stack_.empty())
      {
        // Custom stack status
        this->current_trigger_ = this->stack_.back();
        status = S_CUSTOM;
      }
      else
      {
        // Idle
        this->current_trigger_ = get_trigger(S_TURN_OFF);
        status = S_TURN_OFF;
      }

      if (old_trigger != this->current_trigger_)
      {
        ESP_LOGV(TAG, "<>> %s->%.*s",
                status,
                (int) this->current_trigger_->get_name_view().size(),
                this->current_trigger_->get_name_view().data());
        this->current_trigger_->trigger();
      }

      this->current_status_ = status;
    }
  }

  float StatusIndicator::get_setup_priority() const { return setup_priority::HARDWARE; }
  #ifdef USE_LOOP_PRIORITY
  float StatusIndicator::get_loop_priority() const { return 50.0f; }
  #endif

  StatusTrigger *StatusIndicator::get_trigger(const char *key)
  {
    auto it = this->triggers_.find(key);
    return (it != this->triggers_.end()) ? it->second : nullptr;
  }

  bool StatusIndicator::has_trigger(const char *key)
  {
    return this->triggers_.count(key) != 0;
  }

  void StatusIndicator::set_trigger(const char *key, StatusTrigger *trigger)
  {
    this->triggers_[key] = trigger;
  }

  void StatusIndicator::push_trigger(StatusTrigger *trigger)
  {
    this->pop_trigger(trigger, true);
    ESP_LOGV(TAG, "Push ID: %s", trigger->get_info().c_str());

    for (auto i = this->stack_.begin(); i != this->stack_.end(); ++i)
    {
      if (trigger->get_priority() < (*i)->get_priority())
      {
        this->stack_.insert(i, trigger);
        this->current_status_ = S_UPDATE_ME;
        return;
      }
    }
    this->stack_.push_back(trigger);
    this->current_status_ = S_UPDATE_ME;
  }

  void StatusIndicator::pop_trigger(StatusTrigger *trigger, bool incl_group)
  {
    std::string_view group = trigger->get_group_view();
    incl_group = incl_group && !group.empty();
    ESP_LOGV(TAG, "Pop by ID: %s || %s", trigger->get_info().c_str(), YESNO(incl_group));

    for (auto i = this->stack_.begin(); i != this->stack_.end();)
    {
      StatusTrigger *st = *i;
      if ((incl_group && group == st->get_group_view()) || (trigger == st))
      {
        i = this->stack_.erase(i);
        this->current_status_ = S_UPDATE_ME;
      }
      else
      {
        ++i;
      }
    }
  }

  void StatusIndicator::pop_trigger(std::string_view group)
  {
    ESP_LOGV(TAG, "Pop by group: %.*s", (int) group.size(), group.data());

    for (auto i = this->stack_.begin(); i != this->stack_.end();)
    {
      if (group == (*i)->get_group_view())
      {
        i = this->stack_.erase(i);
        this->current_status_ = S_UPDATE_ME;
      }
      else
      {
        ++i;
      }
    }
  }

}  // namespace esphome::status_indicator