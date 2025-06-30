#pragma once

#include "util.h"
#include "tc_bus_device.h"

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome
{
    namespace tc_bus
    {
        // Actions
        template<typename... Ts> class TCBusDeviceSendAction : public Action<Ts...>
        {
            public:
                TCBusDeviceSendAction(TCBusDeviceComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(TelegramType, type)
                TEMPLATABLE_VALUE(uint8_t, address)
                TEMPLATABLE_VALUE(uint32_t, payload)

                void play(Ts... x)
                {
                    this->parent_->send_telegram(this->type_.value(x...), this->address_.value(x...), this->payload_.value(x...));
                }

            protected:
                TCBusDeviceComponent *parent_;
        };

        template<typename... Ts> class TCBusDeviceUpdateSettingAction : public Action<Ts...>
        {
            public:
                TCBusDeviceUpdateSettingAction(TCBusDeviceComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(SettingType, type)
                TEMPLATABLE_VALUE(uint8_t, value)

                void play(Ts... x)
                {
                    this->parent_->update_setting(this->type_.value(x...), this->value_.value(x...));
                }

            protected:
                TCBusDeviceComponent *parent_;
        };

        template<typename... Ts> class TCBusDeviceReadMemoryAction : public Action<Ts...>
        {
            public:
                TCBusDeviceReadMemoryAction(TCBusDeviceComponent *parent) : parent_(parent) {}

                void play(Ts... x) { this->parent_->read_memory(); }

            protected:
                TCBusDeviceComponent *parent_;
        };

        template<typename... Ts> class TCBusDeviceIdentifyAction : public Action<Ts...>
        {
            public:
                TCBusDeviceIdentifyAction(TCBusDeviceComponent *parent) : parent_(parent) {}

                void play(Ts... x) { this->parent_->request_version(); }

            protected:
                TCBusDeviceComponent *parent_;
        };

        // Callbacks
        class ReadMemoryCompleteTrigger : public Trigger<std::vector<uint8_t>> {
            public:
                explicit ReadMemoryCompleteTrigger(TCBusDeviceComponent *parent) {
                    parent->add_read_memory_complete_callback([this](const std::vector<uint8_t> &value) { this->trigger(value); });
                }
        };

        class ReadMemoryTimeoutTrigger : public Trigger<> {
            public:
                explicit ReadMemoryTimeoutTrigger(TCBusDeviceComponent *parent) {
                    parent->add_read_memory_timeout_callback([this]() { this->trigger(); });
                }
        };

        class IdentifyTimeoutTrigger : public Trigger<> {
            public:
                explicit IdentifyTimeoutTrigger(TCBusDeviceComponent *parent) {
                    parent->add_identify_timeout_callback([this]() { this->trigger(); });
                }
        };
        class IdentifyUnknownTrigger : public Trigger<> {
            public:
                explicit IdentifyUnknownTrigger(TCBusDeviceComponent *parent) {
                    parent->add_identify_timeout_callback([this]() { this->trigger(); });
                }
        };
        class IdentifyCompleteTrigger : public Trigger<ModelData> {
            public:
                explicit IdentifyCompleteTrigger(TCBusDeviceComponent *parent) {
                    parent->add_identify_complete_callback([this](const ModelData &value) { this->trigger(value); });
                }
        };
    }  // namespace tc_bus
}  // namespace esphome