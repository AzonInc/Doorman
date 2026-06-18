#pragma once

#include "../tc_bus.h"
#include "esphome/components/lock/lock.h"

namespace esphome::tc_bus
{
    class BusTelegramListenerLock final : public lock::Lock, public TCBusLockListener, public Parented<TCBusComponent>
    {
        public:
            void setup() override;
            
            void unlock(uint32_t *timer, uint16_t auto_reset) override;
            void lock(uint32_t *timer) override;
            void open() override;

            void control(const esphome::lock::LockCall &call) override;

            // Automation Callbacks
            template<typename F> void add_before_unlock_callback(F &&callback)
            {
                this->before_unlock_callback_.add(std::forward<F>(callback));
            }

            template<typename F> void add_after_unlock_callback(F &&callback)
            {
                this->after_unlock_callback_.add(std::forward<F>(callback));
            }

            template<typename F> void add_lock_callback(F &&callback)
            {
                this->lock_callback_.add(std::forward<F>(callback));
            }

        protected:
            // Automation Callbacks
            LazyCallbackManager<void()> before_unlock_callback_{};
            LazyCallbackManager<void()> after_unlock_callback_{};
            LazyCallbackManager<void()> lock_callback_{};
    };
}