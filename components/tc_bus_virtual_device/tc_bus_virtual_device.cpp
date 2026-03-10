#include "tc_bus_virtual_device.h"
#include "util.h"

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

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"

using namespace esphome;

namespace esphome::tc_bus
{
    void TCBusVirtualDeviceComponent::setup()
    {
        ESP_LOGCONFIG(TAG, "Running setup");

        if(this->tc_bus_->is_failed()) {
            this->mark_failed(LOG_STR("TC:BUS failed to setup!"));
            return;
        }
    
        if(!this->tc_bus_->is_ready()) {
            this->mark_failed(LOG_STR("TC:BUS is not setup yet!"));
            return;
        }

        uint32_t hash = fnv1_hash("tc_bus_virtual_device_" + this->internal_id_);
        this->pref_ = global_preferences->make_preference<TCBusVirtualDeviceSettings>(hash, true);

        TCBusVirtualDeviceSettings recovered{};
        if (!this->pref_.load(&recovered))
        {
            uint8_t mac[6];
            get_mac_address_raw(mac);
            uint32_t mac_addr = (mac[3] << 16) | (mac[4] << 8) | (mac[5] & 0xF0) >> 4;

            recovered.serial_number = mac_addr;
        }

        this->call_state_ = CallState::IDLE;

        this->set_serial_number(recovered.serial_number, false);
        this->set_address(recovered.address, false);

        this->address_divider_ = recovered.address_divider;
        this->door_readiness_duration_ = recovered.door_readiness_duration;
        this->call_time_duration_ = recovered.call_time_duration;
        this->door_opener_duration_ = recovered.door_opener_duration;
        this->force_long_door_opener_protocol_ = recovered.force_long_door_opener_protocol;
        this->auto_answer_call_ = recovered.auto_answer_call;

        if(this->device_group_ == VIRTUAL_DEVICE_GROUP_INDOOR_STATION)
        {
            // Address 0x00
            memory_[0x00] = 0x20; memory_[0x01] = 0x00; memory_[0x02] = 0x00; memory_[0x03] = 0x10;
            // Address 0x04
            memory_[0x04] = 0x00; memory_[0x05] = 0x00; memory_[0x06] = 0x1F; memory_[0x07] = 0x42;
            // Address 0x08
            memory_[0x08] = 0x40; memory_[0x09] = 0x1F; memory_[0x0A] = 0x42; memory_[0x0B] = 0x40;
            // Address 0x0C
            memory_[0x0C] = 0x10; memory_[0x0D] = 0x00; memory_[0x0E] = 0x00; memory_[0x0F] = 0x00;
            // Address 0x10
            memory_[0x10] = 0x00; memory_[0x11] = 0x00; memory_[0x12] = 0x00; memory_[0x13] = 0x00;
            // Address 0x14
            memory_[0x14] = 0x00; memory_[0x15] = 0x01; memory_[0x16] = 0x00; memory_[0x17] = 0x49;
            // Address 0x18
            memory_[0x18] = 0x00; memory_[0x19] = 0x05; memory_[0x1A] = 0x00; memory_[0x1B] = 0x05;
            // Address 0x1C
            memory_[0x1C] = 0xFF; memory_[0x1D] = 0xFF; memory_[0x1E] = 0xFF; memory_[0x1F] = 0xFF;
        }
        else
        {
            // TODO: Fill memory with default values for outdoor station
        }
        

        #ifdef USE_SWITCH
        if (this->force_long_door_opener_protocol_switch_ != nullptr)
        {
            this->force_long_door_opener_protocol_switch_->publish_state(this->force_long_door_opener_protocol_);
        }
        if (this->auto_answer_call_switch_ != nullptr)
        {
            this->auto_answer_call_switch_->publish_state(this->auto_answer_call_);
        }
        #endif

        #ifdef USE_NUMBER
        if (this->address_divider_number_ != nullptr)
        {
            this->address_divider_number_->publish_state(this->address_divider_);
        }
        if (this->door_readiness_duration_number_ != nullptr)
        {
            this->door_readiness_duration_number_->publish_state(this->door_readiness_duration_);
        }
        if (this->call_time_duration_number_ != nullptr)
        {
            this->call_time_duration_number_->publish_state(this->call_time_duration_);
        }
        if (this->door_opener_duration_number_ != nullptr)
        {
            this->door_opener_duration_number_->publish_state(this->door_opener_duration_);
        }
        #endif

        // Register remote listener
        this->tc_bus_->register_remote_listener(this, 10);

        this->high_freq_.start();
    }

