#pragma once

#include "tcs_intercom.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome
{
    namespace tcs_intercom
    {
        template<typename... Ts> class TCSIntercomSendAction : public Action<Ts...>
        {
            public:
                TCSIntercomSendAction(TCSComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(uint32_t, command)
                TEMPLATABLE_VALUE(uint32_t, address)
                TEMPLATABLE_VALUE(CommandType, type)
                TEMPLATABLE_VALUE(uint32_t, serial_number)

                void play(Ts... x)
                {
                    if(this->command_.value(x...) == 0)
                    {
                        this->parent_->send_command_generate(this->type_.value(x...), this->address_.value(x...), this->serial_number_.value(x...));
                    }
                    else
                    {
                        this->parent_->send_command(this->command_.value(x...));
                    }
                }

            protected:
                TCSComponent *parent_;
        };

        class ReceivedCommandTrigger : public Trigger<CommandData> {
            public:
                explicit ReceivedCommandTrigger(TCSComponent *parent) {
                    parent->add_received_command_callback([this](const CommandData &value) { this->trigger(value); });
                }
        };
    }
}