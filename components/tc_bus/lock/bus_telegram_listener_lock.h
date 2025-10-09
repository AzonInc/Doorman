#pragma once

#include "../tc_bus.h"
#include "esphome/components/lock/lock.h"

namespace esphome
{
    namespace tc_bus
    {
        class BusTelegramListenerLock final : public lock::Lock, public TCBusLockListener, public Parented<TCBusComponent>
        {
            public:
                void unlock(uint32_t *timer, uint16_t auto_lock) override;
                void lock(uint32_t *timer) override;

                void control(const esphome::lock::LockCall &call) override;

                // Automation Callbacks
                void add_before_unlock_callback(std::function<void()> &&callback)
                {
                    this->before_unlock_callback_.add(std::move(callback));
                }

                void add_after_unlock_callback(std::function<void()> &&callback)
                {
                    this->after_unlock_callback_.add(std::move(callback));
                }

                void add_lock_callback(std::function<void()> &&callback)
                {
                    this->lock_callback_.add(std::move(callback));
                }

            protected:
                // Automation Callbacks
                CallbackManager<void()> before_unlock_callback_{};
                CallbackManager<void()> after_unlock_callback_{};
                CallbackManager<void()> lock_callback_{};
        };

    }  // namespace tc_bus
}  // namespace esphome