    void TCBusVirtualDeviceComponent::save_preferences()
    {
        TCBusVirtualDeviceSettings settings{};
        settings.serial_number = this->serial_number_;
        settings.address = this->address_;
        settings.address_divider = this->address_divider_;
        settings.door_readiness_duration = this->door_readiness_duration_;
        settings.call_time_duration = this->call_time_duration_;
        settings.door_opener_duration = this->door_opener_duration_;

        settings.force_long_door_opener_protocol = this->force_long_door_opener_protocol_;
        settings.auto_answer_call = this->auto_answer_call_;

        if (!this->pref_.save(&settings))
        {
            ESP_LOGW(TAG, "Failed to save settings to flash memory.");
        }
    }

    void TCBusVirtualDeviceComponent::dump_config()
    {
        ESP_LOGCONFIG(TAG, "TC:BUS Virtual Device:");
        ESP_LOGCONFIG(TAG, "  Group: %s", virtual_device_group_to_string(this->device_group_));
        ESP_LOGCONFIG(TAG, "  Model: %s", this->device_group_ == VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION ? "PUK" : "ISW6010 + Smart Stick");
        ESP_LOGCONFIG(TAG, "  Serial Number: %i", this->serial_number_);
        if(this->device_group_ == VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
        }

        #ifdef USE_SWITCH
        ESP_LOGCONFIG(TAG, "  Switches:");
        LOG_SWITCH("    ", "Force long door opener protocol", this->force_long_door_opener_protocol_switch_);
        LOG_SWITCH("    ", "Auto answer call", this->auto_answer_call_switch_);
        #endif

        #ifdef USE_NUMBER
        ESP_LOGCONFIG(TAG, "  Number Inputs:");
        LOG_NUMBER("    ", "Serial Number", this->serial_number_number_);
        LOG_NUMBER("    ", "Address", this->address_number_);
        LOG_NUMBER("    ", "Address Divider", this->address_divider_number_);
        LOG_NUMBER("    ", "Door Readiness Duration", this->door_readiness_duration_number_);
        LOG_NUMBER("    ", "Call Time Duration", this->call_time_duration_number_);
        LOG_NUMBER("    ", "Door Opener Duration", this->door_opener_duration_number_);
        #endif
    }

    void TCBusVirtualDeviceComponent::loop()
    {
        
    }

    void TCBusVirtualDeviceComponent::set_serial_number(uint32_t serial_number, bool save) {

        if(serial_number > 0xFFFFF)
        {
            ESP_LOGW(TAG, "Invalid Serial Number, reset to device MAC.");

            uint8_t mac[6];
            get_mac_address_raw(mac);
            uint32_t mac_addr = (mac[3] << 16) | (mac[4] << 8) | (mac[5] & 0xF0) >> 4;

            serial_number = mac_addr;
        }

        this->serial_number_ = serial_number;

        // Save to flash
        if(save)
        {
            this->save_preferences();
        }

        // Update Entities
        #ifdef USE_NUMBER
        if (this->serial_number_number_ != nullptr)
        {
            this->serial_number_number_->publish_state(serial_number);
        }
        #endif
    }

    void TCBusVirtualDeviceComponent::set_address(uint8_t address, bool save) {

        if(address > 0xFF)
        {
            ESP_LOGW(TAG, "Invalid Address, reset to 0.");
            address = 0;
        }

        this->address_ = address;

        // Save to flash
        if(save)
        {
            this->save_preferences();
        }

        // Update Entities
        #ifdef USE_NUMBER
        if (this->address_number_ != nullptr)
        {
            this->address_number_->publish_state(address);
        }
        #endif
    }

    bool TCBusVirtualDeviceComponent::on_receive(tc_bus::TelegramData telegram_data, bool received)
    {
        if (!received || telegram_data.is_retransmission)
        {
            return false;
        }

        // General
        if (telegram_data.type == TELEGRAM_TYPE_SEARCH_DEVICES)
        {
            if(this->tc_bus_->selected_device_group_ == this->device_group_)
            {
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_FOUND_DEVICE, 0, 0, this->serial_number_);
            }
        }
        else if (telegram_data.type == TELEGRAM_TYPE_SELECT_DEVICE_GROUP)
        {
            if(telegram_data.payload == this->device_group_)
            {
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1); // workaround - remove later - or not
            }
            
