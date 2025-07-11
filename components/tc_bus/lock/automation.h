#pragma once

#include "./bus_telegram_listener_lock.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome
{
    namespace tc_bus
    {
        class BeforeUnlockTrigger : public Trigger<> {
            public:
                explicit BeforeUnlockTrigger(BusTelegramListenerLock *parent) {
                    parent->add_before_unlock_callback([this]() { this->trigger(); });
                }
        };

        class AfterUnlockTrigger : public Trigger<> {
            public:
                explicit AfterUnlockTrigger(BusTelegramListenerLock *parent) {
                    parent->add_after_unlock_callback([this]() { this->trigger(); });
                }
        };

        class LockTrigger : public Trigger<> {
            public:
                explicit LockTrigger(BusTelegramListenerLock *parent) {
                    parent->add_lock_callback([this]() { this->trigger(); });
                }
        };
    }  // namespace tc_bus
}  // namespace esphome