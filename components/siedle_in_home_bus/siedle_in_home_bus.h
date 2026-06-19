#pragma once

#include <vector>

#include "protocol.h"
#include "queue.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace esphome::siedle_in_home_bus
{

    class SiedleInHomeBusListener
    {
        public:
            virtual ~SiedleInHomeBusListener() = default;
            void set_auto_off_ms(uint32_t ms) { this->auto_off_ms_ = ms; }
            virtual void on_message(const SiedleInHomeBusMessage &msg) = 0;
            virtual void on_loop() {}

        protected:
            uint32_t auto_off_ms_{0};
            uint32_t auto_off_timer_{0};
    };

    struct SiedleInHomeBusStore
    {
        static void gpio_intr(SiedleInHomeBusStore *arg);
        static void timer_intr(SiedleInHomeBusStore *arg);

        ISRInternalGPIOPin carrier_pin;
        ISRInternalGPIOPin rx_pin;
        ISRInternalGPIOPin load_pin;
        ISRInternalGPIOPin tx_pin;

        enum BusStatus : uint8_t { IDLE, RECEIVING, SENDING, TERMINATING };
        volatile BusStatus status{IDLE};
        volatile uint32_t transferred_msg{0};
        volatile uint8_t bits_left{0};
        volatile uint8_t bit_ticks_left{0};
        volatile uint8_t aborted_at_bit{0};

        QueueHandle_t receive_queue{nullptr};
        esp_timer_handle_t bus_timer{nullptr};
    };

    class SiedleInHomeBusComponent : public Component
    {
        public:
            void set_carrier_pin(InternalGPIOPin *pin) { this->carrier_pin_ = pin; }
            void set_rx_pin(InternalGPIOPin *pin) { this->rx_pin_ = pin; }
            void set_load_pin(InternalGPIOPin *pin) { this->load_pin_ = pin; }
            void set_tx_pin(InternalGPIOPin *pin) { this->tx_pin_ = pin; }
            void set_dump(bool dump) { this->dump_ = dump; }

            float get_setup_priority() const override { return setup_priority::BUS; }
            void setup() override;
            void loop() override;
            void dump_config() override;

            void register_listener(SiedleInHomeBusListener *listener);
            void send_message(const SiedleInHomeBusMessage &msg);

            template<typename F> void add_on_message_callback(F &&callback)
            {
                this->message_callback_.add(std::forward<F>(callback));
            }

        protected:
            InternalGPIOPin *carrier_pin_{nullptr};
            InternalGPIOPin *rx_pin_{nullptr};
            InternalGPIOPin *load_pin_{nullptr};
            InternalGPIOPin *tx_pin_{nullptr};
            bool dump_{false};

            SiedleInHomeBusStore store_{};
            std::vector<SiedleInHomeBusListener *> listeners_;
            FixedQueue<uint32_t, 4> send_queue_;

            void internal_send_message(uint32_t raw);

            LazyCallbackManager<void(SiedleInHomeBusMessage)> message_callback_{};
    };

    static SiedleInHomeBusComponent *global_siedle_in_home_bus = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace esphome::siedle_in_home_bus