            if(this->memory_mode_)
            {
                this->memory_mode_ = false;
                ESP_LOGW(TAG, "MEMORY MODE FALSE");
            }
        }
        else if(telegram_data.type == TELEGRAM_TYPE_SELECT_MEMORY_PAGE && telegram_data.serial_number == this->serial_number_)
        {
            this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
            this->memory_mode_ = true;
            return true;
        }
        else if(telegram_data.type == TELEGRAM_TYPE_STOP_TALKING || telegram_data.type == TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL)
        {
            bool cancelled = this->reset_call();
            if(cancelled)
            {
                this->call_ended_callback_.call(telegram_data);
            }
            return true;
        }
        else if(telegram_data.type == TELEGRAM_TYPE_READ_MEMORY_BLOCK && this->memory_mode_)
        {
            uint32_t block = this->memory_[telegram_data.address] << 24 | this->memory_[telegram_data.address + 1] << 16 | this->memory_[telegram_data.address + 2] << 8 | this->memory_[telegram_data.address + 3];
            this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, block);
            return true;
        }
        else if(telegram_data.type == TELEGRAM_TYPE_WRITE_MEMORY && this->memory_mode_)
        {
            this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);

            memory_[telegram_data.address/4] = (telegram_data.payload >> 8) & 0xFF;
            memory_[(telegram_data.address/4) + 1] = telegram_data.payload & 0xFF;

            return true;
        }
        
        // Device specific
        if(this->device_group_ == VIRTUAL_DEVICE_GROUP_INDOOR_STATION)
        {
            if(telegram_data.type == TELEGRAM_TYPE_DOOR_CALL && telegram_data.serial_number == this->serial_number_)
            {
                // 1. receive door call from outdoor station
                // 2. send ACK STATUS

                // 3. send start talking to outdoor station with answer_call()
                // 4. receive acknowledge from outdoor station to initiate call

                // Door call from outdoor station
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                this->call_internal_ = false;
                this->call_address_ = telegram_data.address;
                this->call_state_ = CallState::IN_RINGING;
                this->door_readiness_active_ = true;
                this->incoming_call_callback_.call(telegram_data);
            }
            else if(telegram_data.type == TELEGRAM_TYPE_INTERNAL_CALL && telegram_data.serial_number == this->serial_number_)
            {
                // 1. receive internal call from indoor station (address 63)
                // 2. send ACK STATUS

                // 3. send start talking to indoor station with answer_call()
                // 4. receive acknowledge from outdoor station to initiate call

                // Internal call from another indoor station
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                this->call_internal_ = true;
                this->call_address_ = telegram_data.address;
                this->call_state_ = CallState::IN_RINGING;
                this->incoming_call_callback_.call(telegram_data);
            }
            else if (telegram_data.type == TELEGRAM_TYPE_END_OF_DOOR_READINESS)
            {
                this->door_readiness_active_ = false;
            }
            else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
            {
                this->call_state_ = CallState::LINE_BUSY;
            }
            else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING)
            {
                // Outgoing internal call accepted by other indoor station
                if(this->call_state_ == CallState::OUT_RINGING && this->call_address_ == telegram_data.serial_number)
                {
                    // out: acknowledge talk
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 3);
                    this->call_state_ = CallState::CONNECTED;
                    this->call_started_callback_.call(telegram_data);

                    // reset ack timeouts
                    this->cancel_timeout("wait_for_call_ack");
                    this->cancel_timeout("wait_for_talking_ack");

                    ESP_LOGI(TAG, "Connected to indoor station");

                    if(this->call_time_duration_ != 0)
                    {
                        this->set_timeout("call_time_limit", (this->call_time_duration_ * 8 * 1000), [this]() {
                            bool cancelled = this->reset_call();
                            ESP_LOGE(TAG, "CALL TIME LIMIT REACHED");

                            TelegramData out_telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING, 63);
                            this->call_ended_callback_.call(out_telegram_data);
                        });
                    }
                }
                else
                {
                    this->call_state_ = CallState::LINE_BUSY;
                }
            }
            else if (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS)
            {
                // Other device accepted
                if(this->call_state_ == CallState::OUT_CHECK_DST)
                {
                    this->call_state_ = CallState::OUT_RINGING;

                    // reset call ack timeout
                    this->cancel_timeout("wait_for_call_ack");

                    ESP_LOGD(TAG, "Indoor Station acknowledged, waiting for start talking");

                    // wait for start talking telegram
                    this->set_timeout("wait_for_talking_ack", CALL_TIMEOUT_MS, [this]() {
                        this->reset_call();
                        ESP_LOGE(TAG, "START TALKING TIMEOUT - No response from indoor station");
                        this->call_failed_callback_.call();
                    });
                }
                else if(this->call_state_ == CallState::IN_WAIT_FOR_INIT)
                {
                    this->call_state_ = CallState::CONNECTED;
                    this->call_started_callback_.call(telegram_data);

                    // reset ack timeouts
                    this->cancel_timeout("wait_for_call_ack");
                    this->cancel_timeout("wait_for_talking_ack");
                }
            }
            else if (telegram_data.type == TELEGRAM_TYPE_RESET)
            {
                this->memory_mode_ = false;
                this->reset_call();
            }
            else if (telegram_data.type == TELEGRAM_TYPE_REQUEST_VERSION && telegram_data.serial_number == this->serial_number_)
            {
                // TASTA PRO AUDIO
                // Version 2.9.12
                // + Smart Stick
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, 0x007ED29C);
            }
        }
        else if(this->device_group_ == VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION)
        {
            // 1. call() - outgoing door call from this outdoor station to indoor station
            // 2. receive ACK STATUS from indoor station

            // 3. receive start talking from indoor station (address is this->address)
            // 4. send acknowledge to indoor station to initiate call


            // in: start talking from indoor station sn to this address
            if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
            {
                ESP_LOGW(TAG, "start talking door call from %i to address %i", telegram_data.serial_number, telegram_data.address);

                // Outgoing door call accepted by indoor station
                // or call initiated by indoor station directly to outdoor station address
                if((this->call_state_ == CallState::IDLE || this->call_state_ == CallState::OUT_RINGING) && telegram_data.address == this->address_)
                {
                    // out: acknowledge talk
                    this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 3);
                    this->call_state_ = CallState::CONNECTED;
                    this->call_started_callback_.call(telegram_data);

                    // reset ack timeouts
                    this->cancel_timeout("wait_for_call_ack");
                    this->cancel_timeout("wait_for_talking_ack");

                    // todo: call time limit / unlimited
                    if(this->call_time_duration_ != 0)
                    {
                        this->set_timeout("call_time_limit", (this->call_time_duration_ * 8 * 1000), [this]() {
                            this->reset_call();
                            ESP_LOGE(TAG, "CALL TIME LIMIT REACHED");

                            TelegramData out_telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, this->address_);
                            this->call_ended_callback_.call(out_telegram_data);
                        });
                    }
                }
                else
                {
                    this->call_state_ = CallState::LINE_BUSY;
                }
            }
            else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING)
            {
                this->call_state_ = CallState::LINE_BUSY;
            }
            else if (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS)
            {
                // Other device accepted
                if(this->call_state_ == CallState::OUT_CHECK_DST)
                {
                    this->call_state_ = CallState::OUT_RINGING;

                    // reset call ack timeout
                    this->cancel_timeout("wait_for_call_ack");

                    ESP_LOGD(TAG, "Indoor Station acknowledged, waiting for start talking");

                    // wait for start talking telegram
                    this->set_timeout("wait_for_talking_ack", CALL_TIMEOUT_MS, [this]() {
                        this->reset_call();
                        ESP_LOGE(TAG, "START TALKING TIMEOUT - No response from indoor station");
                        this->call_failed_callback_.call();
                    });

                    this->door_readiness_active_ = true;

                    // Door readiness timeout
                    this->set_timeout("door_readiness_timeout", (this->door_readiness_duration_ * 8 * 1000), [this]() {
                        this->tc_bus_->send_telegram(TELEGRAM_TYPE_END_OF_DOOR_READINESS, this->address_);
                        this->door_readiness_active_ = false;
                        ESP_LOGE(TAG, "DOOR READINESS TIMEOUT");
                    });
                }
            }
            else if (telegram_data.type == TELEGRAM_TYPE_RESET)
            {
                this->memory_mode_ = false;
                this->reset_call();
                this->door_readiness_active_ = false;
                this->cancel_timeout("door_readiness_timeout");
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_INITIALIZE_DOOR_STATION, this->address_);
            }
            else if (telegram_data.type == TELEGRAM_TYPE_REQUEST_VERSION && telegram_data.serial_number == this->serial_number_)
            {
                // PUK
                // Version 2.9.12
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, 0x0420D29C);
            }
        }

        return true;
    }

    bool TCBusVirtualDeviceComponent::reset_call()
    {
        bool was_active = (this->call_state_ != CallState::IDLE && this->call_state_ != CallState::LINE_BUSY);
        
        this->call_internal_ = false;
        this->call_address_ = 0;
        this->call_state_ = CallState::IDLE;

        if(was_active)
        {
            this->cancel_timeout("call_time_limit");
            this->cancel_timeout("wait_for_call_ack");
            this->cancel_timeout("wait_for_talking_ack");
            return true;
        }

        return false;
    }

    void TCBusVirtualDeviceComponent::call(uint32_t destination, bool internal)
    {
        if(this->call_state_ != CallState::IDLE)
        {
            ESP_LOGE(TAG, "Line is busy");
            return;
        }

        this->call_address_ = destination;

        if(this->device_group_ == VIRTUAL_DEVICE_GROUP_INDOOR_STATION)
        {
            if(internal)
            {
                // Call indoor station serial number from address 63 (static)
                this->call_internal_ = true;
                this->call_state_ = CallState::OUT_CHECK_DST;
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_INTERNAL_CALL, 63, 0, destination);

                // ack timeout
                this->set_timeout("wait_for_call_ack", 20, [this]() {
                    this->reset_call();
                    ESP_LOGE(TAG, "ACK TIMEOUT - No response from indoor station");
                    this->call_failed_callback_.call();
                });
            }
            else
            {
                // Talk directly to outdoor station address (destination) from this serial number
                this->call_internal_ = false;
                this->call_state_ = CallState::OUT_CHECK_DST;
                this->tc_bus_->send_telegram(TELEGRAM_TYPE_START_TALKING_DOOR_CALL, destination, this->door_readiness_active_ ? 1 : 0, this->serial_number_);
                
                // ack timeout
                this->set_timeout("wait_for_talking_ack", 20, [this]() {
                    this->reset_call();
                    ESP_LOGE(TAG, "ACK TIMEOUT - No response from outdoor station");
                    this->call_failed_callback_.call();
                });
            }
        }
        else if(this->device_group_ == VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION)
        {
            this->door_readiness_active_ = false;
            this->cancel_timeout("door_readiness_timeout");

            // Call indoor station serial number from this outdoor station
            this->call_internal_ = false;
            this->call_state_ = CallState::OUT_CHECK_DST;

            // send door call to indoor station from this outdoor station
            // destination serial number of indoor station
            // source address of this outdoor station
            this->tc_bus_->send_telegram(TELEGRAM_TYPE_DOOR_CALL, this->address_, 0, destination);
            
            // ack timeout
            this->set_timeout("wait_for_call_ack", 20, [this]() {
                this->reset_call();
                ESP_LOGE(TAG, "ACK TIMEOUT - No response from indoor station");
                this->call_failed_callback_.call();
            });
        }
    }

    void TCBusVirtualDeviceComponent::answer_call()
    {
        if(this->call_state_ != CallState::IN_RINGING)
        {
            ESP_LOGE(TAG, "No call to answer!");
            return;
        }

        this->tc_bus_->send_telegram(this->call_internal_ ? TELEGRAM_TYPE_START_TALKING : TELEGRAM_TYPE_START_TALKING_DOOR_CALL, this->call_address_, 0, this->serial_number_);
        this->call_state_ = CallState::IN_WAIT_FOR_INIT;
        
        // ack timeout
        this->set_timeout("wait_for_talking_ack", 20, [this]() {
            this->reset_call();
            ESP_LOGE(TAG, "ACK TIMEOUT - No response from outdoor station");
            this->call_failed_callback_.call();
        });
    }

    void TCBusVirtualDeviceComponent::end_call()
    {
        if(this->call_state_ != CallState::CONNECTED && this->call_state_ != CallState::OUT_CHECK_DST && this->call_state_ != CallState::OUT_RINGING)
        {
            ESP_LOGE(TAG, "No call to end!");
            return;
        }

        TelegramData telegram_data;

        if(this->device_group_ == VIRTUAL_DEVICE_GROUP_INDOOR_STATION)
        {
            if(this->call_internal_)
            {
                telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING, 63);
            }
            else
            {
                telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, (uint8_t)this->call_address_);
            }
        }
        else if(this->device_group_ == VIRTUAL_DEVICE_GROUP_OUTDOOR_STATION)
        {
            telegram_data = this->tc_bus_->send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, this->address_);
        }

        this->reset_call();

        this->call_ended_callback_.call(telegram_data);
    }

    TelegramData TCBusVirtualDeviceComponent::send_telegram(TelegramType type, uint8_t address, uint32_t payload, uint32_t wait_duration)
    {
        return this->tc_bus_->send_telegram(type, address, payload, this->serial_number_, wait_duration);
    }
}