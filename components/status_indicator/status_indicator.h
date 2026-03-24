#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/light/automation.h"

namespace esphome::status_indicator
{
  class StatusTrigger;

  struct CStrHash {
    using is_transparent = void;
    size_t operator()(const char       *s)  const noexcept { return std::hash<std::string_view>{}(s); }
    size_t operator()(std::string_view  sv) const noexcept { return std::hash<std::string_view>{}(sv); }
    size_t operator()(const std::string &s) const noexcept { return std::hash<std::string_view>{}(s); }
  };
  struct CStrEqual {
    using is_transparent = void;
    bool operator()(const char *a, const char *b)           const noexcept { return std::string_view(a) == b; }
    bool operator()(std::string_view a,  const char *b)     const noexcept { return a == b; }
    bool operator()(const char *a, std::string_view b)      const noexcept { return std::string_view(a) == b; }
    bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
  };

  union StatusFlags
  {
    struct {
      uint8_t on_error    : 1;
      uint8_t on_warning  : 1;
      uint8_t on_network  : 1;
      uint8_t on_api      : 1;
      uint8_t on_mqtt     : 1;
      uint8_t on_wifi_ap  : 1;
      uint8_t on_improv   : 1;
    };
    uint8_t setter = 0;  // uint8 suffices for 6 bits; saves 1 byte vs uint16
  };

  class StatusIndicator : public Component
  {
  public:
    void dump_config() override;
    void loop() override;

    float get_setup_priority() const override;
    #ifdef USE_LOOP_PRIORITY
    float get_loop_priority() const override;
    #endif

    // All key lookups accept const char* pointing to stable string literals.
    // No heap allocation on get/has/set for known compile-time keys.
    StatusTrigger *get_trigger(const char *key);
    bool           has_trigger(const char *key);
    void           set_trigger(const char *key, StatusTrigger *trigger);

    void push_trigger(StatusTrigger *trigger);
    void pop_trigger(StatusTrigger *trigger, bool incl_group = false);
    void pop_trigger(std::string_view group);  // replaces pop_trigger(const std::string&)

  protected:
    StatusTrigger *current_trigger_{nullptr};
    StatusFlags    status_{};

    // Points to a compile-time string literal → zero heap, zero copy.
    const char    *current_status_{""};

    // unordered_map: O(1) lookups vs O(log n) for std::map.
    // CStrHash/CStrEqual allow lookup by string_view with no key allocation.
    std::unordered_map<const char *, StatusTrigger *, CStrHash, CStrEqual> triggers_{};

    std::vector<StatusTrigger *> stack_{};
  };

  class StatusTrigger : public Trigger<>
  {
  public:
    explicit StatusTrigger(StatusIndicator *parent,
                           std::string group,
                           uint32_t priority,
                           std::string name)
        : parent_(parent),
          name_(std::move(name)),
          group_(std::move(group)),
          priority_(priority) {}

    // Zero-copy view getters for internal hot-path use
    std::string_view get_name_view()  const { return this->name_; }
    std::string_view get_group_view() const { return this->group_; }
    uint32_t         get_priority()   const { return this->priority_; }

    // std::string getters kept for ESPHome framework compatibility
    std::string get_name()  { return this->name_; }
    std::string get_group() { return this->group_; }
    std::string get_info()
    {
      return this->name_ + " \t| " + this->group_ + " \t| " + std::to_string(this->priority_);
    }

  protected:
    StatusIndicator *parent_;
    std::string      name_;
    std::string      group_;
    uint32_t         priority_;
  };

}  // namespace esphome::status_indicator