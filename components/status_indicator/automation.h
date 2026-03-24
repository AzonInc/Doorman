#pragma once

#include "status_indicator.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome::status_indicator
{
  class StatusTrigger;

  template<typename... Ts> class StatusCondition : public Condition<Ts...>
  {
  public:
    StatusCondition(StatusIndicator *parent, bool state) : parent_(parent), state_(state) {}
    bool check(Ts... x) override { return (this->parent_->status_.setter == 0) == this->state_; }

  protected:
    StatusIndicator *parent_;
    bool state_;
  };

  template<typename... Ts> class StatusAction : public Action<Ts...>, public Parented<StatusIndicator>
  {
  public:
    void set_state(bool state) { this->state_ = state; }
    void set_trigger(StatusTrigger *trigger) { this->trigger_ = trigger; }
    void set_group(std::string_view group) { this->group_ = group; }

    void play(Ts... x) override
    {
      if (this->state_)
      {
        if (this->trigger_ != nullptr)
        {
            this->parent_->push_trigger(this->trigger_);
        }
      }
      else if (!this->group_.empty())
      {
        this->parent_->pop_trigger(std::string_view(this->group_));
      }
      else if (this->trigger_ != nullptr)
      {
        this->parent_->pop_trigger(this->trigger_, false);
      }
    }

  protected:
    StatusTrigger *trigger_{nullptr};
    std::string group_{};
    bool state_{false};
  };
}