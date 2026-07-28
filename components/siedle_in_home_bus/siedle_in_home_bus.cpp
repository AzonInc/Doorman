#include "siedle_in_home_bus.h"
#include "esphome/core/log.h"

namespace esphome::siedle_in_home_bus
{

    static const char *const TAG = "siedle_in_home_bus";

    static constexpr uint32_t BIT_DURATION_US = 2000;
    static constexpr uint8_t TICKS_PER_BIT = 4;
    static constexpr uint8_t RECEIVE_QUEUE_SIZE = 16;

    void SiedleInHomeBusComponent::setup()
    {
        this->carrier_pin_->setup();
        this->rx_pin_->setup();

        this->load_pin_->setup();
        this->load_pin_->digital_write(false);
        this->tx_pin_->setup();
        this->tx_pin_->digital_write(true);

        this->store_.carrier_pin = this->carrier_pin_->to_isr();
        this->store_.rx_pin = this->rx_pin_->to_isr();
        this->store_.load_pin = this->load_pin_->to_isr();
        this->store_.tx_pin = this->tx_pin_->to_isr();

        this->store_.receive_queue = xQueueCreate(RECEIVE_QUEUE_SIZE, sizeof(uint32_t));

        const esp_timer_create_args_t timer_args = {
            .callback = reinterpret_cast<esp_timer_cb_t>(&SiedleInHomeBusStore::timer_intr),
            .arg = &this->store_,
            .dispatch_method = ESP_TIMER_ISR,
            .name = "siedle_bus_timer",
        };
        esp_timer_create(&timer_args, &this->store_.bus_timer);

        this->carrier_pin_->attach_interrupt(&SiedleInHomeBusStore::gpio_intr, &this->store_,
                                             gpio::INTERRUPT_RISING_EDGE);
    }

    void SiedleInHomeBusComponent::loop()
    {
        for (auto *listener : this->listeners_)
        {
            listener->on_loop();
        }

        if (this->store_.aborted_at_bit != 0)
        {
            ESP_LOGE(TAG, "Aborted receive at bit %d", this->store_.aborted_at_bit);
            this->store_.aborted_at_bit = 0;
        }

        uint32_t raw;
        while (xQueueReceive(this->store_.receive_queue, &raw, 0) == pdTRUE)
        {
            SiedleInHomeBusMessage msg(raw);

            if (this->dump_)
            {
                ESP_LOGI(TAG, "Received %s", msg.to_string().c_str());
                msg.has_unexpected_bits();
            }

            this->message_callback_.call(msg);

            for (auto *listener : this->listeners_)
            {
                listener->on_message(msg);
            }
        }

        if (!this->send_queue_.empty() && this->store_.status == SiedleInHomeBusStore::IDLE)
        {
            uint32_t raw_to_send = this->send_queue_.front();
            this->send_queue_.pop();
            this->internal_send_message(raw_to_send);
        }
    }

    void SiedleInHomeBusComponent::dump_config()
    {
        ESP_LOGCONFIG(TAG, "Siedle In-Home Bus:");
        LOG_PIN("  Carrier Pin: ", this->carrier_pin_);
        LOG_PIN("  RX Pin:      ", this->rx_pin_);
        LOG_PIN("  Load Pin:    ", this->load_pin_);
        LOG_PIN("  TX Pin:      ", this->tx_pin_);
    }

    void SiedleInHomeBusComponent::register_listener(SiedleInHomeBusListener *listener)
    {
        this->listeners_.push_back(listener);
    }

    void SiedleInHomeBusComponent::send_message(const SiedleInHomeBusMessage &msg)
    {
        if (!this->send_queue_.push(msg.get_raw()))
        {
            ESP_LOGW(TAG, "Send queue full, dropping message");
        }
    }

    void SiedleInHomeBusComponent::internal_send_message(uint32_t raw)
    {
        ESP_LOGD(TAG, "Sending 0x%08" PRIX32, raw);

        portDISABLE_INTERRUPTS();
        if (this->store_.status != SiedleInHomeBusStore::IDLE)
        {
            portENABLE_INTERRUPTS();
            ESP_LOGE(TAG, "Cannot send: bus not idle (status=%d)", this->store_.status);
            return;
        }

        this->store_.transferred_msg = raw;
        this->store_.status = SiedleInHomeBusStore::SENDING;
        this->store_.bits_left = 31;
        this->store_.bit_ticks_left = TICKS_PER_BIT;

        // Write MSB (bit 31) immediately; ISR handles bits 30..0
        this->load_pin_->digital_write(true);
        this->tx_pin_->digital_write((raw >> 31) & 0x01);

        esp_timer_start_periodic(this->store_.bus_timer, BIT_DURATION_US / TICKS_PER_BIT);
        portENABLE_INTERRUPTS();
    }

    void IRAM_ATTR HOT SiedleInHomeBusStore::gpio_intr(SiedleInHomeBusStore *arg)
    {
        if (arg->status != IDLE)
        {
            return;
        }

        arg->status = RECEIVING;
        arg->transferred_msg = 0;
        arg->bits_left = 32;
        arg->bit_ticks_left = TICKS_PER_BIT / 2;  // sample at midpoint of first bit

        esp_timer_start_periodic(arg->bus_timer, BIT_DURATION_US / TICKS_PER_BIT);
    }

    void IRAM_ATTR HOT SiedleInHomeBusStore::timer_intr(SiedleInHomeBusStore *arg)
    {
        if (arg->status == IDLE)
        {
            return;
        }

        if (--arg->bit_ticks_left > 0)
        {
            return;
        }

        switch (arg->status)
        {
            case RECEIVING:
            {
                if (!arg->carrier_pin.digital_read())
                {
                    arg->aborted_at_bit = arg->bits_left;
                    arg->status = IDLE;
                    return;
                }
                arg->transferred_msg = (arg->transferred_msg << 1) | arg->rx_pin.digital_read();
                if (--arg->bits_left > 0)
                {
                    arg->bit_ticks_left = TICKS_PER_BIT;
                }
                else
                {
                    BaseType_t higher_prio_woken = pdFALSE;
                    uint32_t msg = arg->transferred_msg;
                    xQueueSendFromISR(arg->receive_queue, &msg, &higher_prio_woken);
                    arg->status = TERMINATING;
                    arg->bit_ticks_left = 2 * TICKS_PER_BIT;
                    if (higher_prio_woken)
                    {
                        portYIELD_FROM_ISR();
                    }
                }
                break;
            }

            case SENDING:
            {
                if (arg->bits_left > 0)
                {
                    --arg->bits_left;
                    arg->tx_pin.digital_write((arg->transferred_msg >> arg->bits_left) & 0x01);
                    arg->bit_ticks_left = TICKS_PER_BIT;
                }
                else
                {
                    arg->tx_pin.digital_write(true);
                    arg->load_pin.digital_write(false);
                    arg->status = TERMINATING;
                    arg->bit_ticks_left = 2 * TICKS_PER_BIT;
                }
                break;
            }

            case TERMINATING:
            {
                if (arg->carrier_pin.digital_read())
                {
                    arg->bit_ticks_left = 1;
                }
                else
                {
                    esp_timer_stop(arg->bus_timer);
                    arg->status = IDLE;
                }
                break;
            }

            default:
                arg->status = IDLE;
                break;
        }
    }

}  // namespace esphome::siedle_in_home_bus
