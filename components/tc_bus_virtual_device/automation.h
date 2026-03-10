#pragma once

#include "util.h"
#include "tc_bus_virtual_device.h"

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome::tc_bus
{
    // Actions
    template<typename... Ts>
    class TCBusVirtualDeviceSendAction : public Action<Ts...>, public Parented<TCBusVirtualDeviceComponent>
    {
        TEMPLATABLE_VALUE(TelegramType, type)
        TEMPLATABLE_VALUE(uint8_t, address)
        TEMPLATABLE_VALUE(uint32_t, payload)

        public:
            void play(const Ts &...x) override {
                this->parent_->send_telegram(this->type_.value(x...), this->address_.value(x...), this->payload_.value(x...));
            }
    };

    template<typename... Ts>
    class TCBusVirtualDeviceCallAction : public Action<Ts...>, public Parented<TCBusVirtualDeviceComponent>
    {
        TEMPLATABLE_VALUE(uint32_t, address)
        TEMPLATABLE_VALUE(bool, internal)

        public:
            void play(const Ts &...x) override {
                this->parent_->call(this->address_.value(x...), this->internal_.value(x...));
            }
    };

    template<typename... Ts>
    class TCBusVirtualDeviceAnswerCallAction : public Action<Ts...>, public Parented<TCBusVirtualDeviceComponent>
    {
        public:
            void play(const Ts &...x) override {
                this->parent_->answer_call();
            }
    };

    template<typename... Ts>
    class TCBusVirtualDeviceEndCallAction : public Action<Ts...>, public Parented<TCBusVirtualDeviceComponent>
    {
        public:
            void play(const Ts &...x) override {
                this->parent_->end_call();
            }
    };

    // Callbacks
    class IncomingCallTrigger : public Trigger<TelegramData> {
        public:
            explicit IncomingCallTrigger(TCBusVirtualDeviceComponent *parent) {
                parent->add_incoming_call_callback([this](const TelegramData &value) { this->trigger(value); });
            }
    };

    class CallStartedTrigger : public Trigger<TelegramData> {
        public:
            explicit CallStartedTrigger(TCBusVirtualDeviceComponent *parent) {
                parent->add_call_started_callback([this](const TelegramData &value) { this->trigger(value); });
            }
    };

    class CallEndedTrigger : public Trigger<TelegramData> {
        public:
            explicit CallEndedTrigger(TCBusVirtualDeviceComponent *parent) {
                parent->add_call_ended_callback([this](const TelegramData &value) { this->trigger(value); });
            }
    };

    class CallFailedTrigger : public Trigger<> {
        public:
            explicit CallFailedTrigger(TCBusVirtualDeviceComponent *parent) {
                parent->add_call_failed_callback([this]() { this->trigger(); });
            }
    };
}