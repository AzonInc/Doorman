#include "protocol.h"
#include "tc_bus.h"

#include "esphome.h"
#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#include <optional>
#include <utility>
#include <vector>
#include <cinttypes>

using namespace esphome;

namespace esphome::tc_bus
{
    void TCBusComponent::setup()
    {
        ESP_LOGCONFIG(TAG, "Running setup");

        global_tc_bus = this;

        this->rx_pin_->setup();
        this->tx_pin_->setup();
        this->tx_pin_->digital_write(false);
        
        size_t queue_size = 16;

        this->telegram_receive_queue = xQueueCreate(16, sizeof(TCBusTelegramQueueItem));
        if (this->telegram_receive_queue == nullptr)
        {
            ESP_LOGE(TAG, "Failed to create telegram receive queue of size %" PRIu32, (uint32_t) queue_size);
            this->mark_failed(LOG_STR("Failed to create telegram receiver queue!"));
            return;
        }

        auto &s = this->store_;
        s.rx_pin = this->rx_pin_->to_isr();
        this->rx_pin_->attach_interrupt(TCBusComponentStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);

        #ifdef USE_BINARY_SENSOR
            // Reset Binary Sensor Listeners
            for (auto &listener : listeners_)
            {
                listener->turn_off(&listener->timer_);
            }
        #endif

        #ifdef USE_LOCK
            // Reset Lock Listeners
            for (auto &listener : lock_listeners_)
            {
                listener->lock(&listener->timer_);
            }
        #endif

        this->high_freq_.start();
    }

    void TCBusComponent::dump_config()
    {
        ESP_LOGCONFIG(TAG, "TC:BUS:");

        LOG_PIN("  RX Pin: ", this->rx_pin_);
        LOG_PIN("  TX Pin: ", this->tx_pin_);

        #ifdef USE_TEXT_SENSOR
        ESP_LOGCONFIG(TAG, "  Text Sensors:");
        LOG_TEXT_SENSOR("    ", "Last Telegram", this->bus_telegram_text_sensor_);
        #endif
    }

    void TCBusComponent::loop()
    {
        // Process received Telegrams

        TelegramData telegram;
        while (xQueueReceive(this->telegram_receive_queue, &telegram, 0) == pdTRUE)
        {
            bool is_echo = false;
            for (auto it = this->sent_telegram_history_.begin(); it != this->sent_telegram_history_.end(); ++it)
            {
                if (*it == telegram.raw)
                {
                    is_echo = true;
                    this->sent_telegram_history_.erase(it);
                    break;
                }
            }

            if (!is_echo)
            {
                TelegramData telegram_data = parseTelegram(telegram.raw, telegram.is_long, telegram.is_response, telegram.is_retransmission);
                this->handle_telegram(telegram_data);
            }
            else
            {
                ESP_LOGV(TAG, "Received telegram 0x%08X, ignoring as echo.", telegram.raw);
            }
        }

        // Process Sensors
        uint32_t now_millis = millis();

        #ifdef USE_BINARY_SENSOR
        // Turn off binary sensor after ... milliseconds
        for (auto &listener : listeners_)
        {
            if (listener->timer_ && now_millis > listener->timer_)
            {
                listener->turn_off(&listener->timer_);
            }
        }
        #endif

        // Process Telegram queue
        this->process_telegram_queue();

        #ifdef USE_LOCK
        // Lock after ... milliseconds
        for (auto &listener : lock_listeners_)
        {
            if (listener->timer_ && now_millis > listener->timer_)
            {
                listener->lock(&listener->timer_);
            }
        }
        #endif
    }

    void TCBusComponent::process_telegram_queue()
    {
        if (!this->telegram_transmit_queue.empty())
        {
            TCBusTelegramQueueItem &queue_item = this->telegram_transmit_queue.front();

            if (!this->store_.sending)
            {
                uint32_t time_since_last_bit = millis() - this->store_.last_bit_change;
                uint32_t min_gap = queue_item.telegram_data.is_response ? 5 : 130;

                if (queue_item.telegram_data.is_response && time_since_last_bit < min_gap)
                {
                    delay_microseconds_safe((min_gap - time_since_last_bit) * 1000);
                }
                else if (time_since_last_bit < min_gap)
                {
                    return;
                }

                this->transmit_telegram(queue_item.telegram_data);
                this->telegram_transmit_queue.pop();
            }
        }
    }

