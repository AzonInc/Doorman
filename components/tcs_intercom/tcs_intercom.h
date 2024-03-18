#pragma once

#include <utility>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/automation.h"

namespace esphome
{
    namespace tcs_intercom
    {
        struct TCSIntercomData
        {
            uint32_t command;
        };

        class TCSIntercomListener
        {
            public:
                void set_command(uint32_t command) { this->command_ = command; }
                void set_auto_off(uint16_t auto_off) { this->auto_off_ = auto_off; }

                virtual void turn_on(uint32_t *timer, uint16_t auto_off) {};
                virtual void turn_off(uint32_t *timer) {};
                uint32_t timer_;
            //private:
                uint32_t command_;
                uint16_t auto_off_;
        };

        struct TCSComponentStore
        {
            static void gpio_intr(TCSComponentStore *arg);

            static volatile uint32_t s_cmd;
            static volatile uint8_t s_cmdLength;
            static volatile bool s_cmdReady;

            ISRInternalGPIOPin rx_pin;
        };

        class TCSComponent : public Component
        {
            public:
                void sending_loop();

                void set_rx_pin(InternalGPIOPin *pin) { rx_pin_ = pin; }
                void set_tx_pin(InternalGPIOPin *pin) { tx_pin_ = pin; }

                void set_event(const char *event) { this->event_ = event; }

                void setup() override;
                void dump_config() override;
                void loop() override;

                void register_listener(TCSIntercomListener *listener);
                void send_command(TCSIntercomData data);

                bool sending;
                uint32_t command;

            protected:
                InternalGPIOPin *rx_pin_;
                InternalGPIOPin *tx_pin_;
                const char* event_;
                TCSComponentStore store_;
                std::vector<TCSIntercomListener *> listeners_{};
        };

        template<typename... Ts> class TCSIntercomSendAction : public Action<Ts...>
        {
            public:
                TCSIntercomSendAction(TCSComponent *parent) : parent_(parent) {}
                TEMPLATABLE_VALUE(uint32_t, command)

                void play(Ts... x) {
                    TCSIntercomData data{};
                    data.command = this->command_.value(x...);
                    this->parent_->send_command(data);
                }

            protected:
                TCSComponent *parent_;
        };

    }  // namespace tcs_intercom

}  // namespace esphome