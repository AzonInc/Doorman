#pragma once

#include "esphome/core/automation.h"
#include "tc_bus.h"

namespace esphome::tc_bus
{
    // Actions
    template<typename... Ts>
    class TCBusSendAction : public Action<Ts...>, public Parented<TCBusComponent> {
        TEMPLATABLE_VALUE(uint32_t, telegram)
        TEMPLATABLE_VALUE(bool, is_long)
        TEMPLATABLE_VALUE(TelegramType, type)
        TEMPLATABLE_VALUE(uint8_t, address)
        TEMPLATABLE_VALUE(uint32_t, payload)
        TEMPLATABLE_VALUE(uint32_t, serial_number)

        public:
            void play(const Ts &...x) override {
                if(this->telegram_.value(x...) == 0)
                {
                    this->parent_->send_telegram(this->type_.value(x...), this->address_.value(x...), this->payload_.value(x...), this->serial_number_.value(x...));
                }
                else
                {
                    if(!this->is_long_.value(x...))
                    {
                        this->parent_->send_telegram(this->telegram_.value(x...));
                    }
                    else
                    {
                        this->parent_->send_telegram(this->telegram_.value(x...), true);
                    }
                }
            }
    };

    template<typename... Ts>
    class TCBusProgrammingModeAction : public Action<Ts...>, public Parented<TCBusComponent> {
        TEMPLATABLE_VALUE(bool, programming_mode)

        public:
            void play(const Ts &...x) override {
                this->parent_->send_telegram(TELEGRAM_TYPE_PROGRAMMING_MODE, 0, this->programming_mode_.value(x...) ? 1 : 0);
            }
    };

    // Callbacks
    class ReceivedTelegramTrigger : public Trigger<TelegramData> {
        public:
            explicit ReceivedTelegramTrigger(TCBusComponent *parent) {
                parent->add_received_telegram_callback([this](const TelegramData &value) { this->trigger(value); });
            }
    };

    class SystemDiscoveryCompleteTrigger : public Trigger<> {
        public:
            explicit SystemDiscoveryCompleteTrigger(TCBusComponent *parent) {
                parent->add_system_discovery_complete_callback([this]() { this->trigger(); });
            }
    };
}