    void TCBusComponent::handle_telegram(TelegramData telegram_data, bool received)
    {
        if (received)
        {
            // From receiver
            ESP_LOGI(TAG,
                "Received Telegram: %s (%i-bit, 0x%08X, %s, %s)\n"
                "  Address: %i\n"
                "  Payload: 0x%X\n"
                "  Serial-Number: %i",
                telegram_type_to_string(telegram_data.type), (telegram_data.is_long ? 32 : (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS ? 4 : 16)), telegram_data.raw, telegram_data.hex, telegram_data.is_retransmission ? "retransmission" : "first", 
                telegram_data.address, 
                telegram_data.payload, 
                telegram_data.serial_number);

            // Additional information
            if(telegram_data.type == TELEGRAM_TYPE_READ_MEMORY_BLOCK)
            {
                ESP_LOGD(TAG, "  Description: Read 4 memory blocks, from %i to %i.", (telegram_data.address * 4), (telegram_data.address * 4) + 4);
            }
            else if(telegram_data.type == TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE)
            {
                ESP_LOGD(TAG, "  Description: Response to Doorman search request.");
            }

            // Fire Callback
            this->received_telegram_callback_.call(telegram_data);

            #ifdef USE_BINARY_SENSOR
            // Fire Binary Sensors
            for (auto &listener : listeners_)
            {
                bool allow_publish = false;

                uint32_t listener_telegram = listener->telegram_.value_or(0);

                if (listener_telegram != 0)
                {
                    allow_publish = (telegram_data.raw == listener_telegram);
                }
                else
                {
                    auto type = listener->type_.value_or(TELEGRAM_TYPE_UNKNOWN);
                    auto addr = listener->address_.value_or(0);
                    auto payload = listener->payload_.value_or(0);
                    auto serial = listener->serial_number_.value_or(0);

                    if (
                        telegram_data.type == type &&
                        (telegram_data.address == addr || addr == 255) &&
                        (telegram_data.payload == payload || payload == 255) &&
                        (serial == 0 || telegram_data.serial_number == serial || serial == 255)
                    )
                    {
                        allow_publish = true;
                    }
                }

                // Trigger listener binary sensor if match found
                if (allow_publish)
                {
                    listener->turn_on(&listener->timer_, listener->auto_off_);
                }
            }
            #endif

            // Process Telegram queue
            this->process_telegram_queue();
        }
        else
        {
            // From transmitter
            ESP_LOGI(TAG,
                "Sending Telegram: %s (%i-bit, 0x%08X, %s, %s)\n"
                "  Address: %i\n"
                "  Payload: 0x%X\n"
                "  Serial-Number: %i",
                telegram_type_to_string(telegram_data.type), 
                (telegram_data.is_long ? 32 : (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS ? 4 : 16)), telegram_data.raw, telegram_data.hex, telegram_data.is_retransmission ? "retransmission" : "first", 
                telegram_data.address, 
                telegram_data.payload, 
                telegram_data.serial_number);

            // Additional information
            if(telegram_data.type == TELEGRAM_TYPE_READ_MEMORY_BLOCK)
            {
                ESP_LOGD(TAG, "  Description: Read 4 memory blocks, from %i to %i.", (telegram_data.address * 4), (telegram_data.address * 4) + 4);
            }
        }

        // Sent or received - no response to identification and read memory process
        
        // Update Door Readiness Status
        if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
        {
            bool door_readiness_state = telegram_data.payload == 1;
            ESP_LOGI(TAG, "  Door readiness: %s", YESNO(door_readiness_state));
        }
        else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING)
        {
            bool talk_mode = telegram_data.payload == 1;
            ESP_LOGI(TAG, "  Talk mode: %s", talk_mode ? "Full duplex / handsfree" : "half duplex");
        }
        else if (telegram_data.type == TELEGRAM_TYPE_END_OF_DOOR_READINESS)
        {
            ESP_LOGI(TAG, "  Door readiness: %s", YESNO(false));
        }
        else if (telegram_data.type == TELEGRAM_TYPE_PROGRAMMING_MODE)
        {
            ESP_LOGI(TAG, "  Programming Mode: %s", YESNO(telegram_data.payload == 1));
            this->programming_mode_ = telegram_data.payload == 1;
        }
        else if (telegram_data.type == TELEGRAM_TYPE_SELECT_DEVICE_GROUP || telegram_data.type == TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET)
        {
            ESP_LOGV(TAG, "Save device group: %d", telegram_data.payload);
            this->selected_device_group_ = (uint8_t)telegram_data.payload;
        }
        else if (telegram_data.type == TELEGRAM_TYPE_SEARCH_DOORMAN_DEVICES)
        {
            uint8_t mac[6];
            get_mac_address_raw(mac);
            uint32_t mac_addr = (mac[3] << 16) | (mac[4] << 8) | mac[5];

            send_telegram(TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE, 0, mac_addr, 0);
        }
        else if (telegram_data.type == TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE)
        {
            uint8_t mac[3];
            mac[0] = (telegram_data.payload >> 16) & 0xFF;
            mac[1] = (telegram_data.payload >> 8) & 0xFF;
            mac[2] = telegram_data.payload & 0xFF;

            ESP_LOGI(TAG, "  Discovered Doorman MAC: %02X:%02X:%02X", mac[0], mac[1], mac[2]);
        }

