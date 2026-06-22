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

        uint32_t hash = fnv1_hash("tc_bus");
        this->pref_ = global_preferences->make_preference<TCBusSettings>(hash, true);

        // Restore settings
        TCBusSettings recovered{};
        if (!this->pref_.load(&recovered))
        {
            // No settings available
            ESP_LOGW(TAG, "Unable to recover preferences");
        }
        else
        {
            this->entrance_address_ = recovered.entrance_address;
            this->second_entrance_address_ = recovered.second_entrance_address;
        }

        this->rx_pin_->setup();
        this->tx_pin_->setup();
        this->tx_pin_->digital_write(false);
        
        this->telegram_receive_queue = xQueueCreate(RECEIVE_QUEUE_SIZE, sizeof(TelegramData));
        if (this->telegram_receive_queue == nullptr)
        {
            ESP_LOGE(TAG, "Failed to create telegram receive queue of size %d", RECEIVE_QUEUE_SIZE);
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

            // Reset door readiness sensor
            if(this->door_readiness_binary_sensor_ != nullptr)
            {
                this->door_readiness_binary_sensor_->publish_state(false);
            }
        #endif

        #ifdef USE_LOCK
            // Reset Lock Listeners
            for (auto &listener : lock_listeners_)
            {
                listener->lock(&listener->timer_);
            }
        #endif

        #ifdef USE_NUMBER
        if (this->entrance_address_number_ != nullptr)
        {
            this->entrance_address_number_->publish_state(this->entrance_address_);
        }
        if (this->second_entrance_address_number_ != nullptr)
        {
            this->second_entrance_address_number_->publish_state(this->second_entrance_address_);
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
            
            if (this->store_.expect_echo && telegram.raw == this->store_.last_telegram_raw)
            {
                this->store_.expect_echo = false;
                is_echo = true;
            }

            if (!is_echo)
            {
                this->store_.retransmission_pending = false;

                TelegramData telegram_data = parseTelegram(telegram.raw, telegram.is_long, telegram.is_response, telegram.is_retransmission);
                this->handle_telegram(telegram_data, TelegramSource::BUS_RECEIVED);
            }
            else
            {
                ESP_LOGV(TAG, "Received telegram 0x%08X, ignoring as echo.", telegram.raw);
            }
        }

        uint32_t now_millis = millis();

        // Process retransmission if pending
        if (this->store_.retransmission_pending)
        {
            uint32_t elapsed_us = micros() - this->retransmit_wait_start_us_;
            if (elapsed_us >= RETRANSMISSION_GAP_US)
            {
                this->store_.retransmission_pending = false;
                this->transmit_telegram(this->retransmit_telegram_, this->retransmit_sender_listener_id_);
            }
        }

        // Address discovery complete
        if (this->address_discovery_timeout_ != 0 && now_millis > this->address_discovery_timeout_)
        {
            this->address_discovery_active_ = false;
            this->address_discovery_timeout_ = 0; // Reset

            std::sort(this->address_discovery_as_, this->address_discovery_as_ + this->address_discovery_as_cnt_);

            if(this->address_discovery_as_cnt_ > 0)
            {
                ESP_LOGI(TAG, "Outdoor station address discovery complete: %i outdoor stations found", this->address_discovery_as_cnt_);
                if(this->address_discovery_as_cnt_ > 2)
                {
                    ESP_LOGW(TAG, "Please check the addresses and set them manually if needed (found more than 2).");
                }

                if(this->address_discovery_as_cnt_ == 1)
                {
                    this->entrance_address_ = this->address_discovery_as_[0];
                    this->entrance_address_number_->publish_state(this->entrance_address_);

                    this->second_entrance_address_ = 63; // Reset to default invalid address
                    this->second_entrance_address_number_->publish_state(this->second_entrance_address_);
                }
                else if(this->address_discovery_as_cnt_ >= 2)
                {
                    this->entrance_address_ = this->address_discovery_as_[0];
                    this->entrance_address_number_->publish_state(this->entrance_address_);

                    this->second_entrance_address_ = this->address_discovery_as_[1];
                    this->second_entrance_address_number_->publish_state(this->second_entrance_address_);
                }
                save_preferences();
            }
            else
            {
                ESP_LOGE(TAG, "Outdoor station discovery complete: No outdoor stations found");
            }

            #ifdef USE_ADDRESS_DISCOVERY_COMPLETE_CALLBACK
            this->address_discovery_complete_callback_.call(this->address_discovery_as_cnt_);
            #endif
        }

        // Process Sensors
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

        {
            uint8_t tail = this->store_.debug_buf_tail;
            const uint8_t head = this->store_.debug_buf_head; // snapshot; ISR only advances head

            if (tail != head && (micros() - this->store_.last_bit_change) >= 8000)
            {
                char buffer[256];
                size_t pos = buf_append_printf(buffer, sizeof(buffer), 0, "Timings: ");

                while (tail != head)
                {
                    const uint32_t value = this->store_.debug_buffer[tail];
                    tail = (tail + 1) & (TCBusComponentStore::DEBUG_BUF_SIZE - 1);

                    size_t prev_pos = pos;
                    pos = buf_append_printf(buffer, sizeof(buffer), pos, "%" PRId32 ", ", value);

                    if (pos >= sizeof(buffer) - 1)
                    {
                        buffer[prev_pos] = '\0';
                        ESP_LOGD(TAG, "%s", buffer);
                        pos = buf_append_printf(buffer, sizeof(buffer), 0, "  %" PRId32 ", ", value);
                    }
                }

                this->store_.debug_buf_tail = tail; // consume after all reads

                if (pos >= 2)
                {
                    buffer[pos - 2] = '\0'; // trim trailing ", "
                    ESP_LOGD(TAG, "%s", buffer);
                }
            }
        }
    }

    void TCBusComponent::save_preferences()
    {
        TCBusSettings settings{};
        settings.entrance_address = this->entrance_address_;
        settings.second_entrance_address = this->second_entrance_address_;

        if (!this->pref_.save(&settings))
        {
            ESP_LOGW(TAG, "Failed to save settings to flash memory.");
        }
    }

    void TCBusComponent::process_telegram_queue()
    {
        if (this->store_.retransmission_pending)
        {
            return;
        }

        if (!this->telegram_transmit_queue.empty())
        {
            TCBusTelegramQueueItem &queue_item = this->telegram_transmit_queue.front();

            if (!this->store_.sending)
            {
                uint32_t time_since_last_bit = micros() - this->store_.last_bit_change;
                uint32_t min_gap = queue_item.telegram_data.is_response ? 5000 : 130000;

                if(time_since_last_bit < min_gap)
                {
                    if (queue_item.telegram_data.is_response)
                    {
                        delay_microseconds_safe(min_gap - time_since_last_bit);
                    }
                    else
                    {
                        return;
                    }
                }

                this->transmit_telegram(queue_item.telegram_data, queue_item.sender_listener_id);
                this->telegram_transmit_queue.pop();
            }
        }
    }

    void TCBusComponent::notify_peer_listeners(const TelegramData& telegram_data, uint8_t sender_listener_id)
    {
        for (auto &entry : this->remote_listeners_)
        {
            if (entry.listener->get_listener_id() == sender_listener_id)
            {
                entry.listener->on_receive(telegram_data, TelegramSource::LOCAL_SENT);
            }
            else
            {
                entry.listener->on_receive(telegram_data, TelegramSource::PEER_SENT);
            }
        }
    }

    void TCBusComponent::handle_telegram(const TelegramData& telegram_data, TelegramSource source)
    {
        const bool received = (source == TelegramSource::BUS_RECEIVED);

        if (received)
        {
            // From receiver
            if(telegram_data.type == TELEGRAM_TYPE_ACK_STATUS)
            {
                ESP_LOGI(TAG, "Received: %s (b%c%c%c%c%s)",
                              telegram_type_to_string(telegram_data.type),
                              (telegram_data.raw >> 3) & 1 ? '1' : '0',
                              (telegram_data.raw >> 2) & 1 ? '1' : '0',
                              (telegram_data.raw >> 1) & 1 ? '1' : '0',
                              (telegram_data.raw >> 0) & 1 ? '1' : '0',
                              telegram_data.is_retransmission ? ", retransmission" : "");
            }
            else
            {
                char hex_buf[9];
                format_telegram_hex(telegram_data.raw, telegram_data.is_long, telegram_data.type, hex_buf);
                ESP_LOGI(TAG, "Received: %s (0x%s%s)",
                              telegram_type_to_string(telegram_data.type),
                              hex_buf,
                              telegram_data.is_retransmission ? ", retransmission" : "");
            }

            if(telegram_data.type != TELEGRAM_TYPE_ACK_STATUS && telegram_data.type != TELEGRAM_TYPE_ACK_DATA)
            {
                ESP_LOGD(TAG,   "  Address: %i\n"
                                "  Payload: 0x%X\n"
                                "  Serial-Number: %i",
                                telegram_data.address, 
                                telegram_data.payload, 
                                telegram_data.serial_number);
            }

            if (telegram_data.type == TELEGRAM_TYPE_SEARCH_DOORMAN_DEVICES)
            {
                uint8_t mac[6];
                get_mac_address_raw(mac);
                uint32_t mac_addr = (mac[3] << 16) | (mac[4] << 8) | mac[5];

                send_telegram(TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE, 0, mac_addr);
            }
            else if (telegram_data.type == TELEGRAM_TYPE_FOUND_DOORMAN_DEVICE)
            {
                uint8_t mac[3];
                mac[0] = (telegram_data.payload >> 16) & 0xFF;
                mac[1] = (telegram_data.payload >> 8) & 0xFF;
                mac[2] = telegram_data.payload & 0xFF;

                ESP_LOGI(TAG, "  Discovered Doorman MAC: %02X:%02X:%02X", mac[0], mac[1], mac[2]);
            }
            else if(telegram_data.type == TELEGRAM_TYPE_ACK_DATA)
            {
                if(this->error_protocol_pending_)
                {
                    this->error_protocol_pending_ = false;

                    uint8_t error_type = (telegram_data.raw >> 24) & 0xF;
                    uint8_t device_group = (telegram_data.raw >> 20) & 0xF;

                    ESP_LOGD(TAG,   "  Description: Error protocol data\n"
                                    "    Device Group: %d", device_group);

                    switch(error_type)
                    {
                        case 1:
                            if(device_group == 2)
                            {
                                uint8_t sub_type = (telegram_data.raw >> 8) & 0xF;
                                uint8_t key_x = (telegram_data.raw >> 4) & 0xF;
                                uint8_t key_y = telegram_data.raw & 0xF;

                                if(sub_type == 0)
                                {
                                    ESP_LOGD(TAG,   "    Message: Key %d;%d stuck", key_x, key_y);
                                }
                                else
                                {
                                    ESP_LOGD(TAG,   "    Message: Key %d;%d of extension %d stuck", key_x, key_y, sub_type);
                                }
                            }
                            else
                            {
                                uint16_t key = telegram_data.raw & 0xFFF;
                                ESP_LOGD(TAG,   "    Message: Key %d stuck", key);
                            }
                            break;
                        case 2:
                            ESP_LOGD(TAG,   "    Message: EEPROM misplaced");
                            break;
                        case 3:
                            ESP_LOGD(TAG,   "    Message: Key extension error");
                            break;
                        case 5:
                            ESP_LOGD(TAG,   "    Message: Interface error");
                            break;
                        case 6:
                            ESP_LOGD(TAG,   "    Message: Subsystem error");
                            break;
                        default:
                            ESP_LOGD(TAG,   "    Message: Unknown Error");
                            break;
                    }
                }
            }

            // Fire Callback
            #ifdef USE_RECEIVED_TELEGRAM_CALLBACK
            this->received_telegram_callback_.call(telegram_data);
            #endif

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
                    listener->turn_on(&listener->timer_, listener->auto_reset_);
                }
            }
            #endif

            // Process Telegram queue
            this->process_telegram_queue();
        }
        else
        {
            // From transmitter
            if(telegram_data.type == TELEGRAM_TYPE_ACK_STATUS)
            {
                ESP_LOGI(TAG, "Sending: %s (b%c%c%c%c%s)",
                              telegram_type_to_string(telegram_data.type),
                              (telegram_data.raw >> 3) & 1 ? '1' : '0',
                              (telegram_data.raw >> 2) & 1 ? '1' : '0',
                              (telegram_data.raw >> 1) & 1 ? '1' : '0',
                              (telegram_data.raw >> 0) & 1 ? '1' : '0',
                              telegram_data.is_retransmission ? ", retransmission" : "");
            }
            else
            {
                char hex_buf[9];
                format_telegram_hex(telegram_data.raw, telegram_data.is_long, telegram_data.type, hex_buf);
                ESP_LOGI(TAG, "Sending: %s (0x%s%s)",
                              telegram_type_to_string(telegram_data.type),
                              hex_buf,
                              telegram_data.is_retransmission ? ", retransmission" : "");
            }

            if(telegram_data.type != TELEGRAM_TYPE_ACK_STATUS && telegram_data.type != TELEGRAM_TYPE_ACK_DATA)
            {
                ESP_LOGD(TAG,   "  Address: %i\n"
                                "  Payload: 0x%X\n"
                                "  Serial-Number: %i",
                                telegram_data.address, 
                                telegram_data.payload, 
                                telegram_data.serial_number);
            }
        }

        // Sent or received - no response to identification and read memory process
        
        if(telegram_data.type == TELEGRAM_TYPE_READ_MEMORY_BLOCK)
        {
            ESP_LOGD(TAG, "  Description: Read 4 bytes, from address %i to %i.", telegram_data.address, telegram_data.address + 4);
        }
        else if(telegram_data.type == TELEGRAM_TYPE_CONTROL_FUNCTION)
        {
            switch(telegram_data.payload)
            {
                case 0xC9:
                    ESP_LOGD(TAG, "  Description: Reading confirmation");
                    break;
                case 0xCA:
                    ESP_LOGD(TAG, "  Description: Synchronization of parallel devices");
                    break;
                case 0xCF:
                    ESP_LOGD(TAG, "  Description: Internal call sender serial number");
                    break;
                case 0xD7:
                    ESP_LOGD(TAG, "  Description: Individual device reset");
                    break;
                case 0xD8:
                    this->error_protocol_pending_ = true;
                    ESP_LOGD(TAG, "  Description: Error protocol");
                    break;
                case 0xEE:
                    ESP_LOGD(TAG, "  Description: Switch on video screen");
                    break;
                default:
                    ESP_LOGD(TAG, "  Description: Control function %d", telegram_data.payload);
                    break;
            }
        }
        else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
        {
            ESP_LOGD(TAG, "  Door readiness: %s", YESNO(telegram_data.payload));
        }
        else if (telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR || telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR_LONG)
        {
            ESP_LOGD(TAG, "  Door readiness: %s", YESNO(telegram_data.payload));
        }
        else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING)
        {
            ESP_LOGD(TAG, "  Talk mode: %s", telegram_data.payload == 1 ? "Full duplex" : "Half duplex");
        }
        else if (telegram_data.type == TELEGRAM_TYPE_DOOR_CALL)
        {
            this->door_readiness_active_ = true;

            #ifdef USE_BINARY_SENSOR
            if (this->door_readiness_binary_sensor_ != nullptr)
            {
                this->door_readiness_binary_sensor_->publish_state(true);
            }
            #endif
        }
        else if (telegram_data.type == TELEGRAM_TYPE_END_OF_DOOR_READINESS)
        {
            // Note:
            // Does not take the address into account, as this telegram is usually sent
            // by the outdoor station for the indoor station, and the indoor station
            // is the only one that reacts to it by changing its door readiness status
            
            this->door_readiness_active_ = false;

            #ifdef USE_BINARY_SENSOR
            if (this->door_readiness_binary_sensor_ != nullptr)
            {
                this->door_readiness_binary_sensor_->publish_state(false);
            }
            #endif
        }
        else if (telegram_data.type == TELEGRAM_TYPE_RESET && this->selected_device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            this->door_readiness_active_ = false;

            #ifdef USE_BINARY_SENSOR
            if (this->door_readiness_binary_sensor_ != nullptr)
            {
                this->door_readiness_binary_sensor_->publish_state(false);
            }
            #endif
        }
        else if (telegram_data.type == TELEGRAM_TYPE_INITIALIZE_DOOR_STATION)
        {
            this->door_readiness_active_ = false;

            #ifdef USE_BINARY_SENSOR
            if (this->door_readiness_binary_sensor_ != nullptr)
            {
                this->door_readiness_binary_sensor_->publish_state(false);
            }
            #endif

            // Add AS address to discovery if active
            if(this->address_discovery_active_)
            {
                this->address_discovery_timeout_ = millis() + 10000;

                for(uint8_t i = 0; i < this->address_discovery_as_cnt_; i++)
                {
                    if(this->address_discovery_as_[i] == telegram_data.address)
                    {
                        return;
                    }
                }

                if(this->address_discovery_as_cnt_ < 5)
                {
                    this->address_discovery_as_[this->address_discovery_as_cnt_++] = telegram_data.address;
                }
            }
        }
        else if (telegram_data.type == TELEGRAM_TYPE_PROGRAMMING_MODE)
        {
            ESP_LOGD(TAG, "  Programming Mode: %s", ONOFF(telegram_data.payload));
            this->programming_mode_ = telegram_data.payload == 1;
        }
        else if (telegram_data.type == TELEGRAM_TYPE_SELECT_DEVICE_GROUP || telegram_data.type == TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET)
        {
            ESP_LOGV(TAG, "Save device group: %d", telegram_data.payload);
            this->selected_device_group_ = (uint8_t)telegram_data.payload;
        }
        else if(telegram_data.type == TELEGRAM_TYPE_FOUND_DEVICE && this->system_discovery_active_)
        {
            uint32_t* arr = nullptr;
            uint8_t*  cnt = nullptr;
            uint8_t   max = 0;

            switch(this->selected_device_group_)
            {
                case 0: arr = system_discovery_is_classic_; cnt = &system_discovery_is_classic_cnt_; max = 50; break;
                case 1: arr = system_discovery_is_handsfree_; cnt = &system_discovery_is_handsfree_cnt_; max = 50; break;
                case 2: arr = system_discovery_as_;  cnt = &system_discovery_as_cnt_;  max = 5;  break;
                case 4: arr = system_discovery_ctr_; cnt = &system_discovery_ctr_cnt_; max = 5;  break;
                case 6: arr = system_discovery_ext_; cnt = &system_discovery_ext_cnt_; max = 5;  break;
                case 11: arr = system_discovery_acc_; cnt = &system_discovery_acc_cnt_; max = 5;  break;
                default: return;
            }

            for(uint8_t i = 0; i < *cnt; i++)
            {
                if(arr[i] == telegram_data.serial_number)
                {
                    return;
                }
            }

            if(*cnt < max)
            {
                arr[(*cnt)++] = telegram_data.serial_number;
            }

            this->cancel_timeout(0xDD);
            this->set_timeout(0xDD, 4000, [this]()
            {
                if(this->system_discovery_full_scan_)
                {
                    uint8_t next_group = 255;
                    switch(this->selected_device_group_)
                    {
                        case 0: next_group = 1; break;
                        case 1: next_group = 2; break;
                        case 2: next_group = 4; break;
                        case 4: next_group = 6; break;
                        case 6: next_group = 11; break;
                    }

                    if(next_group == 255)
                    {
                        ESP_LOGD(TAG, "Finished scanning device groups.");
                        finish_system_discovery();
                    }
                    else
                    {
                        ESP_LOGD(TAG, "Finished scanning device group %d, next group: %d.", this->selected_device_group_, next_group);
                        discover_system_devices(next_group);
                    }
                }
                else
                {
                    finish_system_discovery();
                }
            });
        }

        if (source != TelegramSource::LOCAL_SENT)
        {
            for (auto &entry : this->remote_listeners_)
            {
                entry.listener->on_receive(telegram_data, source);
            }
        }

        #ifdef USE_LOCK
        if (telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR || telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR_LONG)
        {
            // Update Locks
            for (auto &listener : lock_listeners_)
            {
                if(telegram_data.address == listener->address_.value_or(0) || listener->address_.value_or(0) == 255)
                {
                    listener->unlock(&listener->timer_, listener->auto_reset_);
                }
            }
        }
        #endif

        #ifdef USE_TEXT_SENSOR
        // Publish Telegram to Last Bus Telegram Sensor
        if (this->bus_telegram_text_sensor_ != nullptr)
        {
            char hex_buf[9];
            format_telegram_hex(telegram_data.raw, telegram_data.is_long, telegram_data.type, hex_buf);
            this->bus_telegram_text_sensor_->publish_state(hex_buf);
        }
        #endif
    }

    void IRAM_ATTR HOT TCBusComponentStore::gpio_intr(TCBusComponentStore *arg)
    {
        static uint8_t state = 0; // 0=WAIT, 1=LENGTH, 2=DATA, 3=PARITY
        static uint8_t expected_bits = 0;
        static uint8_t bit_index = 0;
        static uint32_t telegram = 0;
        static uint32_t last_us = 0;

        static bool telegram_is_long = false;
        static bool telegram_is_response = false;
        static bool telegram_is_retransmission = false;
        static bool wait_for_response = false;

        // Calculate time difference
        const uint32_t now_us = micros();
        const uint32_t us = now_us - last_us;

        // Filter glitches
        if (us < PULSE_FILTER)
        {
            return;
        }

        {
            uint8_t head = arg->debug_buf_head;
            uint8_t next = (head + 1) & (DEBUG_BUF_SIZE - 1);
            if (next != arg->debug_buf_tail) // drop silently only when truly full
            {
                arg->debug_buffer[head] = us;
                arg->debug_buf_head = next; // publish after write
            }
        }

        // Save last bit timestamp
        last_us = now_us;
        arg->last_bit_change = now_us;

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
            arg->expect_echo_isr = false;
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
            state = 1;

            if (arg->expect_echo_isr)
            {
                arg->expect_echo_isr = false;
            }
            else if (arg->retransmission_pending)
            {
                arg->retransmission_pending = false;
            }
            return;
        }

        // Invalid pulse while waiting or idle - ignore
        if (state == 0)
        {
            return;
        }

        // Invalid pulse in active state - reset
        if (bit < 0)
        {
            state = 0;
            return;
        }

        if(state == 1)
        {
            telegram_is_long = (bit == 1);
            expected_bits = telegram_is_long ? 32 : wait_for_response ? 4 : 16;
            bit_index = 0;
            state = 2;
        }
        else if(state == 2)
        {
            telegram = (telegram << 1) | bit;
            if (++bit_index >= expected_bits)
            {
                state = 3;
            }
        }
        else if(state == 3)
        {
            // Start value 1: parity(telegram) XOR 1, then compare to received bit
            const bool parity_ok = (__builtin_parity(telegram) ^ 1) == bit;

            telegram_is_response = (expected_bits == 4) || (expected_bits == 32 && wait_for_response);

            if (parity_ok)
            {
                TelegramData t;
                t.raw = telegram;
                t.is_long = telegram_is_long;
                t.is_response = telegram_is_response;

                if(arg->last_telegram_raw == telegram && telegram_is_retransmission)
                {
                    t.is_retransmission = true;
                }

                arg->last_telegram_raw = telegram;
                telegram_is_retransmission = false; // reset

                BaseType_t higher = pdFALSE;
                xQueueSendFromISR(global_tc_bus->telegram_receive_queue, &t, &higher);
                portYIELD_FROM_ISR(higher);

                wait_for_response = !telegram_is_response;
            }

            state = 0;
        }
    }

    #ifdef USE_BINARY_SENSOR
    void TCBusComponent::register_listener(TCBusBinarySensorListener *listener)
    {
        this->listeners_.push_back(listener);
    }
    #endif

    #ifdef USE_LOCK
    void TCBusComponent::register_listener(TCBusLockListener *listener)
    {
        this->lock_listeners_.push_back(listener);
    }
    #endif

    TelegramData TCBusComponent::send_telegram(const TelegramData& telegram_data, uint8_t sender_listener_id)
    {
        if (telegram_data.raw == 0  && telegram_data.type != TELEGRAM_TYPE_ACK_STATUS && telegram_data.type != TELEGRAM_TYPE_ACK_DATA)
        {
            ESP_LOGW(TAG, "Sending telegram of type %s is not yet supported.", telegram_type_to_string(telegram_data.type));
            return telegram_data;
        }

        if (!this->telegram_transmit_queue.push({telegram_data, sender_listener_id}))
        {
            ESP_LOGW(TAG, "Telegram queue full, dropping telegram 0x%08X", telegram_data.raw);
        }
        
        // Process Telegram queue
        this->process_telegram_queue();

        return telegram_data;
    }

    void TCBusComponent::transmit_telegram(const TelegramData& telegram_data, uint8_t sender_listener_id)
    {   
        if (this->store_.sending)
        {
            char hex_buf[9];
            format_telegram_hex(telegram_data.raw, telegram_data.is_long, telegram_data.type, hex_buf);
            ESP_LOGW(TAG, "Transmission of telegram %s cancelled, another transmission is in progress!", hex_buf);
        }
        else
        {
            uint32_t start_us = micros();
            uint32_t time_between = start_us - this->store_.last_bit_change;
            ESP_LOGV(TAG, "Last bit %i us ago", time_between);

            this->store_.sending = true;
            this->store_.last_telegram_raw = telegram_data.raw;
            this->store_.expect_echo = true;
            this->store_.expect_echo_isr = true;

            // Calculate length based on telegram type
            // Status Acknowledge telegrams only have 4 bits
            uint8_t length = (telegram_data.is_long ? 32 : (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS ? 4 : 16));

            uint32_t data = telegram_data.raw;

            // Parity bit (odd parity)
            uint8_t parity = __builtin_parity(data) ^ 1;

            // Begin transmission
            this->tx_pin_->digital_write(true);
            delay_microseconds_safe(PULSE_START);

            // Length: 32 or 16 (4) bits
            this->tx_pin_->digital_write(false);
            delay_microseconds_safe(telegram_data.is_long ? PULSE_BIT_1 : PULSE_BIT_0);

            // Process bits
            for (int i = length; i-- > 0;)
            {
                bool bit = (data >> i) & 1;
                this->tx_pin_->digital_write(i & 1);
                delay_microseconds_safe(bit ? PULSE_BIT_1 : PULSE_BIT_0);
            }

            // Parity
            this->tx_pin_->digital_write(true);
            delay_microseconds_safe(parity ? PULSE_BIT_1 : PULSE_BIT_0);
            this->tx_pin_->digital_write(false);

            this->store_.sending = false;

            if (!telegram_data.is_retransmission && telegram_data.type != TELEGRAM_TYPE_ACK_STATUS && telegram_data.type != TELEGRAM_TYPE_ACK_DATA)
            {
                this->store_.retransmission_pending = true;
                this->retransmit_wait_start_us_ = micros();
                this->retransmit_telegram_ = telegram_data;
                this->retransmit_telegram_.is_retransmission = true;
                this->retransmit_sender_listener_id_ = sender_listener_id;
            }

            this->handle_telegram(telegram_data, TelegramSource::LOCAL_SENT);

            if (sender_listener_id != 0)
            {
                this->notify_peer_listeners(telegram_data, sender_listener_id);
            }
            else
            {
                for (auto &entry : this->remote_listeners_)
                {
                    entry.listener->on_receive(telegram_data, TelegramSource::BUS_RECEIVED);
                }
            }
        }
    }

    void TCBusComponent::discover_outdoor_station_addresses()
    {
        ESP_LOGI(TAG, "Start outdoor station address discovery");

        this->address_discovery_active_ = true;
        this->address_discovery_timeout_ = millis() + 20000;

        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET, 0, 2, 0);
    }

    void TCBusComponent::discover_system_devices(uint8_t device_group)
    {
        if(device_group == 255)
        {
            this->system_discovery_full_scan_ = true;
            device_group = 0; // full scan starts with 0
        }
        else
        {
            if(device_group != 0 && device_group != 1 && device_group != 2 && device_group != 4 && device_group != 6 && device_group != 11)
            {
                ESP_LOGE(TAG, "Unsupported device group %d for system discovery, aborting.", device_group);
                return;
            }
        }

        if(this->system_discovery_active_ == false)
        {
            if(this->system_discovery_full_scan_)
            {
                ESP_LOGI(TAG, "Start system discovery: All Device Groups");
            }
            else
            {
                ESP_LOGI(TAG, "Start system discovery: Device Group %d", device_group);
            }

            this->system_discovery_active_ = true;
        }
        else
        {
            ESP_LOGI(TAG, "Continue system discovery: Device Group %d", device_group);
        }

        // 0 = Indoor stations 0
        // 1 = Indoor stations 1
        // 2 = Outdoor stations
        // 3 = Doorbell button extension (at least one known)
        // 4 = Controllers (Power supply)
        // 7 = Bell
        // 6 = Extra modules
        // 7 = Gateways/interfaces
        // 8 = reserved
        // 9 = Guard stations
        // A = tcp3 (?)
        // B = Access control devices

        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, device_group, 0);
        send_telegram(TELEGRAM_TYPE_SEARCH_DEVICES, 0, 0);

        this->cancel_timeout(0xDD);
        this->set_timeout(0xDD, 4000, [this]()
        {
            if(this->system_discovery_full_scan_)
            {
                uint8_t next_group = 255;
                switch(this->selected_device_group_)
                {
                    case 0: next_group = 1; break;
                    case 1: next_group = 2; break;
                    case 2: next_group = 4; break;
                    case 4: next_group = 6; break;
                    case 6: next_group = 11; break;
                }

                if(next_group == 255)
                {
                    finish_system_discovery();
                }
                else
                {
                    discover_system_devices(next_group);
                }
            }
            else
            {
                finish_system_discovery();
            }
        });
    }

    void TCBusComponent::finish_system_discovery()
    {
        this->system_discovery_active_ = false;
        this->system_discovery_full_scan_ = false;

        this->cancel_timeout(0xDD);

        uint16_t device_cnt = system_discovery_is_classic_cnt_ + system_discovery_is_handsfree_cnt_ + system_discovery_as_cnt_ + system_discovery_ctr_cnt_ + system_discovery_ext_cnt_ + system_discovery_acc_cnt_;

        ESP_LOGI(TAG, "System discovery complete. Found %i devices:", device_cnt);

        log_device_list("Classic Indoor Stations", system_discovery_is_classic_, system_discovery_is_classic_cnt_);
        ESP_LOGI(TAG, "  ");

        log_device_list("Handsfree Indoor Stations", system_discovery_is_handsfree_, system_discovery_is_handsfree_cnt_);
        ESP_LOGI(TAG, "  ");

        log_device_list("Outdoor Stations", system_discovery_as_, system_discovery_as_cnt_);
        ESP_LOGI(TAG, "  ");

        log_device_list("Controllers (Power Supply)", system_discovery_ctr_, system_discovery_ctr_cnt_);
        ESP_LOGI(TAG, "  ");
        
        log_device_list("Functional Extensions", system_discovery_ext_, system_discovery_ext_cnt_);
        ESP_LOGI(TAG, "  ");

        log_device_list("Access Control", system_discovery_acc_, system_discovery_acc_cnt_);

        #ifdef USE_SYSTEM_DISCOVERY_COMPLETE_CALLBACK
        this->system_discovery_complete_callback_.call(device_cnt);
        #endif
    }

    void TCBusComponent::log_device_list(const char* name, const uint32_t* list, uint8_t count)
    {
        ESP_LOGI(TAG, "  %s: %d", name, count);

        if(count > 0)
        {
            for (uint8_t i = 0; i < count; i++)
            {
                ESP_LOGI(TAG, "    %i", list[i]);
            }
        }
        else
        {
            ESP_LOGI(TAG, "  No devices found");
        }
    }
}