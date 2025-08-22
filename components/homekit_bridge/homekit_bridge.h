#pragma once
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <hap.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <map>
#include "const.h"
#include "util.h"

#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif

namespace esphome
{
namespace homekit {

  class HomeKitBridgeComponent : public Component
  {
  #ifdef USE_BUTTON
    SUB_BUTTON(reset)
  #endif

  private:
    static constexpr const char* TAG = "homekit_bridge";

  public:
    float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
    void setup() override;
    void loop() override;
    void dump_config() override;

    void set_task_stack_size(uint32_t task_stack_size){ this->task_stack_size_ = task_stack_size; }
    void set_setup_code(const char *setup_code) { this->setup_code_ = setup_code; }
    void set_setup_id(const char *setup_id) { this->setup_id_ = setup_id; }
    void set_name(const char *name) { this->name_ = name; }
    void set_model(const char *model) { this->model_ = model; }
    void set_manufacturer(const char *manufacturer) { this->manufacturer_ = manufacturer; }
    void set_task_stack_size(const char *setup_id) { this->setup_id_ = setup_id; }

    void report_heap();
    void factory_reset();

    bool is_paired();
    bool is_connected();

    static void static_hap_event_handler(hap_event_t event, void *data);
    static int static_identify_handler(hap_acc_t *ha);

    void add_pairing_started_callback(std::function<void()> &&callback)
    {
        this->pairing_started_callback_.add(std::move(callback));
    }

    void add_pairing_aborted_callback(std::function<void()> &&callback)
    {
        this->pairing_aborted_callback_.add(std::move(callback));
    }

    void add_pairing_timeout_callback(std::function<void()> &&callback)
    {
        this->pairing_timeout_callback_.add(std::move(callback));
    }

    void add_pairing_completed_callback(std::function<void(std::string)> &&callback)
    {
        this->pairing_completed_callback_.add(std::move(callback));
    }

    void add_identify_callback(std::function<void()> &&callback)
    {
        this->identify_callback_.add(std::move(callback));
    }

    void add_controller_connected_callback(std::function<void(std::string)> &&callback)
    {
        this->controller_connected_callback_.add(std::move(callback));
    }

    void add_controller_disconnected_callback(std::function<void(std::string)> &&callback)
    {
        this->controller_disconnected_callback_.add(std::move(callback));
    }

  protected:
    bool is_ready_ = false;
    uint8_t connection_count_ = 0;
    uint32_t task_stack_size_ = 4096;

    const char *setup_code_ = "159-35-728";
    const char *setup_id_ = "DMS3";
    const char *name_ = "HomeKit Bridge";
    const char *model_ = "ESP32 HAP";
    const char *manufacturer_ = "ESPHome";

    CallbackManager<void()> pairing_started_callback_{};
    CallbackManager<void()> pairing_aborted_callback_{};
    CallbackManager<void()> pairing_timeout_callback_{};
    CallbackManager<void(std::string)> pairing_completed_callback_{};
    CallbackManager<void()> identify_callback_{};
    CallbackManager<void(std::string)> controller_connected_callback_{};
    CallbackManager<void(std::string)> controller_disconnected_callback_{};
  };

  static HomeKitBridgeComponent *global_homekit_bridge;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace homekit
}  // namespace esphome