        // Call remote listeners
        for (auto &entry : this->remote_listeners_)
        {
            entry.listener->on_receive(telegram_data, received);
        }

        #ifdef USE_LOCK
        if (telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR || telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR_LONG)
        {
            // Update Locks
            for (auto &listener : lock_listeners_)
            {
                if(telegram_data.address == listener->address_.value_or(0) || listener->address_.value_or(0) == 255)
                {
                    listener->unlock(&listener->timer_, listener->auto_lock_);
                }
            }
        }
        #endif

        #ifdef USE_TEXT_SENSOR
        // Publish Telegram to Last Bus Telegram Sensor
        if (this->bus_telegram_text_sensor_ != nullptr)
        {
            this->bus_telegram_text_sensor_->publish_state(telegram_data.hex);
        }
        #endif
    }

    void IRAM_ATTR HOT TCBusComponentStore::gpio_intr(TCBusComponentStore *arg)
    {
        /*if(arg->sending)
        {
            return;
        }*/

        static DecoderState state = DecoderState::WAIT_FOR_START;
        static uint8_t expected_bits = 0;
        static uint8_t bit_index = 0;
        static uint32_t telegram = 0;
        static bool telegram_is_long = false;
        static bool telegram_is_response = false;
        static uint32_t last_us = 0;
        static bool telegram_is_retransmission = false;
        static bool wait_for_response = false;

        // Calculate time difference
        uint32_t now_us = micros();
        uint32_t us = now_us - last_us;

        // Filter glitches
        if (us < PULSE_FILTER)
        {
            return;
        }

        last_us = now_us;

        // Save last bit timestamp
        arg->last_bit_change = millis();

        // Classify pulse into bit value (-1 = not a data bit)
        int bit = -1;
        if (us >= PULSE_BIT_0_MIN_US && us <= PULSE_BIT_0_MAX_US)
        {
            bit = 0;
        }
        else if (us >= PULSE_BIT_1_MIN_US && us <= PULSE_BIT_1_MAX_US)
        {
            bit = 1;
        }

        // Response gap timeout
        if (us > ACK_TIMEOUT_US)
        {
            wait_for_response = false;
        }

        // Save potential retransmission gap
        if (us >= RETRANSMISSION_GAP_MIN_US && us <= RETRANSMISSION_GAP_MAX_US)
        {
            telegram_is_retransmission = true;
        }
        else if (us > RETRANSMISSION_GAP_MAX_US)
        {
            telegram_is_retransmission = false;
        }

        // Start pulse
        if (us >= PULSE_START_MIN_US && us <= PULSE_START_MAX_US)
        {
            telegram = 0;
            telegram_is_long = false;
            telegram_is_response = false;
            bit_index = 0;
            state = DecoderState::LENGTH_BIT;
            return;
        }

        // Invalid pulse while waiting or idle → ignore
        if (state == DecoderState::WAIT_FOR_START)
        {
            return;
        }

        // Invalid pulse in active state → reset
        if (bit < 0)
        {
            state = DecoderState::WAIT_FOR_START;
            return;
        }

        switch (state)
        {
            case DecoderState::LENGTH_BIT:
            {
                telegram_is_long = (bit == 1);
                expected_bits = telegram_is_long ? 32 : wait_for_response ? 4 : 16;
                bit_index = 0;
                state = DecoderState::DATA_BITS;
                break;
            }

            case DecoderState::DATA_BITS:
            {
                telegram = (telegram << 1) | (uint32_t)bit;

                if (++bit_index >= expected_bits)
                {
                    state = DecoderState::CRC_BIT;
                }
                break;
            }

            case DecoderState::CRC_BIT:
            {
                // CRC: start value 1, XOR all data bits MSB first, length_bit excluded
                uint8_t crc = 1;
                for (int8_t i = (int8_t)expected_bits - 1; i >= 0; --i)
                {
                    crc ^= (telegram >> i) & 1u;
                }

                const bool crc_ok = (crc & 1u) == (uint8_t)bit;
                telegram_is_response = (expected_bits == 4) || (expected_bits == 32 && wait_for_response);

                if (crc_ok)
                {
                    TelegramData t;
                    t.raw = telegram;
                    t.is_long = telegram_is_long;
                    t.is_response = telegram_is_response;
                    t.is_retransmission = telegram_is_retransmission;

                    BaseType_t higher = pdFALSE;
                    xQueueSendFromISR(global_tc_bus->telegram_receive_queue, &t, &higher);

                    wait_for_response = !telegram_is_response;
                }

                state = DecoderState::WAIT_FOR_START;
                break;
            }
        }
    }

    #ifdef USE_BINARY_SENSOR
    void TCBusComponent::register_listener(TCBusListener *listener)
    {
        this->listeners_.push_back(listener);
    }
    #endif

    #ifdef USE_LOCK
    void TCBusComponent::register_lock_listener(TCBusLockListener *listener)
    {
        this->lock_listeners_.push_back(listener);
    }
    #endif

    TelegramData TCBusComponent::send_telegram(uint32_t telegram, uint32_t wait_duration)
    {
        // Determine length of telegram
        // Not reliable as its based on the 32 bit integer itself
        bool is_long = (telegram > 0xFFFF);

        TelegramData telegram_data = parseTelegram(telegram, is_long);
        return send_telegram(telegram_data, wait_duration);
    }

    TelegramData TCBusComponent::send_telegram(uint32_t telegram, bool is_long, uint32_t wait_duration)
    {
        TelegramData telegram_data = parseTelegram(telegram, is_long);
        return send_telegram(telegram_data, wait_duration);
    }

    TelegramData TCBusComponent::send_telegram(TelegramType type, uint8_t address, uint32_t payload, uint32_t serial_number, uint32_t wait_duration)
    {
        TelegramData telegram_data = buildTelegram(type, address, payload, serial_number);
        return send_telegram(telegram_data, wait_duration);
    }

    TelegramData TCBusComponent::send_telegram(TelegramData telegram_data, uint32_t wait_duration)
    {
        if (telegram_data.raw == 0  && telegram_data.type != TELEGRAM_TYPE_ACK_STATUS && telegram_data.type != TELEGRAM_TYPE_ACK_DATA)
        {
            ESP_LOGW(TAG, "Sending telegram of type %s is not yet supported.", telegram_type_to_string(telegram_data.type));
            return telegram_data;
        }

        if (!this->telegram_transmit_queue.push({telegram_data, wait_duration}))
        {
            ESP_LOGW(TAG, "Telegram queue full, dropping telegram 0x%08X", telegram_data.raw);
        }
        
        // Process Telegram queue
        this->process_telegram_queue();

        return telegram_data;
    }

    void TCBusComponent::transmit_telegram(TelegramData telegram_data)
    {   
        if (this->store_.sending)
        {
            ESP_LOGW(TAG, "Transmission of telegram %s cancelled, another transmission is in progress!", telegram_data.hex);
        }
        else
        {
            uint32_t start_millis = millis();
            uint32_t time_between = start_millis - this->store_.last_bit_change;
            ESP_LOGD(TAG, "transmit: Last bit %i ms ago", time_between);

            this->sent_telegram_history_.push_back(telegram_data.raw);

            this->store_.sending = true;

            // Start Telegram
            this->tx_pin_->digital_write(true);
            delay_microseconds_safe(PULSE_START);

            // Length: 32 or 16 (4) bits
            this->tx_pin_->digital_write(false);
            delay_microseconds_safe(telegram_data.is_long ? PULSE_BIT_1 : PULSE_BIT_0);

            // Calculate length based on telegram type
            // Status Acknowledge telegrams only have 4 bits
            uint8_t length = (telegram_data.is_long ? 32 : (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS ? 4 : 16));

            // Track checksum
            uint8_t checksm = 1;

            // Process all bits
            for (int i = length - 1; i >= 0; i--)
            {
                // Extract single bit
                bool bit = (telegram_data.raw & (1UL << i)) != 0;

                // Update checksum
                checksm ^= bit;

                // Send bit as mark/space sequence
                this->tx_pin_->digital_write(i % 2 != 0);
                delay_microseconds_safe(bit ? PULSE_BIT_1 : PULSE_BIT_0);
            }

            this->tx_pin_->digital_write(true);
            delay_microseconds_safe(checksm ? PULSE_BIT_1 : PULSE_BIT_0);
            this->tx_pin_->digital_write(false);

            this->store_.sending = false;
        }

        this->handle_telegram(telegram_data, false);
    }
}