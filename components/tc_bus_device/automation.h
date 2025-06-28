#pragma once

#include "tc_bus_device.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome
{
    namespace tc_bus
    {
        template<typename... Ts> class TCBusDeviceUpdateSettingAction : public Action<Ts...>
        {
            public:
                TCBusDeviceUpdateSettingAction(TCBusComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(SettingType, type)
                TEMPLATABLE_VALUE(uint32_t, serial_number)
                TEMPLATABLE_VALUE(uint8_t, value)

                void play(Ts... x)
                {
                    this->parent_->update_setting(this->type_.value(x...), this->value_.value(x...), this->serial_number_.value(x...));
                }

            protected:
                TCBusComponent *parent_;
        };

        template<typename... Ts> class TCBusDeviceReadMemoryAction : public Action<Ts...>
        {
            public:
                TCBusDeviceReadMemoryAction(TCBusComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(uint32_t, serial_number)

                void play(Ts... x) { this->parent_->read_memory(this->serial_number_.value(x...)); }

            protected:
                TCBusComponent *parent_;
        };

        template<typename... Ts> class TCBusDeviceIdentifyAction : public Action<Ts...>
        {
            public:
                TCBusDeviceIdentifyAction(TCBusComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(uint32_t, serial_number)
                TEMPLATABLE_VALUE(uint8_t, device_group)

                void play(Ts... x) { this->parent_->request_version(this->serial_number_.value(x...), this->device_group_.value(x...)); }

            protected:
                TCBusComponent *parent_;
        };

        class ReceivedTelegramTrigger : public Trigger<TelegramData> {
            public:
                explicit ReceivedTelegramTrigger(TCBusComponent *parent) {
                    parent->add_received_telegram_callback([this](const TelegramData &value) { this->trigger(value); });
                }
        };

        class ReadMemoryCompleteTrigger : public Trigger<std::vector<uint8_t>> {
            public:
                explicit ReadMemoryCompleteTrigger(TCBusComponent *parent) {
                    parent->add_read_memory_complete_callback([this](const std::vector<uint8_t> &value) { this->trigger(value); });
                }
        };

        class ReadMemoryTimeoutTrigger : public Trigger<> {
            public:
                explicit ReadMemoryTimeoutTrigger(TCBusComponent *parent) {
                    parent->add_read_memory_timeout_callback([this]() { this->trigger(); });
                }
        };

        class IdentifyTimeoutTrigger : public Trigger<> {
            public:
                explicit IdentifyTimeoutTrigger(TCBusComponent *parent) {
                    parent->add_identify_timeout_callback([this]() { this->trigger(); });
                }
        };
        class IdentifyUnknownTrigger : public Trigger<> {
            public:
                explicit IdentifyUnknownTrigger(TCBusComponent *parent) {
                    parent->add_identify_timeout_callback([this]() { this->trigger(); });
                }
        };
        class IdentifyCompleteTrigger : public Trigger<ModelData> {
            public:
                explicit IdentifyCompleteTrigger(TCBusComponent *parent) {
                    parent->add_identify_complete_callback([this](const ModelData &value) { this->trigger(value); });
                }
        };
    }  // namespace tc_bus
}  // namespace esphome