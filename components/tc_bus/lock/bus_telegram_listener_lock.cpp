#include "bus_telegram_listener_lock.h"

namespace esphome
{
    namespace tc_bus
    {
        // lock & unlock called by telegram receiver function
        void BusTelegramListenerLock::unlock(uint32_t *timer, uint16_t auto_lock)
        {
            this->publish_state(lock::LOCK_STATE_UNLOCKED);
            if (auto_lock > 0) *timer = millis() + auto_lock;
        }

        void BusTelegramListenerLock::lock(uint32_t *timer)
        {
            this->publish_state(lock::LOCK_STATE_LOCKED);
            *timer = 0;

            this->lock_callback_.call();
        }

        void BusTelegramListenerLock::control(const esphome::lock::LockCall &call)
        {
            if (call.get_state() == esphome::lock::LOCK_STATE_UNLOCKED)
            {
                this->before_unlock_callback_.call();

                uint8_t listener_address = 0;
                if(this->address_.has_value())
                {
                    listener_address = this->address_.value();
                }
                else if (this->address_lambda_.has_value())
                {
                    auto optional_value = (*this->address_lambda_)();
                    if (optional_value.has_value())
                    {
                        listener_address = optional_value.value();
                    }
                }

                this->parent_->send_telegram(TELEGRAM_TYPE_OPEN_DOOR, listener_address);

                this->after_unlock_callback_.call();
            }
        }
    }  // namespace tc_bus
}  // namespace esphome