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

    template<typename... Ts>
    class TCBusSystemDiscoveryAction : public Action<Ts...>, public Parented<TCBusComponent> {
        TEMPLATABLE_VALUE(uint8_t, device_group)

        public:
            void play(const Ts &...x) override {
                this->parent_->discover_system_devices(this->device_group_.value(x...));
            }
    };

    template<typename... Ts>
    class TCBusAddressDiscoveryAction : public Action<Ts...>, public Parented<TCBusComponent> {
        public:
            void play(const Ts &...x) override {
                this->parent_->discover_outdoor_station_addresses();
            }
    };

    // Callbacks
    #ifdef USE_RECEIVED_TELEGRAM_CALLBACK
    class ReceivedTelegramTrigger : public Trigger<TelegramData> {
        public:
            explicit ReceivedTelegramTrigger(TCBusComponent *parent) {
                parent->add_received_telegram_callback([this](const TelegramData &value) { this->trigger(value); });
            }
    };
    #endif

    #ifdef USE_SYSTEM_DISCOVERY_COMPLETE_CALLBACK
    class SystemDiscoveryCompleteTrigger : public Trigger<uint16_t> {
        public:
            explicit SystemDiscoveryCompleteTrigger(TCBusComponent *parent) {
                parent->add_system_discovery_complete_callback([this](const uint16_t &value) { this->trigger(value); });
            }
    };
    #endif

    #ifdef USE_ADDRESS_DISCOVERY_COMPLETE_CALLBACK
    class AddressDiscoveryCompleteTrigger : public Trigger<uint8_t> {
        public:
            explicit AddressDiscoveryCompleteTrigger(TCBusComponent *parent) {
                parent->add_address_discovery_complete_callback([this](const uint8_t &value) { this->trigger(value); });
            }
    };
    #endif
}