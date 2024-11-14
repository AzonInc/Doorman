#pragma once

#include "tc_bus.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome
{
    namespace tc_bus
    {
        template<typename... Ts> class TCBusSendAction : public Action<Ts...>
        {
            public:
                TCBusSendAction(TCBusComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(uint32_t, command)
                TEMPLATABLE_VALUE(CommandType, type)
                TEMPLATABLE_VALUE(uint8_t, address)
                TEMPLATABLE_VALUE(uint32_t, payload)
                TEMPLATABLE_VALUE(uint32_t, serial_number)

                void play(Ts... x)
                {
                    if(this->command_.value(x...) == 0)
                    {
                        this->parent_->send_command(this->type_.value(x...), this->address_.value(x...), this->payload_.value(x...), this->serial_number_.value(x...));
                    }
                    else
                    {
                        this->parent_->send_command(this->command_.value(x...));
                    }
                }

            protected:
                TCBusComponent *parent_;
        };

        template<typename... Ts> class TCBusUpdateSettingAction : public Action<Ts...>
        {
            public:
                TCBusUpdateSettingAction(TCBusComponent *parent) : parent_(parent) {}
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

        template<typename... Ts> class TCBusProgrammingModeAction : public Action<Ts...>
        {
            public:
                TCBusProgrammingModeAction(TCBusComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(bool, programming_mode)

                void play(Ts... x) { this->parent_->set_programming_mode(this->programming_mode_.value(x...)); }

            protected:
                TCBusComponent *parent_;
        };

        template<typename... Ts> class TCBusReadMemoryAction : public Action<Ts...>
        {
            public:
                TCBusReadMemoryAction(TCBusComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(uint32_t, serial_number)

                void play(Ts... x) { this->parent_->read_memory(this->serial_number_.value(x...)); }

            protected:
                TCBusComponent *parent_;
        };

        class ReceivedCommandTrigger : public Trigger<CommandData> {
            public:
                explicit ReceivedCommandTrigger(TCBusComponent *parent) {
                    parent->add_received_command_callback([this](const CommandData &value) { this->trigger(value); });
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
    }  // namespace tc_bus
}  // namespace esphome