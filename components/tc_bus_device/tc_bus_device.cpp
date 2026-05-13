#include "tc_bus_device.h"
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
    void TCBusDeviceComponent::setup()
    {
        ESP_LOGCONFIG(TAG, "Running setup");

        if (this->tc_bus_->is_failed())
        {
            this->mark_failed(LOG_STR("TC:BUS failed to setup!"));
            return;
        }
    
        if (!this->tc_bus_->is_ready())
        {
            this->mark_failed(LOG_STR("TC:BUS is not setup yet!"));
            return;
        }

        uint32_t hash = fnv1_hash("tc_bus_device_" + this->internal_id_);
        this->pref_ = global_preferences->make_preference<TCBusDeviceSettings>(hash, true);

        // Register remote listener
        // Before loading settings because we need listener_id_
        if (this->virtual_)
        {
            // Higher priority for virtual devices to ensure they receive telegrams in time
            this->tc_bus_->register_remote_listener(this, 10);
        }
        else
        {
            this->tc_bus_->register_remote_listener(this);
        }

        // Restore settings
        TCBusDeviceSettings recovered{};
        if (!this->pref_.load(&recovered))
        {
            // Generate serial number for virtual device based on mac address + listener id
            if (this->virtual_)
            {
                uint8_t mac[6];
                get_mac_address_raw(mac);
                uint32_t mac_addr = ((mac[3] & 0x0F) << 16) | (mac[4] << 8) | mac[5];
                recovered.serial_number = mac_addr + this->listener_id_;
            }
        }

        this->call_state_ = CallState::IDLE;

        this->set_serial_number(recovered.serial_number, false);

        this->parallel_serial_number_ = recovered.parallel_serial_number;
        this->address_ = recovered.address;
        this->address_divider_ = recovered.address_divider;
        this->door_readiness_duration_ = recovered.door_readiness_duration;
        this->call_time_duration_ = recovered.call_time_duration;
        this->door_opener_duration_ = recovered.door_opener_duration;
        this->use_long_door_opener_protocol_ = recovered.use_long_door_opener_protocol;
        this->auto_answer_call_ = recovered.auto_answer_call;
        this->call_time_unlimited_ = recovered.call_time_unlimited;
        this->calling_requires_door_readiness_ = recovered.calling_requires_door_readiness;
        this->door_opener_requires_door_readiness_ = recovered.door_opener_requires_door_readiness;
        this->door_opener_requires_active_call_ = recovered.door_opener_requires_active_call;

        // Needs settings above
        this->set_model(recovered.model, false);

        // Allocation failed
        if(this->model_data_.memory_size > 0 && this->memory_buffer_size_ == 0)
        {
            this->mark_failed();
            return;
        }

        #ifdef USE_BINARY_SENSOR
        // Reset Binary Sensor Listeners
        for (auto &listener : listeners_)
        {
            listener->turn_off(&listener->timer_);
        }
        #endif

        // Schedule flows for physical devices only
        if (this->virtual_ == false && this->auto_configuration_ && this->serial_number_ != 0)
        {
            if (this->model_ == MODEL_NONE)
            {
                ESP_LOGD(TAG, "Schedule flow: Model identification (setup)");
                identify_device();
            }
        }

        if (this->virtual_)
        {
            this->high_freq_.start();
        }
    }

    void TCBusDeviceComponent::set_serial_number(uint32_t serial_number, bool save)
    {
        if (serial_number > 1000000)
        {
            if (this->virtual_)
            {
                ESP_LOGW(TAG, "Invalid Serial Number, reset to device MAC + Listener ID.");

                uint8_t mac[6];
                get_mac_address_raw(mac);
                uint32_t mac_addr = ((mac[3] & 0x0F) << 16) | (mac[4] << 8) | mac[5];
                serial_number = mac_addr + this->listener_id_;
            }
            else
            {
                ESP_LOGW(TAG, "Invalid Serial Number, reset to 0.");
                serial_number = 0;
            }
        }

        bool changed = serial_number != this->serial_number_;

        this->serial_number_ = serial_number;

        // Save to flash
        if (save)
        {
            this->save_preferences();
        }

        // Only for physical devices
        if (this->virtual_ == false && serial_number != 0)
        {
            // Schedule model identification flow
            if (this->auto_configuration_ && save && changed)
            {
                // Clear memory because device serial number changed
                clear_memory_buffer();

                ESP_LOGD(TAG, "Schedule flow: Model identification (changed serial number from 0)");

                // Wait for possible doorbell ringtone (setup mode)
                this->set_timeout(3000, [this]()
                {
                    identify_device();
                });
            }
        }

        // Update Entities
        #ifdef USE_NUMBER
        if (this->serial_number_number_ != nullptr)
        {
            this->serial_number_number_->publish_state(serial_number);
        }
        #endif
    }

    void TCBusDeviceComponent::set_model(Model model, bool save)
    {
        bool changed = model != this->model_;
        bool changed_from_none = this->model_ == MODEL_NONE && changed;

        this->model_ = model;
        this->model_data_ = getModelData(model);

        // Save to flash
        if (save)
        {
            this->save_preferences();
        }

        // Update Entities
        #ifdef USE_SELECT
        if (this->model_select_ != nullptr)
        {
            this->model_select_->publish_state(model_to_string(model));
        }
        #endif

        // Memory buffer allocation / deallocation
        if(reallocate_memory_buffer())
        {
            // No further actions needed for model without memory support
            if(this->model_data_.memory_size == 0)
            {
                ESP_LOGV(TAG, "set_model() skipping further actions - no memory support");
                return;
            }

            if(this->virtual_)
            {
                ESP_LOGD(TAG, "Rebuild Memory Buffer");
                this->memory_buffer_ready_ = true;

                if(this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
                {
                    update_setting(SETTING_PARALLEL_SERIAL_NUMBER, this->parallel_serial_number_);
                    update_setting(SETTING_RINGTONE_MUTE, 0);
                    update_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL, 0);
                    update_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL, 0);
                    update_setting(SETTING_RINGTONE_INTERNAL_CALL, 0);
                    update_setting(SETTING_RINGTONE_FLOOR_CALL, 0);
                    update_setting(SETTING_VOLUME_RINGTONE, 0);
                    update_setting(SETTING_VOLUME_HANDSET_DOOR_CALL, 0);
                    update_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL, 0);
                    update_setting(SETTING_AS_ADDRESS_DIVIDER, this->address_divider_);
                    update_setting(SETTING_VAS_ADDRESS_DIVIDER, 32);
                    update_setting(SETTING_AUTO_ANSWER_CALL, this->auto_answer_call_);
                    update_setting(SETTING_CALL_TIME_UNLIMITED, this->call_time_unlimited_);
                    update_setting(SETTING_AMBIENT_LIGHT_IN_STANDBY, 0);
                    update_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL, this->use_long_door_opener_protocol_);
                }
                else if(this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
                {
                    update_setting(SETTING_ADDRESS, this->address_);
                    update_setting(SETTING_ADDRESS_LOCK, 1);
                    update_setting(SETTING_CALLING_REQUIRES_DOOR_READINESS, 0);
                    update_setting(SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS, 0);
                    update_setting(SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL, 0);
                    update_setting(SETTING_DOOR_OPENER_DURATION, 4);
                    update_setting(SETTING_CALL_TIME_DURATION, 7);
                    update_setting(SETTING_DOOR_READINESS_DURATION, 7);

                    // Fill side memory buffer with default values (0xFF)
                    for (uint8_t side = 1; side <= this->model_data_.sides; side++)
                    {
                        size_t offset = get_page_offset(side);
                        memset(&this->memory_buffer_[offset], 0xFF, this->model_data_.memory_size_side);
                    }
                }

                ESP_LOGD(TAG, "Virtual Memory Buffer ready");

                // Publish settings after new buffer is built
                this->publish_settings();
            }
            else
            {
                // When model of physical device was changed
                this->publish_settings();

                // Automatic configuration
                if(this->auto_configuration_)
                {
                    if(save)
                    {
                        // On regular change

                        // Model changed from none to something else / memory buffer not read from device yet
                        if(changed_from_none || !this->memory_buffer_ready_)
                        {
                            ESP_LOGD(TAG, "Schedule flow: Memory reading (%s)", changed_from_none ? "changed model - from none" : "changed model - buffer empty");
                            read_memory();
                        }
                    }
                    else
                    {
                        // On setup
                        ESP_LOGD(TAG, "Schedule flow: Memory reading (setup)");
                        read_memory();
                    }
                }
            }
        }
        else
        {
            ESP_LOGE(TAG, "Failed to reallocate memory buffer!");
        }
    }


    bool TCBusDeviceComponent::clear_memory_buffer()
    {
        ESP_LOGD(TAG, "Clear memory buffer");
        if(this->memory_buffer_ != nullptr)
        {
            memset(this->memory_buffer_, 0, this->memory_buffer_size_);
        }
        this->memory_buffer_ready_ = false;

        return true;
    }

    bool TCBusDeviceComponent::reallocate_memory_buffer()
    {
        uint16_t new_buffer_size = this->model_data_.memory_size + (this->model_data_.sides * this->model_data_.memory_size_side);

        ESP_LOGD(TAG, "Allocating memory buffer (%i bytes)", new_buffer_size);

        // No buffer needed
        if(new_buffer_size == 0)
        {
            // Deallocate if allocated
            if (this->memory_buffer_ != nullptr)
            {
                ESP_LOGD(TAG, "Deallocating memory buffer");

                RAMAllocator<uint8_t> allocator;
                allocator.deallocate(this->memory_buffer_, this->memory_buffer_size_);
                this->memory_buffer_ = nullptr;
            }

            ESP_LOGD(TAG, "Reset memory buffer");
            this->memory_buffer_size_ = 0;
            this->memory_buffer_ready_ = false;

            return true;
        }
        else
        {
            // Initial allocation
            if (this->memory_buffer_ == nullptr)
            {
                RAMAllocator<uint8_t> allocator;
                this->memory_buffer_ = allocator.allocate(new_buffer_size);
                if (this->memory_buffer_ == nullptr)
                {
                    ESP_LOGE(TAG, "Cannot allocate memory buffer!");
                    return false;
                }
                else
                {
                    this->memory_buffer_size_ = new_buffer_size;
                    ESP_LOGD(TAG, "Allocated memory buffer (%i bytes)", new_buffer_size);
                }

                // Clear buffer
                clear_memory_buffer();
            }
            else
            {
                // Skip if same size
                if (new_buffer_size == this->memory_buffer_size_)
                {
                    // Skip allocation and clearing
                    ESP_LOGD(TAG, "No reallocation needed, same size.");
                }
                else
                {
                    // Reallocate
                    RAMAllocator<uint8_t> allocator;
                    uint8_t *new_buffer = allocator.reallocate(this->memory_buffer_, new_buffer_size);
                    if (new_buffer == nullptr)
                    {
                        ESP_LOGE(TAG, "Cannot reallocate memory buffer! Leaving as is.");
                        return false;
                    }
                    else
                    {
                        this->memory_buffer_size_ = new_buffer_size;
                        this->memory_buffer_ = new_buffer;

                        ESP_LOGD(TAG, "Reallocated memory buffer (%i bytes)", new_buffer_size);
                    }

                    // Clear buffer
                    clear_memory_buffer();
                }
            }
        }

        return true;
    }

    void TCBusDeviceComponent::save_preferences()
    {
        TCBusDeviceSettings settings{};
        settings.serial_number = this->serial_number_;
        settings.parallel_serial_number = this->parallel_serial_number_;
        settings.address = this->address_;
        settings.model = this->model_;

        settings.address_divider = this->address_divider_;
        settings.door_readiness_duration = this->door_readiness_duration_;
        settings.call_time_duration = this->call_time_duration_;
        settings.door_opener_duration = this->door_opener_duration_;
        settings.use_long_door_opener_protocol = this->use_long_door_opener_protocol_;
        settings.auto_answer_call = this->auto_answer_call_;
        settings.call_time_unlimited = this->call_time_unlimited_;
        settings.calling_requires_door_readiness = this->calling_requires_door_readiness_;
        settings.door_opener_requires_door_readiness = this->door_opener_requires_door_readiness_;
        settings.door_opener_requires_active_call = this->door_opener_requires_active_call_;

        if (!this->pref_.save(&settings))
        {
            ESP_LOGW(TAG, "Failed to save settings to flash memory.");
        }
    }

    void TCBusDeviceComponent::dump_config()
    {
        ESP_LOGCONFIG(TAG, this->virtual_ ? "Virtual TC:BUS Device:" : "TC:BUS Device:");
        ESP_LOGCONFIG(TAG, "  Group: %s", device_group_to_string(this->model_data_.device_group));
        ESP_LOGCONFIG(TAG, "  Model: %s", model_to_string(this->model_));
        ESP_LOGCONFIG(TAG, "  Serial Number: %i", this->serial_number_);

        if (this->virtual_ && this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
            ESP_LOGCONFIG(TAG, "  Door Readiness Duration: %i sec.", this->door_readiness_duration_ * 8);
            ESP_LOGCONFIG(TAG, "  Door Opener Duration: %i sec.", this->door_opener_duration_ * 8);
        }

        if (this->virtual_ && (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE))
        {
            ESP_LOGCONFIG(TAG, "  Address Divider: %i", this->address_divider_);
        }

        if (this->virtual_ && (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE || this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION))
        {
            ESP_LOGCONFIG(TAG, "  Call Time Duration: %i sec.", this->call_time_duration_ * 8);
        }

        if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
        {
            ESP_LOGCONFIG(TAG, "  Parallel Serial Number: %i", this->parallel_serial_number_);
            ESP_LOGCONFIG(TAG, "  Always use long door opener protocol: %s", YESNO(this->use_long_door_opener_protocol_));
        }

        #ifdef USE_BUTTON
        ESP_LOGCONFIG(TAG, "  Buttons:");
        LOG_BUTTON("    ", "Identify", this->identify_device_button_);
        LOG_BUTTON("    ", "Read Memory", this->read_memory_button_);
        #endif

        #ifdef USE_SWITCH
        ESP_LOGCONFIG(TAG, "  Switches:");
        LOG_SWITCH("    ", "Always use long door opener protocol", this->use_long_door_opener_protocol_switch_);
        LOG_SWITCH("    ", "Ringtone Mute", this->ringtone_mute_switch_);
        LOG_SWITCH("    ", "Auto answer call", this->auto_answer_call_switch_);
        LOG_SWITCH("    ", "Call Time Unlimited", this->call_time_unlimited_switch_);
        LOG_SWITCH("    ", "Calling requires door readiness", this->calling_requires_door_readiness_switch_);
        LOG_SWITCH("    ", "Door opener requires door readiness", this->door_opener_requires_door_readiness_switch_);
        LOG_SWITCH("    ", "Door opener requires active call", this->door_opener_requires_active_call_switch_);
        LOG_SWITCH("    ", "Address Lock", this->address_lock_switch_);
        #endif

        #ifdef USE_NUMBER
        ESP_LOGCONFIG(TAG, "  Number Inputs:");
        LOG_NUMBER("    ", "Serial Number", this->serial_number_number_);
        LOG_NUMBER("    ", "Parallel Serial Number", this->parallel_serial_number_number_);
        LOG_NUMBER("    ", "Address", this->address_number_);
        LOG_NUMBER("    ", "Volume Handset Door Call", this->volume_handset_door_call_number_);
        LOG_NUMBER("    ", "Volume Handset Internal Call", this->volume_handset_internal_call_number_);
        LOG_NUMBER("    ", "Volume Ringtone", this->volume_ringtone_number_);
        LOG_NUMBER("    ", "Address Divider", this->address_divider_number_);
        LOG_NUMBER("    ", "Door Readiness Duration", this->door_readiness_duration_number_);
        LOG_NUMBER("    ", "Call Time Duration", this->call_time_duration_number_);
        LOG_NUMBER("    ", "Door Opener Duration", this->door_opener_duration_number_);
        #endif

        #ifdef USE_SELECT
        ESP_LOGCONFIG(TAG, "  Select Inputs:");
        LOG_SELECT("    ", "Model", this->model_select_);
        LOG_SELECT("    ", "Ringtone Entrance Door Call", this->ringtone_entrance_door_call_select_);
        LOG_SELECT("    ", "Ringtone Second Entrance Door Call", this->ringtone_second_entrance_door_call_select_);
        LOG_SELECT("    ", "Ringtone Floor Call", this->ringtone_floor_call_select_);
        LOG_SELECT("    ", "Ringtone Internal Call", this->ringtone_internal_call_select_);
        #endif
    }

    void TCBusDeviceComponent::loop()
    {
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

        if (reading_memory_timeout_ != 0 && now_millis > reading_memory_timeout_)
        {
            reading_memory_timeout_ = 0; // Reset
            ESP_LOGW(TAG, "Memory reading stuck - retry #%i", reading_memory_try_);
            read_selected_memory_block();
        }

        // Process flows for physical devices only
        if (!this->virtual_)
        {
            this->process_flow_queue();
        }
    }

    bool TCBusDeviceComponent::on_receive(const tc_bus::TelegramData& telegram_data, TelegramSource source)
    {
        const bool received = (source == TelegramSource::BUS_RECEIVED || source == TelegramSource::PEER_SENT);

        if (!received || telegram_data.is_retransmission)
        {
            return false;
        }

        if (this->virtual_)
        {
            // Virtual Device

            // General
            if (telegram_data.type == TELEGRAM_TYPE_SELECT_DEVICE_GROUP)
            {
                /*if (telegram_data.payload == this->model_data_.device_group)
                {
                    // workaround - remove later 
                    // Has issues when multiple devices of this group are connected
                    // They acknowledge at the same time which destroys some telegrams
                    send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                }*/
                
                if (this->memory_mode_)
                {
                    this->memory_mode_ = false;
                    ESP_LOGD(TAG, "MEMORY MODE OFF");
                }
            }
            else if (telegram_data.type == TELEGRAM_TYPE_SEARCH_DEVICES)
            {
                if (this->tc_bus_->get_selected_device_group() == this->model_data_.device_group)
                {
                    // workaround - remove later 
                    // Has issues when multiple devices of this group are connected
                    // They acknowledge at the same time which destroys some telegrams
                    //send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);

                    send_telegram(TELEGRAM_TYPE_FOUND_DEVICE);
                }
            }
            else if (telegram_data.type == TELEGRAM_TYPE_REQUEST_VERSION && telegram_data.serial_number == this->serial_number_)
            {
                // HW version 1, FW version 2.9.12
                uint16_t identifier = model_to_identifier(this->model_);
                const uint32_t payload = ((uint32_t)0x1 << 28) | ((uint32_t)identifier << 16) | (0xD << 12) | 0x29C;
                send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, payload);
            }
            else if (telegram_data.type == TELEGRAM_TYPE_SELECT_MEMORY_PAGE && telegram_data.serial_number == this->serial_number_)
            {
                send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);

                this->reading_memory_current_page_ = telegram_data.address;

                this->memory_mode_ = true;
                ESP_LOGD(TAG, "MEMORY MODE ON");
            }
            else if (telegram_data.type == TELEGRAM_TYPE_READ_MEMORY_BLOCK && this->memory_mode_)
            {
                size_t offset = get_page_offset(this->reading_memory_current_page_);
                size_t index = offset + telegram_data.address;
                uint32_t block;
                std::memcpy(&block, this->memory_buffer_ + index, sizeof(uint32_t));
                block = __builtin_bswap32(block);

                send_telegram(TELEGRAM_TYPE_ACK_DATA, 0, block);
                return true;
            }
            else if (telegram_data.type == TELEGRAM_TYPE_WRITE_MEMORY && this->memory_mode_)
            {
                send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);

                size_t offset = get_page_offset(this->reading_memory_current_page_);
                size_t index = offset + telegram_data.address;
                uint16_t payload = __builtin_bswap16(static_cast<uint16_t>(telegram_data.payload));
                std::memcpy(this->memory_buffer_ + index, &payload, sizeof(uint16_t));
                return true;
            }
            else if (telegram_data.type == TELEGRAM_TYPE_STOP_TALKING || telegram_data.type == TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL)
            {
                bool cancelled = this->reset_call();
                if (cancelled)
                {
                    ESP_LOGW(TAG, "Virtual device: Stop talking received, disconnected call");

                    #ifdef USE_CALL_ENDED_CALLBACK
                    this->call_ended_callback_.call(telegram_data);
                    #endif

                    send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                }
                else
                {
                    ESP_LOGW(TAG, "Virtual device: Stop talking received, no action taken");
                }
                return true;
            }

            // Device specific
            if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
            {
                if (telegram_data.type == TELEGRAM_TYPE_DOOR_CALL)
                {
                    if(telegram_data.serial_number == this->serial_number_ || telegram_data.serial_number == this->parallel_serial_number_)
                    {
                        // 1. receive door call from outdoor station
                        // 2. send ACK STATUS

                        // 3. send start talking to outdoor station with answer_call()
                        // 4. receive acknowledge from outdoor station to initiate call

                        // Door call from outdoor station
                        if(telegram_data.serial_number == this->parallel_serial_number_)
                        {
                            this->call_from_parallel_sn_ = true;
                            ESP_LOGW(TAG, "Virtual device: Incoming door call for parallel device");

                            // ack timeout
                            this->set_timeout("wait_for_call_ack", ACK_TIMEOUT_MS, [this]() {
                                this->reset_call();
                                ESP_LOGE(TAG, "Call failed - parallel device did not acknowledge");

                                #ifdef USE_CALL_FAILED_CALLBACK
                                this->call_failed_callback_.call();
                                #endif
                            });
                        }
                        else
                        {
                            // Aknowledge call (device available)
                            send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);
                            
                            this->call_from_parallel_sn_ = false;
                            ESP_LOGW(TAG, "Virtual device: Incoming door call for this device");
                        }

                        this->call_internal_ = false;
                        this->call_address_ = telegram_data.address;
                        this->call_state_ = CallState::IN_RINGING;

                        this->door_readiness_address_ = telegram_data.address;
                        this->door_readiness_active_ = true;

                        #ifdef USE_INCOMING_CALL_CALLBACK
                        this->incoming_call_callback_.call(telegram_data);
                        #endif
                    }
                    else
                    {
                        // Reset door readiness if call was for another device
                        this->door_readiness_address_ = 0;
                        this->door_readiness_active_ = false;
                    }
                }
                else if(telegram_data.type == TELEGRAM_TYPE_CONTROL_FUNCTION && telegram_data.payload == 0xCF)
                {
                    ESP_LOGW(TAG, "Virtual device: Internal call sender serial number received (not used)");
                }
                else if (telegram_data.type == TELEGRAM_TYPE_INTERNAL_CALL && (telegram_data.serial_number == this->serial_number_ || telegram_data.serial_number == this->parallel_serial_number_))
                {
                    // 1. receive internal call from indoor station (address 63)
                    // 2. send ACK STATUS

                    // 3. send start talking to indoor station with answer_call()
                    // 4. receive acknowledge from outdoor station to initiate call

                    // Internal call from another indoor station
                    
                    if(telegram_data.serial_number == this->parallel_serial_number_)
                    {
                        this->call_from_parallel_sn_ = true;
                        ESP_LOGW(TAG, "Virtual device: Incoming internal call for parallel device");

                        // ack timeout
                        this->set_timeout("wait_for_call_ack", ACK_TIMEOUT_MS, [this]() {
                            this->reset_call();
                            ESP_LOGE(TAG, "Call failed - parallel device did not acknowledge");

                            #ifdef USE_CALL_FAILED_CALLBACK
                            this->call_failed_callback_.call();
                            #endif
                        });
                    }
                    else
                    {
                        // Aknowledge call (device available)
                        send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 1);

                        this->call_from_parallel_sn_ = false;
                        ESP_LOGW(TAG, "Virtual device: Incoming internal call for this device");
                    }

                    this->call_internal_ = true;
                    this->call_address_ = telegram_data.address;
                    this->call_state_ = CallState::IN_RINGING;

                    #ifdef USE_INCOMING_CALL_CALLBACK
                    this->incoming_call_callback_.call(telegram_data);
                    #endif
                }
                else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
                {
                    if(this->call_state_ == CallState::IN_RINGING)
                    {
                        if(telegram_data.serial_number == this->serial_number_ || telegram_data.serial_number == this->parallel_serial_number_)
                        {
                            ESP_LOGW(TAG, "Virtual device: Cancel call (Call started on parallel device)");
                        }
                        else
                        {
                            ESP_LOGW(TAG, "Virtual device: Cancel call (Call started on another device)");
                        }

                        this->reset_call();

                        #ifdef USE_CALL_FAILED_CALLBACK
                        this->call_failed_callback_.call();
                        #endif
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Virtual device: Line busy (Call started on another device)");
                    }

                    this->call_state_ = CallState::LINE_BUSY;
                }
                else if (telegram_data.type == TELEGRAM_TYPE_START_TALKING)
                {
                    // Outgoing internal call accepted by other indoor station
                    if (this->call_state_ == CallState::OUT_RINGING && this->call_address_ == telegram_data.serial_number)
                    {
                        uint8_t flags = 0; // 3
                        flags |= (1 << 0); // duplex
                        flags |= (1 << 1); // unknown (is usually set)

                        // out: acknowledge talk
                        send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, flags);
                        this->call_state_ = CallState::CONNECTED;

                        // reset ack timeouts
                        this->cancel_timeout("wait_for_call_ack");
                        this->cancel_timeout("wait_for_talking_ack");

                        ESP_LOGD(TAG, "VIS + IS >> Connected in full duplex mode");

                        #ifdef USE_CALL_STARTED_CALLBACK
                        this->call_started_callback_.call(telegram_data);
                        #endif

                        if (this->call_time_unlimited_ == false)
                        {
                            this->set_timeout("call_time_limit", CALL_TIME_LIMIT_MS, [this]() {
                                this->reset_call();

                                ESP_LOGD(TAG, "Maximum call time reached - disconnecting call");

                                TelegramData out_telegram_data = send_telegram(TELEGRAM_TYPE_STOP_TALKING, 63);

                                #ifdef USE_CALL_ENDED_CALLBACK
                                this->call_ended_callback_.call(out_telegram_data);
                                #endif
                            });
                        }
                    }
                    else
                    {
                        if(this->call_state_ == CallState::IN_RINGING)
                        {
                            if(telegram_data.serial_number == this->serial_number_ || telegram_data.serial_number == this->parallel_serial_number_)
                            {
                                ESP_LOGW(TAG, "Virtual device: Cancel call (Call started on parallel device)");
                            }
                            else
                            {
                                ESP_LOGW(TAG, "Virtual device: Cancel call (Call started on another device)");
                            }

                            this->reset_call();

                            #ifdef USE_CALL_FAILED_CALLBACK
                            this->call_failed_callback_.call();
                            #endif
                        }
                        else
                        {
                            ESP_LOGW(TAG, "Virtual device: Line busy (Call started on another device)");
                        }

                        this->call_state_ = CallState::LINE_BUSY;
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_ACK_STATUS)
                {
                    if (this->call_state_ == CallState::IN_RINGING)
                    {
                        if(this->call_from_parallel_sn_)
                        {
                            this->cancel_timeout("wait_for_call_ack");

                            if(this->call_internal_)
                            {
                                ESP_LOGW(TAG, "Virtual device: Incoming internal call aknowledged by parallel device");
                            }
                            else
                            {
                                ESP_LOGW(TAG, "Virtual device: Incoming door call aknowledged by parallel device");
                            }

                            ESP_LOGW(TAG, "Virtual device: Wait for start talking");

                            // wait for start talking telegram
                            this->set_timeout("wait_for_talking_ack", CALL_TIMEOUT_MS, [this]() {
                                this->reset_call();
                                ESP_LOGE(TAG, "Call timeout - No start talking in time");

                                #ifdef USE_CALL_FAILED_CALLBACK
                                this->call_failed_callback_.call();
                                #endif
                            });
                        }
                    }
                    else if (this->call_state_ == CallState::OUT_CHECK_DST)
                    {
                        // Other device accepted

                        this->call_state_ = CallState::OUT_RINGING;

                        // reset call ack timeout
                        this->cancel_timeout("wait_for_call_ack");

                        bool ringtone_muted = (telegram_data.payload & (1 << 1)) != 0;  // xx1x
                        bool door_opener_auto = (telegram_data.payload & (1 << 2)) != 0;  // 1xxx

                        ESP_LOGD(TAG, "Indoor Station acknowledged (%s), waiting for start talking", ringtone_muted ? "ringtone muted" : "ringing");

                        // wait for start talking telegram
                        this->set_timeout("wait_for_talking_ack", CALL_TIMEOUT_MS, [this]() {
                            this->reset_call();
                            ESP_LOGE(TAG, "Call timeout - No answer from indoor station");

                            #ifdef USE_CALL_FAILED_CALLBACK
                            this->call_failed_callback_.call();
                            #endif
                        });
                    }
                    else if (this->call_state_ == CallState::IN_WAIT_FOR_INIT)
                    {
                        // reset ack timeouts
                        this->cancel_timeout("wait_for_call_ack");
                        this->cancel_timeout("wait_for_talking_ack");

                        this->call_state_ = CallState::CONNECTED;

                        bool full_duplex = telegram_data.raw & (1 << 0);
                        ESP_LOGD(TAG, "VIS + %s >> Connected in %s duplex mode", (this->call_internal_ ? "IS" : "AS"), (full_duplex ? "full" : "half"));

                        #ifdef USE_CALL_STARTED_CALLBACK
                        TelegramData telegram_data_cb;
                        if (this->call_internal_)
                        {
                            telegram_data_cb = buildTelegram(TELEGRAM_TYPE_START_TALKING, this->call_address_, 0, this->call_from_parallel_sn_ ? this->parallel_serial_number_ : this->serial_number_);
                        }
                        else
                        {
                            telegram_data_cb = buildTelegram(TELEGRAM_TYPE_START_TALKING_DOOR_CALL, this->call_address_, this->door_readiness_active_ ? 1 : 0, this->call_from_parallel_sn_ ? this->parallel_serial_number_ : this->serial_number_);
                        }

                        this->call_started_callback_.call(telegram_data_cb);
                        #endif
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_END_OF_DOOR_READINESS)
                {
                    this->door_readiness_address_ = 0;
                    this->door_readiness_active_ = false;
                }
                else if (telegram_data.type == TELEGRAM_TYPE_RESET || (telegram_data.type == TELEGRAM_TYPE_CONTROL_FUNCTION && telegram_data.payload == 0xD7 && telegram_data.serial_number == this->serial_number_))
                {
                    if (this->tc_bus_->get_selected_device_group() == this->model_data_.device_group || telegram_data.serial_number == this->serial_number_)
                    {
                        this->reset_call();
                        this->door_readiness_address_ = 0;
                        this->door_readiness_active_ = false;
                        this->memory_mode_ = false;
                    }
                }
            }
            else if (this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
            {
                // 1. call() - outgoing door call from this outdoor station to indoor station
                // 2. receive ACK STATUS from indoor station

                // 3. receive start talking from indoor station (address is this->address)
                // 4. send acknowledge to indoor station to initiate call


                // in: start talking from indoor station sn to this address
                if (telegram_data.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
                {
                    // Outgoing door call accepted by indoor station
                    // or call initiated by indoor station directly to outdoor station address
                    if ((this->call_state_ == CallState::IDLE || this->call_state_ == CallState::OUT_RINGING) && telegram_data.address == this->address_)
                    {
                        // indoor station calls directly to outdoor station without door readiness
                        if (this->calling_requires_door_readiness_ && this->door_readiness_active_ == false)
                        {
                            ESP_LOGE(TAG, "Door readiness is not active - rejecting call from indoor station");
                            this->reset_call();
                        }
                        else
                        {
                            uint8_t flags = 0; // 3
                            flags |= (1 << 0); // duplex
                            flags |= (1 << 1); // unknown (is usually set)

                            // out: acknowledge talk
                            send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, flags);
                            this->call_state_ = CallState::CONNECTED;

                            ESP_LOGD(TAG, "VAS + IS >> Connected in full duplex mode");

                            // reset ack timeouts
                            this->cancel_timeout("wait_for_call_ack");
                            this->cancel_timeout("wait_for_talking_ack");

                            #ifdef USE_CALL_STARTED_CALLBACK
                            this->call_started_callback_.call(telegram_data);
                            #endif

                            // call time limit
                            if (this->call_time_duration_ != 0)
                            {
                                this->set_timeout("call_time_limit", (this->call_time_duration_ * 8 * 1000), [this]() {
                                    this->reset_call();
                                    
                                    ESP_LOGD(TAG, "Maximum call time reached - disconnecting call from indoor station");

                                    TelegramData out_telegram_data = send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, this->address_);

                                    #ifdef USE_CALL_ENDED_CALLBACK
                                    this->call_ended_callback_.call(out_telegram_data);
                                    #endif
                                });
                            }

                            // Normally end of door readiness timeout is also started after
                            // initiating a call from the indoor station to the outdoor station
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
                    if (this->call_state_ == CallState::OUT_CHECK_DST)
                    {
                        this->call_state_ = CallState::OUT_RINGING;

                        // reset call ack timeout
                        this->cancel_timeout("wait_for_call_ack");

                        bool ringtone_muted = telegram_data.payload == 3;

                        ESP_LOGD(TAG, "Indoor Station acknowledged (%s), waiting for start talking", ringtone_muted ? "ringtone muted" : "ringing");
                        
                        // wait for start talking telegram
                        this->set_timeout("wait_for_talking_ack", CALL_TIMEOUT_MS, [this]() {
                            this->reset_call();
                            ESP_LOGE(TAG, "Call timeout - No answer from indoor station");

                            #ifdef USE_CALL_FAILED_CALLBACK
                            this->call_failed_callback_.call();
                            #endif
                        });

                        // Door readiness
                        this->door_readiness_address_ = this->address_;
                        this->door_readiness_active_ = true;

                        if (this->door_readiness_duration_ != 0)
                        {
                            this->set_timeout("door_readiness_timeout", (this->door_readiness_duration_ * 8 * 1000), [this]() {
                                ESP_LOGD(TAG, "Maximum door readiness time reached");
                                this->door_readiness_active_ = false;
                                this->door_readiness_address_ = 0;
                                send_telegram(TELEGRAM_TYPE_END_OF_DOOR_READINESS, this->address_);
                            });
                        }
                    }
                }
                else if ((telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR || telegram_data.type == TELEGRAM_TYPE_OPEN_DOOR_LONG) && telegram_data.address == this->address_)
                {
                    // Open door command received from indoor station
                    if (this->door_opener_requires_door_readiness_ == false || (this->door_opener_requires_door_readiness_ && this->door_readiness_active_))
                    {
                        if (this->door_opener_requires_active_call_ == false || (this->door_opener_requires_active_call_ && this->call_state_ == CallState::CONNECTED))
                        {
                            // Ack door opened
                            send_telegram(TELEGRAM_TYPE_ACK_STATUS, 0, 0x001);

                            this->open_door();
                        }
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_RESET || (telegram_data.type == TELEGRAM_TYPE_CONTROL_FUNCTION && telegram_data.payload == 0xD7 && telegram_data.serial_number == this->serial_number_))
                {
                    if (this->tc_bus_->get_selected_device_group() == this->model_data_.device_group || telegram_data.serial_number == this->serial_number_)
                    {
                        this->door_readiness_address_ = 0;
                        this->door_readiness_active_ = false;
                        this->memory_mode_ = false;
                        this->reset_call();
                        this->cancel_timeout("door_readiness_timeout");
                        send_telegram(TELEGRAM_TYPE_INITIALIZE_DOOR_STATION, this->address_);
                    }
                }
            }
        }
        else
        {
            // Physical Device

            // Flow handling
            if (this->current_flow_ != FLOW_NONE)
            {
                if(telegram_data.type == TELEGRAM_TYPE_ACK_DATA)
                {
                    if (this->current_flow_ == FLOW_READ_MEMORY)
                    {
                        uint8_t percent = ((reading_memory_count_ + 1) * 100) / reading_memory_max_;

                        ESP_LOGI(TAG,   "Read memory of %s:\n"
                                        "  Progress: %i%%",
                                        device_group_to_string(this->model_data_.device_group), percent);

                        ESP_LOGD(TAG,   "  Address: %i to %i\n"
                                        "  Block Data: %s",
                                        (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4,
                                        format_hex_pretty(telegram_data.raw, ' ', false).c_str());

                        // Reset counter & timeout
                        reading_memory_timeout_ = 0;
                        reading_memory_try_ = 0;

                        // Save Data to memory buffer
                        if(this->memory_buffer_ != nullptr)
                        {
                            size_t offset = get_page_offset(this->reading_memory_current_page_);
                            uint16_t write_index = offset + (reading_memory_count_ * 4);
                            if (write_index + 3 < this->memory_buffer_size_)
                            {
                                this->memory_buffer_[write_index]     = (telegram_data.raw >> 24) & 0xFF;
                                this->memory_buffer_[write_index + 1] = (telegram_data.raw >> 16) & 0xFF;
                                this->memory_buffer_[write_index + 2] = (telegram_data.raw >> 8) & 0xFF;
                                this->memory_buffer_[write_index + 3] = telegram_data.raw & 0xFF;
                            }
                            else
                            {
                                ESP_LOGW(TAG, "Buffer overflow at index %i, skipping", write_index);
                            }
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Buffer not allocated! Aborting...");

                            #ifdef USE_READ_MEMORY_FAILED_CALLBACK
                            this->read_memory_failed_callback_.call();
                            #endif

                            this->complete_current_flow();

                            return true;
                        }

                        // Next 4 Data Blocks
                        reading_memory_count_++;

                        // Last block of current page complete
                        if (reading_memory_count_ == reading_memory_max_)
                        {
                            // Last page complete
                            if(reading_memory_current_page_ == reading_memory_max_page_ - 1)
                            {
                                this->memory_buffer_ready_ = true;

                                ESP_LOGD(TAG, "  Total size: %i Bytes",
                                              device_group_to_string(this->model_data_.device_group),
                                              this->model_data_.memory_size + (this->model_data_.sides * this->model_data_.memory_size_side));

                                print_memory_buffer();

                                #ifdef USE_READ_MEMORY_COMPLETE_CALLBACK
                                this->read_memory_complete_callback_.call();
                                #endif

                                this->publish_settings();

                                // Complete this request and process next in queue
                                this->complete_current_flow();

                                // Reset
                                reading_memory_try_ = 0;
                                reading_memory_count_ = 0;
                                reading_memory_max_ = 0;
                                reading_memory_current_page_ = 0;
                                reading_memory_max_page_ = 0;
                            }
                            else
                            {
                                reading_memory_current_page_++;
                                read_selected_memory_page();
                            }
                        }
                        else
                        {
                            // Read next block
                            read_selected_memory_block();
                        }

                        // Do not proceed
                        return true;
                    }
                    else if (current_flow_ == FLOW_READ_MEMORY_UPDATE)
                    {
                        ESP_LOGI(TAG,   "Received requested device memory block:\n"
                                        "  Address: %i to %i\n"
                                        "  Block Data: %s", 
                                        (reading_memory_count_ * 4), (reading_memory_count_ * 4) + 4,
                                        format_hex_pretty(telegram_data.raw, ' ', false).c_str());

                        // Reset counter & timeout
                        reading_memory_timeout_ = 0;
                        reading_memory_try_ = 0;

                        // Save Data to memory Store
                        if(this->memory_buffer_ != nullptr)
                        {
                            size_t offset = get_page_offset(this->reading_memory_current_page_);
                            uint16_t write_index = offset + (reading_memory_count_ * 4);

                            if (write_index + 3 < this->memory_buffer_size_)
                            {
                                memory_buffer_[write_index]     = (telegram_data.raw >> 24) & 0xFF;
                                memory_buffer_[write_index + 1] = (telegram_data.raw >> 16) & 0xFF;
                                memory_buffer_[write_index + 2] = (telegram_data.raw >> 8) & 0xFF;
                                memory_buffer_[write_index + 3] = telegram_data.raw & 0xFF;
                            }
                            else
                            {
                                ESP_LOGW(TAG, "Buffer overflow at index %i, skipping", write_index);
                            }
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Buffer not allocated! Aborting...");

                            #ifdef USE_READ_MEMORY_FAILED_CALLBACK
                            this->read_memory_failed_callback_.call();
                            #endif

                            this->complete_current_flow();
                            return true;
                        }

                        this->publish_settings();

                        // Complete this request and process next in queue
                        this->complete_current_flow();

                        // Do not proceed
                        return true;
                    }
                    else if (current_flow_ == FLOW_IDENTIFY_DEVICE)
                    {
                        this->cancel_timeout("wait_for_identification_group_0");
                        this->cancel_timeout("wait_for_identification_group_1");
                        this->cancel_timeout("wait_for_identification_other");

                        ModelData device;
                        device.device_group = this->tc_bus_->get_selected_device_group();

                        if (((telegram_data.raw >> 12) & 0xF) == 0xD)
                        {
                            // New models

                            // Hardware version
                            device.hardware_version = (telegram_data.raw >> 28) & 0xF;

                            const uint16_t model_key = (telegram_data.raw >> 16) & 0xFFF;
                            const uint32_t fw_version = telegram_data.raw & 0xFFF;

                            // FW Version
                            device.firmware_version = fw_version;
                            device.firmware_major   = (fw_version >> 8) & 0xF;
                            device.firmware_minor   = (fw_version >> 4) & 0xF;
                            device.firmware_patch   =  fw_version       & 0xF;

                            // Model
                            device.model = identifier_to_model(device.device_group, model_key, device.hardware_version, fw_version);
                        
                            // Identifier pages
                            // device.sides = (telegram_data.raw >> 16) & 0xF;
                        }
                        else
                        {
                            if (device.device_group == 0 || device.device_group == 1)
                            {
                                // Old indoor station models
                                switch(telegram_data.raw)
                                {
                                    // TTC-XX
                                    case 0x08000040:
                                        device.model = MODEL_IS_TTCXX;
                                        break;

                                    // TTS-XX
                                    case 0x02010040:
                                        device.model = MODEL_IS_TTSXX;
                                        break;

                                    // ISH 1030
                                    case 0x08000048:
                                    case 0x08080048:
                                        device.model = MODEL_IS_ISH1030;
                                        break;

                                    default:
                                        ESP_LOGW(TAG,   "%s partially identified\n"
                                                        "  This device is not fully supported in the identification routine.\n"
                                                        "  Potential models: ISH1030 / VMH / VML / VMF / VME / TTN / TTS / TTF / TC20X\n"
                                                        "  Note: Please open an issue and provide your logs in order to implement support for this device model.",
                                                        device_group_to_string(device.device_group));
                                        break;
                                }
                            }
                            else if (device.device_group == 2)
                            {
                                // Old outdoor station models
                                switch(telegram_data.raw)
                                {
                                    default:
                                        ESP_LOGW(TAG,   "%s partially identified\n"
                                                        "  This device is not fully supported in the identification routine.\n"
                                                        "  Potential models: PAK / PUK / PES / PDS / TCU2 / AVD / VPDS / AVE / VPES / AVU / VPUK / AD1 / AD2 / AD3\n"
                                                        "  Note: Please open an issue and provide your logs in order to implement support for this device model.",
                                                        device_group_to_string(device.device_group));
                                        break;
                                }
                            }
                            else if (device.device_group == 4)
                            {
                                // Old controller models
                                switch(telegram_data.raw)
                                {
                                    case 0x877F5804:
                                        device.model = MODEL_CTRL_BVS20;
                                        break;

                                    default:
                                        ESP_LOGW(TAG,   "%s partially identified\n"
                                                        "  This device is not fully supported in the identification routine.\n"
                                                        "  Potential models: BVS20 / BVS100 / NVV1000 / PSG / VBVS05 / BSG2\n"
                                                        "  Note: Please open an issue and provide your logs in order to implement support for this device model.",
                                                        device_group_to_string(device.device_group));
                                        break;
                                }
                            }
                            else
                            {
                                // Old models of other groups
                                // Not implemented
                            }
                        }

                        if (device.model != MODEL_NONE)
                        {
                            // Add missing information
                            ModelData m = getModelData(device.model);
                            device.memory_size = m.memory_size;
                            device.memory_size_side = m.memory_size_side;
                            device.sides = m.sides;

                            ESP_LOGI(TAG,   "%s identified\n"
                                            "  Model: %s\n"
                                            "  Version: %i\n"
                                            "  Firmware: %i.%i.%i",
                                            device_group_to_string(device.device_group),
                                            model_to_string(device.model),
                                            device.hardware_version,
                                            device.firmware_major,
                                            device.firmware_minor,
                                            device.firmware_patch);

                            // Update Model
                            this->set_model(device.model);

                            #ifdef USE_IDENTIFY_COMPLETE_CALLBACK
                            this->identify_complete_callback_.call(device);
                            #endif
                        }
                        else
                        {
                            ESP_LOGE(TAG,   "Unable to identify %s\n"
                                            "  Response: %s\n"
                                            "  Note: Please open an issue and provide your logs in order to implement support for this device model.",
                                            device_group_to_string(device.device_group),
                                            telegram_data.hex);

                            #ifdef USE_IDENTIFY_UNKNOWN_CALLBACK
                            this->identify_unknown_callback_.call();
                            #endif
                        }

                        // Complete this request and process next in queue
                        this->complete_current_flow();

                        // Do not proceed
                        return true;
                    }
                }
            }

            // Device specific
            if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
            {
                if (telegram_data.type == TELEGRAM_TYPE_DOOR_CALL)
                {
                    if(telegram_data.serial_number == this->serial_number_ || telegram_data.serial_number == this->parallel_serial_number_)
                    {
                        this->door_readiness_address_ = telegram_data.address;
                        this->door_readiness_active_ = true;
                    }
                    else
                    {
                        // Reset door readiness if call was for another device
                        this->door_readiness_address_ = 0;
                        this->door_readiness_active_ = false;
                    }
                }
                else if (telegram_data.type == TELEGRAM_TYPE_END_OF_DOOR_READINESS)
                {
                    this->door_readiness_address_ = 0;
                    this->door_readiness_active_ = false;
                }
                else if (telegram_data.type == TELEGRAM_TYPE_RESET || (telegram_data.type == TELEGRAM_TYPE_CONTROL_FUNCTION && telegram_data.payload == 0xD7 && telegram_data.serial_number == this->serial_number_))
                {
                    if (this->tc_bus_->get_selected_device_group() == this->model_data_.device_group || telegram_data.serial_number == this->serial_number_)
                    {
                        this->reset_call();
                        this->door_readiness_address_ = 0;
                        this->door_readiness_active_ = false;
                    }
                }
            }
        }

        #ifdef USE_BINARY_SENSOR
        // Fire Binary Sensors
        for (auto &listener : listeners_)
        {
            bool allow_publish = (telegram_data.type == (listener->type_.value_or(TELEGRAM_TYPE_UNKNOWN))) &&
                (telegram_data.address == listener->address_.value_or(0) || listener->address_.value_or(0) == 255) &&
                (telegram_data.payload == listener->payload_.value_or(0) || listener->payload_.value_or(0) == 255) &&
                (telegram_data.serial_number == this->serial_number_);

            // Trigger listener binary sensor if match found
            if (allow_publish)
            {
                listener->turn_on(&listener->timer_, listener->auto_reset_);
            }
        }
        #endif

        return true;
    }

    void TCBusDeviceComponent::print_memory_buffer()
    {
        size_t offset = 0;
        for (uint8_t current_page = 0; current_page < this->model_data_.sides + 1; current_page++)
        {
            const size_t page_size = (current_page == 0) ? this->model_data_.memory_size : this->model_data_.memory_size_side;
            const size_t chunk_size = 16;
            const uint8_t* page_ptr = memory_buffer_ + offset;

            for (size_t i = 0; i < page_size; i += chunk_size)
            {
                char line[3 * chunk_size + 1];
                size_t pos = 0;

                for (size_t j = 0; j < chunk_size && (i + j) < page_size; j++)
                {
                    pos += sprintf(&line[pos], "%02X ", page_ptr[i + j]);
                }

                line[pos] = '\0';

                ESP_LOGI(TAG, "  [%i][%03u]: %s", current_page, i, line);
            }

            offset += page_size;
        }
    }

    void TCBusDeviceComponent::read_selected_memory_block()
    {
        if (reading_memory_try_ == 3)
        {
            // Timeout - reset
            clear_memory_buffer();

            reading_memory_try_ = 0;
            reading_memory_count_ = 0;
            reading_memory_max_ = 0;
            reading_memory_max_page_ = 0;
            reading_memory_current_page_ = 0;

            ESP_LOGE(TAG, "Memory reading cancelled after timeout!");

            #ifdef USE_READ_MEMORY_FAILED_CALLBACK
            this->read_memory_failed_callback_.call();
            #endif

            // Complete this request and process next in queue
            this->complete_current_flow();

            return;
        }

        send_telegram(TELEGRAM_TYPE_READ_MEMORY_BLOCK, (reading_memory_count_ * 4), 0, this->serial_number_, 260);
        reading_memory_try_++;
        reading_memory_timeout_ = millis() + 2000;
    }

    void TCBusDeviceComponent::open_door()
    {
        if (this->virtual_ && this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            // Virtual Outdoor Stations

            this->cancel_timeout("door_opener_timeout");

            ESP_LOGD(TAG, "Turn on door opener");

            #ifdef USE_BINARY_SENSOR
            if (this->door_opener_binary_sensor_ != nullptr)
            {
                this->door_opener_binary_sensor_->publish_state(true);
            }
            #endif

            this->set_timeout("door_opener_timeout", this->door_opener_duration_ * 1000, [this]() {
                ESP_LOGD(TAG, "Time elapsed - turn off door opener");

                #ifdef USE_BINARY_SENSOR
                if (this->door_opener_binary_sensor_ != nullptr)
                {
                    this->door_opener_binary_sensor_->publish_state(false);
                }
                #endif
            });
        }
        else if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
        {
            // Indoor Stations

            TelegramType type = TELEGRAM_TYPE_OPEN_DOOR;
            if (this->virtual_ == false && this->get_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL) == 1)
            {
                type = TELEGRAM_TYPE_OPEN_DOOR_LONG;
            }
            else if (this->use_long_door_opener_protocol_)
            {
                type = TELEGRAM_TYPE_OPEN_DOOR_LONG;
            }

            send_telegram(type, this->door_readiness_address_, this->door_readiness_active_ ? 1 : 0);
        }
        else
        {
            ESP_LOGW(TAG, "This method is only available for virtual indoor- and outdoor stations, and physical indoor stations");
        }
    }

    bool TCBusDeviceComponent::reset_call()
    {
        bool was_active = (this->call_state_ != CallState::IDLE && this->call_state_ != CallState::LINE_BUSY);
        
        this->call_internal_ = false;
        this->call_from_parallel_sn_ = false;
        this->call_address_ = 0;
        this->call_state_ = CallState::IDLE;

        if (was_active)
        {
            this->cancel_timeout("call_time_limit");
            this->cancel_timeout("wait_for_call_ack");
            this->cancel_timeout("wait_for_talking_ack");
            return true;
        }

        return false;
    }

    void TCBusDeviceComponent::call(uint32_t destination, bool internal)
    {
        if (!this->virtual_)
        {
            ESP_LOGW(TAG, "This method is only available for virtual devices");
            return;
        }

        if (this->call_state_ != CallState::IDLE)
        {
            ESP_LOGE(TAG, "Line is busy");
            return;
        }

        this->call_address_ = destination;

        if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
        {
            if (internal)
            {
                ESP_LOGI(TAG, "Virtual device: Calling indoor station %d", destination);

                // Call indoor station serial number from address 63 (static)
                this->call_internal_ = true;
                this->call_state_ = CallState::OUT_CHECK_DST;
                send_telegram(TELEGRAM_TYPE_INTERNAL_CALL, 63, 0, destination);

                // ack timeout
                this->set_timeout("wait_for_call_ack", ACK_TIMEOUT_MS, [this]() {
                    this->reset_call();
                    ESP_LOGE(TAG, "Call failed - No response from indoor station");

                    #ifdef USE_CALL_FAILED_CALLBACK
                    this->call_failed_callback_.call();
                    #endif
                });
            }
            else
            {
                ESP_LOGI(TAG, "Virtual device: Talk to outdoor station %d", destination);

                // Talk directly to outdoor station address (destination) from this serial number
                this->call_internal_ = false;
                this->call_state_ = CallState::IN_WAIT_FOR_INIT;
                send_telegram(TELEGRAM_TYPE_START_TALKING_DOOR_CALL, destination, this->door_readiness_active_ ? 1 : 0);
                
                // ack timeout
                this->set_timeout("wait_for_talking_ack", ACK_TIMEOUT_MS, [this]() {
                    this->reset_call();
                    ESP_LOGE(TAG, "Call timeout - No answer from outdoor station");

                    #ifdef USE_CALL_FAILED_CALLBACK
                    this->call_failed_callback_.call();
                    #endif
                });
            }
        }
        else if (this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGI(TAG, "Virtual device: Calling indoor station %d", destination);
            
            this->cancel_timeout("door_readiness_timeout");

            // Call indoor station serial number from this outdoor station
            this->call_internal_ = false;
            this->call_state_ = CallState::OUT_CHECK_DST;

            // send door call to indoor station from this outdoor station
            // destination serial number of indoor station
            // source address of this outdoor station
            send_telegram(TELEGRAM_TYPE_DOOR_CALL, this->address_, 0, destination);
            
            // ack timeout
            this->set_timeout("wait_for_call_ack", ACK_TIMEOUT_MS, [this]() {
                this->reset_call();
                ESP_LOGE(TAG, "Call failed - No response from indoor station");

                #ifdef USE_CALL_FAILED_CALLBACK
                this->call_failed_callback_.call();
                #endif
            });
        }
    }

    void TCBusDeviceComponent::answer_call()
    {
        if (!this->virtual_ && (this->device_group_ != DEVICE_GROUP_INDOOR_STATION_CLASSIC && this->device_group_ != DEVICE_GROUP_INDOOR_STATION_HANDSFREE))
        {
            ESP_LOGW(TAG, "This method is only available for indoor stations and virtual outdoor stations");
            return;
        }

        if (this->call_state_ != CallState::IN_RINGING)
        {
            ESP_LOGE(TAG, "No call to answer!");
            return;
        }

        if(this->call_from_parallel_sn_)
        {
            ESP_LOGW(TAG, "Virtual device: Answering call for parallel device");
        }
        else
        {
            ESP_LOGI(TAG, "Virtual device: Answering call for this device");
        }

        send_telegram(this->call_internal_ ? TELEGRAM_TYPE_START_TALKING : TELEGRAM_TYPE_START_TALKING_DOOR_CALL, this->call_address_, this->call_internal_ ? 0 : (this->door_readiness_active_ ? 1 : 0), this->call_from_parallel_sn_ ? this->parallel_serial_number_ : this->serial_number_);
        this->call_state_ = CallState::IN_WAIT_FOR_INIT;

        // ack timeout
        this->set_timeout("wait_for_talking_ack", ACK_TIMEOUT_MS, [this]() {
            ESP_LOGE(TAG, "Call timeout - no answer from other device %s station", this->call_internal_ ? "indoor" : "outdoor");

            this->reset_call();

            #ifdef USE_CALL_FAILED_CALLBACK
            this->call_failed_callback_.call();
            #endif
        });
    }

    void TCBusDeviceComponent::end_call()
    {
        if (!this->virtual_ && (this->device_group_ != DEVICE_GROUP_INDOOR_STATION_CLASSIC && this->device_group_ != DEVICE_GROUP_INDOOR_STATION_HANDSFREE))
        {
            ESP_LOGW(TAG, "This method is only available for indoor stations and virtual outdoor stations");
            return;
        }

        if (this->call_state_ != CallState::CONNECTED && this->call_state_ != CallState::OUT_CHECK_DST && this->call_state_ != CallState::OUT_RINGING)
        {
            ESP_LOGE(TAG, "No call to end!");
            return;
        }

        TelegramData telegram_data;

        if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
        {
            if (this->call_internal_)
            {
                ESP_LOGD(TAG, "End call - disconnecting call (VIS -> IS)");

                telegram_data = send_telegram(TELEGRAM_TYPE_STOP_TALKING, 63);
            }
            else
            {
                ESP_LOGD(TAG, "End call - disconnecting call (VIS -> AS)");
                telegram_data = send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, (uint8_t)this->call_address_);
            }
        }
        else if (this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGD(TAG, "End call - disconnecting call (AS -> (V)IS)");
            telegram_data = send_telegram(TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL, this->address_);
        }

        this->reset_call();

        #ifdef USE_CALL_ENDED_CALLBACK
        this->call_ended_callback_.call(telegram_data);
        #endif
    }

    void TCBusDeviceComponent::publish_settings()
    {
        if (!this->memory_buffer_ready_)
        {
            return;
        }

        ESP_LOGI(TAG,   this->virtual_ ? "Virtual %s Settings:\n" : "%s Settings:\n"
                        "  Model: %s\n"
                        "  Serial Number: %i",
                       device_group_to_string(this->model_data_.device_group),  model_to_string(this->model_), this->serial_number_);

        if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
        {
            if (supports_setting(SETTING_PARALLEL_SERIAL_NUMBER))
            {
                ESP_LOGI(TAG, "  Parallel Serial Number: %i", get_setting(SETTING_PARALLEL_SERIAL_NUMBER));
            }

            ESP_LOGI(TAG, "  Ringtone Settings:");
            if (supports_setting(SETTING_VOLUME_RINGTONE))
            {
                ESP_LOGI(TAG, "    Volume: %i", get_setting(SETTING_VOLUME_RINGTONE));
            }
            if (supports_setting(SETTING_RINGTONE_MUTE))
            {
                ESP_LOGI(TAG, "    Muted: %s", YESNO(get_setting(SETTING_RINGTONE_MUTE)));
            }
            
            if (supports_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL))
            {
                ESP_LOGI(TAG, "    Entrance Door Call: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL)));
            }
            if (supports_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL))
            {
                ESP_LOGI(TAG, "    Second Entrance Door Call: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL)));
            }
            if (supports_setting(SETTING_RINGTONE_INTERNAL_CALL))
            {
                ESP_LOGI(TAG, "    Internal Call: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_INTERNAL_CALL)));
            }
            if (supports_setting(SETTING_RINGTONE_FLOOR_CALL))
            {
                ESP_LOGI(TAG, "    Floor Call: %s", int_to_ringtone(get_setting(SETTING_RINGTONE_FLOOR_CALL)));
            }
            

            if (supports_setting(SETTING_VOLUME_HANDSET_DOOR_CALL))
            {
                ESP_LOGI(TAG, "  Handset volume (Door Call): %i", get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
            }
            if (supports_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL))
            {
                ESP_LOGI(TAG, "  Handset volume (Internal Call): %i", get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
            }

            if (supports_setting(SETTING_AS_ADDRESS_DIVIDER))
            {
                ESP_LOGI(TAG, "  Address Divider (AS): %i", get_setting(SETTING_AS_ADDRESS_DIVIDER));
            }
            if (supports_setting(SETTING_VAS_ADDRESS_DIVIDER))
            {
                ESP_LOGI(TAG, "  Address Divider (VAS): %i", get_setting(SETTING_VAS_ADDRESS_DIVIDER));
            }

            if (supports_setting(SETTING_AUTO_ANSWER_CALL))
            {
                ESP_LOGI(TAG, "  Auto Answer Call: %s", YESNO(get_setting(SETTING_AUTO_ANSWER_CALL)));
            }
            if (supports_setting(SETTING_CALL_TIME_UNLIMITED))
            {
                ESP_LOGI(TAG, "  Call Time Unlimited: %s", YESNO(get_setting(SETTING_CALL_TIME_UNLIMITED)));
            }

            if (supports_setting(SETTING_AMBIENT_LIGHT_IN_STANDBY))
            {
                ESP_LOGI(TAG, "  Ambient Light in standby: %s", YESNO(get_setting(SETTING_AMBIENT_LIGHT_IN_STANDBY) == 0));
            }

            if (supports_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL))
            {
                ESP_LOGI(TAG, "  Always use long door opener protocol: %s", YESNO(get_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL)));
            }

            #ifdef USE_SELECT
            if (this->ringtone_entrance_door_call_select_)
            {
                this->ringtone_entrance_door_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL)));
            }
            if (this->ringtone_second_entrance_door_call_select_)
            {
                this->ringtone_second_entrance_door_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL)));
            }
            if (this->ringtone_floor_call_select_)
            {
                this->ringtone_floor_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_FLOOR_CALL)));
            }
            if (this->ringtone_internal_call_select_)
            {
                this->ringtone_internal_call_select_->publish_state(int_to_ringtone(get_setting(SETTING_RINGTONE_INTERNAL_CALL)));
            }
            #endif

            #ifdef USE_NUMBER
            if (this->parallel_serial_number_number_ != nullptr)
            {
                this->parallel_serial_number_number_->publish_state(get_setting(SETTING_PARALLEL_SERIAL_NUMBER));
            }
            if (this->address_divider_number_ != nullptr)
            {
                this->address_divider_number_->publish_state(get_setting(SETTING_AS_ADDRESS_DIVIDER));
            }
            if (this->volume_handset_door_call_number_)
            {
                this->volume_handset_door_call_number_->publish_state(get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));
            }
            if (this->volume_handset_internal_call_number_)
            {
                this->volume_handset_internal_call_number_->publish_state(get_setting(SETTING_VOLUME_HANDSET_INTERNAL_CALL));
            }
            if (this->volume_ringtone_number_)
            {
                this->volume_ringtone_number_->publish_state(get_setting(SETTING_VOLUME_RINGTONE));
            }
            #endif

            #ifdef USE_SWITCH
            if (this->ringtone_mute_switch_)
            {
                this->ringtone_mute_switch_->publish_state(get_setting(SETTING_RINGTONE_MUTE) == 1);
            }
            if (this->auto_answer_call_switch_)
            {
                this->auto_answer_call_switch_->publish_state(get_setting(SETTING_AUTO_ANSWER_CALL) == 1);
            }
            if (this->use_long_door_opener_protocol_switch_)
            {
                this->use_long_door_opener_protocol_switch_->publish_state(get_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL) == 1);
            }
            if (this->call_time_unlimited_switch_)
            {
                this->call_time_unlimited_switch_->publish_state(get_setting(SETTING_CALL_TIME_UNLIMITED) == 1);
            }
            #endif
        }
        else if (this->device_group_ == DEVICE_GROUP_OUTDOOR_STATION)
        {
            if (supports_setting(SETTING_ADDRESS))
            {
                ESP_LOGI(TAG, "  Address: %i", get_setting(SETTING_ADDRESS));
            }
            if (supports_setting(SETTING_ADDRESS_LOCK))
            {
                ESP_LOGI(TAG, "  Address Lock: %s", YESNO(get_setting(SETTING_ADDRESS_LOCK)));
            }
            if (supports_setting(SETTING_CALLING_REQUIRES_DOOR_READINESS))
            {
                ESP_LOGI(TAG, "  Calling requires door readiness: %s", YESNO(get_setting(SETTING_CALLING_REQUIRES_DOOR_READINESS)));
            }
            if (supports_setting(SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS))
            {
                ESP_LOGI(TAG, "  Door Opener requires door readiness: %s", YESNO(get_setting(SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS)));
            }
            if (supports_setting(SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL))
            {
                ESP_LOGI(TAG, "  Door Opener requires active call: %s", YESNO(get_setting(SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL)));
            }
            if (supports_setting(SETTING_DOOR_OPENER_DURATION))
            {
                uint8_t door_opener_dur = get_setting(SETTING_DOOR_OPENER_DURATION);
                ESP_LOGI(TAG, "  Door Opener Duration: %i sec.", door_opener_dur);
            }
            if (supports_setting(SETTING_CALL_TIME_DURATION))
            {
                uint8_t calling_dur = get_setting(SETTING_CALL_TIME_DURATION);
                if (calling_dur == 0)
                {
                    ESP_LOGI(TAG, "  Maximum Call Duration: Unlimited");
                }
                else
                {
                    ESP_LOGI(TAG, "  Maximum Call Duration: %i sec.", calling_dur * 8);
                }
            }
            if (supports_setting(SETTING_DOOR_READINESS_DURATION))
            {
                uint8_t door_readiness_dur = get_setting(SETTING_DOOR_READINESS_DURATION);
                if (door_readiness_dur == 0)
                {
                    ESP_LOGI(TAG, "  Door Readiness Duration: Unlimited");
                }
                else
                {
                    ESP_LOGI(TAG, "  Door Readiness Duration: %i sec.", door_readiness_dur * 8);
                }
            }

            uint8_t button_rows = get_setting(SETTING_BUTTON_ROWS);
            uint8_t button_cols = 1;

            if (this->model_ == MODEL_AS_TCU3     || this->model_ == MODEL_AS_TCU3_EX1 || this->model_ == MODEL_AS_TCU3_EX2 || 
                this->model_ == MODEL_AS_TCU3_EX3 || this->model_ == MODEL_AS_TCU3_EX4 || this->model_ == MODEL_AS_TCU3_EX5 || 
                this->model_ == MODEL_AS_TCU3_EX6 || this->model_ == MODEL_AS_TCU4     || this->model_ == MODEL_AS_TCU4_EX1 || 
                this->model_ == MODEL_AS_TCU4_EX2 || this->model_ == MODEL_AS_TCU4_EX3 || this->model_ == MODEL_AS_TCU4_EX4 || 
                this->model_ == MODEL_AS_TCU4_EX5 || this->model_ == MODEL_AS_TCU4_EX6 || this->model_ == MODEL_AS_TCU2)
            {
                // Can have multiple extensions
                // working with memory pages
                button_rows = 4;
                button_cols = 4;
            }
            else if (this->model_ == MODEL_AS_TKM_AS || this->model_ == MODEL_AS_TKM_ASV)
            {
                // Multiple button layouts based on index 7 and 112
                uint8_t station_length = get_memory_bit(7, 0) ? 1 : 0;
                uint8_t layout_byte = get_memory_bits(112, 6, 7);
                uint8_t max_buttons = (station_length == 0) ? 4 : 8;
                uint8_t total_bits  = max_buttons - 1;
                uint8_t button_index = 1;
                uint8_t width = 1;

                button_rows = 0;
                for (uint8_t i = 0; i <= total_bits && button_index <= max_buttons; i++)
                {
                    bool bit = (layout_byte >> i) & 1;

                    if (bit == 1)
                    {
                        width++;
                    }
                    else
                    {
                        button_rows++;
                        button_index += width;
                        width = 1;
                    }
                }
            }
            else if (this->model_ == MODEL_AS_PES)
            {
                button_cols = 2;
            }
            else if (this->model_ == MODEL_AS_PDS0X || this->model_ == MODEL_AS_PDS0X04)
            {
                if (get_setting(SETTING_HAS_CODE_LOCK) == 254)
                {
                    button_rows = 1;
                }
            }

            ESP_LOGI(TAG, "  Device Button Configuration: %d", button_rows * button_cols);
            if (button_rows == 0 || button_rows == 255 || button_cols == 0)
            {
                ESP_LOGW(TAG, "    Invalid button configuration: rows=%i, cols=%i. Skipping button listing.", button_rows, button_cols);
            }
            else
            {
                if(this->model_ == MODEL_AS_TKM_AS || this->model_ == MODEL_AS_TKM_ASV)
                {
                    uint8_t station_length = get_memory_bit(7, 0) ? 1 : 0;
                    uint8_t layout_byte = get_memory_bits(112, 6, 7);
                    uint8_t max_buttons = (station_length == 0) ? 4 : 8;
                    uint8_t total_bits  = max_buttons - 1;
                    uint8_t button_index = 1;
                    uint8_t width = 1;

                    for (uint8_t i = 0; i <= total_bits && button_index <= max_buttons; i++)
                    {
                        bool bit = (layout_byte >> i) & 1;

                        if (bit == 1)
                        {
                            width++;
                        }
                        else
                        {
                            DoorbellButtonConfig btn = get_doorbell_button(button_index, 0);
                            log_doorbell_button(btn, button_index, 0);
                            button_index += width;
                            width = 1;
                        }
                    }
                }
                else
                {
                    // For each extension
                    for (uint8_t current_side = 0; current_side <= this->model_data_.sides; current_side++)
                    {
                        ESP_LOGD(TAG, "Loop state: current_side=%d, sides=%d, button_rows=%d, button_cols=%d", current_side, this->model_data_.sides, button_rows, button_cols);

                        if(current_side != 0)
                        {
                            // TCKE3
                            if (this->model_ == MODEL_AS_TCU3     || this->model_ == MODEL_AS_TCU3_EX1 || this->model_ == MODEL_AS_TCU3_EX2 || 
                                this->model_ == MODEL_AS_TCU3_EX3 || this->model_ == MODEL_AS_TCU3_EX4 || this->model_ == MODEL_AS_TCU3_EX5 || 
                                this->model_ == MODEL_AS_TCU3_EX6 || this->model_ == MODEL_AS_TCU4     || this->model_ == MODEL_AS_TCU4_EX1 || 
                                this->model_ == MODEL_AS_TCU4_EX2 || this->model_ == MODEL_AS_TCU4_EX3 || this->model_ == MODEL_AS_TCU4_EX4 || 
                                this->model_ == MODEL_AS_TCU4_EX5 || this->model_ == MODEL_AS_TCU4_EX6)
                            {
                                button_cols = 8;
                            }

                            ESP_LOGI(TAG, "  Extension %d Button Configuration: %d", current_side, button_rows * button_cols);
                        }

                        for (uint8_t row = 1; row <= button_rows; row++)
                        {
                            ESP_LOGD(TAG, "Row start: row=%d, button_rows=%d, button_cols=%d, current_side=%d", row, button_rows, button_cols, current_side);

                            for (uint8_t col = 1; col <= button_cols; col++)
                            {
                                ESP_LOGD(TAG, "Col start: col=%d", col);

                                // For non-matrix models, col is ignored (pass 0)
                                DoorbellButtonConfig btn = get_doorbell_button(row, col, current_side);

                                ESP_LOGD(TAG, "After get_doorbell_button: row=%d, col=%d, button_rows=%d, button_cols=%d, current_side=%d", row, col, button_rows, button_cols, current_side);

                                log_doorbell_button(btn, row, col);

                                ESP_LOGD(TAG, "After log: row=%d, col=%d, button_rows=%d, button_cols=%d", row, col, button_rows, button_cols);
                                ESP_LOGD(TAG, "Col end: col=%d", col);
                            }

                            ESP_LOGD(TAG, "Row end: row=%d", row);
                        }

                        ESP_LOGD(TAG, "Row end");
                    }
                }
            }

            #ifdef USE_NUMBER
            if (this->address_number_ != nullptr)
            {
                this->address_number_->publish_state(get_setting(SETTING_ADDRESS));
            }
            if (this->door_opener_duration_number_ != nullptr)
            {
                this->door_opener_duration_number_->publish_state(get_setting(SETTING_DOOR_OPENER_DURATION) * 8);
            }
            if (this->call_time_duration_number_ != nullptr)
            {
                this->call_time_duration_number_->publish_state(get_setting(SETTING_CALL_TIME_DURATION) * 8);
            }
            if (this->door_readiness_duration_number_ != nullptr)
            {
                this->door_readiness_duration_number_->publish_state(get_setting(SETTING_DOOR_READINESS_DURATION));
            }
            #endif

            #ifdef USE_SWITCH
            if (this->address_lock_switch_ != nullptr)
            {
                this->address_lock_switch_->publish_state(get_setting(SETTING_ADDRESS_LOCK) == 1);
            }
            if (this->calling_requires_door_readiness_switch_ != nullptr)
            {
                this->calling_requires_door_readiness_switch_->publish_state(get_setting(SETTING_CALLING_REQUIRES_DOOR_READINESS) == 1);
            }
            if (this->door_opener_requires_door_readiness_switch_ != nullptr)
            {
                this->door_opener_requires_door_readiness_switch_->publish_state(get_setting(SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS) == 1);
            }
            if (this->door_opener_requires_active_call_switch_ != nullptr)
            {
                this->door_opener_requires_active_call_switch_->publish_state(get_setting(SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL) == 1);
            }
            #endif
        }
        else
        {
            // Other devices
            ESP_LOGW(TAG, "  Note: This Device Group is not implemented yet!");
        }
    }

    void TCBusDeviceComponent::log_doorbell_button(const DoorbellButtonConfig& btn, uint8_t row, uint8_t col)
    {
        ESP_LOGI(TAG, "    Button [%i,%i]:", row, col);
        
        ESP_LOGI(TAG, "      Primary Action: %s", doorbell_button_action_to_string(btn.primary_action));
        if (btn.primary_action != DOORBELL_BUTTON_ACTION_NONE && btn.primary_action != DOORBELL_BUTTON_ACTION_LIGHT)
        {
            ESP_LOGI(TAG, "        Payload: %i", btn.primary_payload);
        }
        if (btn.primary_action == DOORBELL_BUTTON_ACTION_DOOR_CALL)
        {
            ESP_LOGI(TAG, "      Secondary Action: %s", doorbell_button_action_to_string(btn.secondary_action));
            if (btn.secondary_action != DOORBELL_BUTTON_ACTION_NONE && btn.secondary_action != DOORBELL_BUTTON_ACTION_LIGHT)
            {
                ESP_LOGI(TAG, "        Payload: %i", btn.secondary_payload);
            }
        }
    }

    #ifdef USE_BINARY_SENSOR
    void TCBusDeviceComponent::register_listener(TCBusDeviceListener *listener)
    {
        this->listeners_.push_back(listener);
    }
    #endif

    TelegramData TCBusDeviceComponent::send_telegram(TelegramType type, uint8_t address, uint32_t payload, uint32_t serial_number, uint32_t wait_duration)
    {
        // Use 32-bit protocol
        if (type == TELEGRAM_TYPE_OPEN_DOOR)
        {
            if (this->virtual_ == false && this->get_setting(SETTING_USE_LONG_DOOR_OPENER_PROTOCOL) == 1)
            {
                type = TELEGRAM_TYPE_OPEN_DOOR_LONG;
            }
            else if (this->use_long_door_opener_protocol_)
            {
                type = TELEGRAM_TYPE_OPEN_DOOR_LONG;
            }
        }

        if(serial_number == 0)
        {
            serial_number = this->serial_number_;
        }

        return this->tc_bus_->send_telegram(type, address, payload, serial_number, wait_duration, this->listener_id_);
    }

    void TCBusDeviceComponent::identify_device()
    {
        if (this->virtual_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");

            #ifdef USE_IDENTIFY_FAILED_CALLBACK
            this->identify_failed_callback_.call();
            #endif

            return;
        }

        if (this->serial_number_ == 0)
        {
            ESP_LOGE(TAG, "Device model cannot be identified without a serial number!");

            #ifdef USE_IDENTIFY_FAILED_CALLBACK
            this->identify_failed_callback_.call();
            #endif

            return;
        }

        enqueue_flow(FLOW_IDENTIFY_DEVICE, this);
    }

    void TCBusDeviceComponent::execute_identify_device()
    {
        if (this->virtual_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        this->cancel_timeout("wait_for_identification_group_0");
        this->cancel_timeout("wait_for_identification_group_1");
        this->cancel_timeout("wait_for_identification_other");

        if (this->device_group_ == DEVICE_GROUP_INDOOR_STATION_CLASSIC || this->device_group_ == DEVICE_GROUP_INDOOR_STATION_HANDSFREE)
        {
            // Indoor Stations

            // First try with group 0
            ESP_LOGI(TAG,   "Identify %s:\n"
                            "  Serial Number: %i",
                            device_group_to_string(DEVICE_GROUP_INDOOR_STATION_CLASSIC), this->serial_number_);

            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, 0, 0, 280); // group 0
            send_telegram(TELEGRAM_TYPE_REQUEST_VERSION);

            this->set_timeout("wait_for_identification_group_0", 1000, [this]()
            {
                // Didn't receive identify result of group 0
                // Second try with group 1
                ESP_LOGI(TAG,   "Identify %s:\n"
                                "  Serial Number: %i",
                                device_group_to_string(DEVICE_GROUP_INDOOR_STATION_HANDSFREE), this->serial_number_);

                send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, 1, 0, 280); // group 1
                send_telegram(TELEGRAM_TYPE_REQUEST_VERSION);

                this->set_timeout("wait_for_identification_group_1", 1000, [this]()
                {
                    // Didn't receive identify result of group 1
                    // Failed

                    #ifdef USE_IDENTIFY_FAILED_CALLBACK
                    this->identify_failed_callback_.call();
                    #endif

                    ESP_LOGE(TAG, "Identification timeout. The device model may not support identification. Please select model manually.");

                    // Complete this request and process next in queue
                    this->complete_current_flow();
                });
            });
        }
        else
        {
            // Other Devices

            ESP_LOGI(TAG,   "Identify %s:\n"
                            "  Serial Number: %i",
                            device_group_to_string(this->device_group_), this->serial_number_);

            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, (uint8_t)this->device_group_, 0, 280);
            send_telegram(TELEGRAM_TYPE_REQUEST_VERSION);

            this->set_timeout("wait_for_identification_other", 1000, [this]() {
                // Failed
                ESP_LOGE(TAG, "Identification timeout:\n"
                              "  The device model may not support identification.\n"
                              "  Please select the model manually.");

                // Complete this request and process next in queue
                this->complete_current_flow();
            });
        }
    }

    // Flow Queue Management
    void TCBusDeviceComponent::enqueue_flow(FlowType type, TCBusDeviceComponent *component, uint8_t index)
    {
        TCBusDeviceFlowQueueItem flow;
        flow.type = type;
        flow.component = component;
        flow.index = index;
        
        if (!s_flow_queue.push(flow))
        {
            ESP_LOGE(component->TAG, "Flow queue is full! Flow dropped.");
            return;
        }

        ESP_LOGD(component->TAG, "Flow enqueued.");
    }

    void TCBusDeviceComponent::process_flow_queue()
    {
        if (s_flow_queue.empty())
        {
            ESP_LOGV(FLOW_QUEUE_TAG, "Queue is empty, nothing to process.");
            return;
        }
        
        if (s_running_flow)
        {
            ESP_LOGV(FLOW_QUEUE_TAG, "Flow already running, waiting...");
            return;
        }

        uint32_t now = millis();
        if (s_last_flow_completion_time > 0 && (now - s_last_flow_completion_time) < 1000)
        {
            ESP_LOGV(FLOW_QUEUE_TAG, "Waiting for cooldown period (%u ms remaining)...", 1000 - (now - s_last_flow_completion_time));
            return;
        }

        TCBusDeviceFlowQueueItem flow = s_flow_queue.front();
        s_flow_queue.pop();

        ESP_LOGD(flow.component->TAG, "Processing queued flow.");

        s_running_flow = true;
        flow.component->set_current_flow(flow.type);

        // Execute the queued flow
        switch (flow.type)
        {
            case FLOW_READ_MEMORY:
                flow.component->execute_read_memory();
                break;
            case FLOW_READ_MEMORY_UPDATE:
                flow.component->execute_read_memory_update(flow.index);
                break;
            case FLOW_IDENTIFY_DEVICE:
                flow.component->execute_identify_device();
                break;
            case FLOW_NONE:
            default:
                ESP_LOGW(flow.component->TAG, "Invalid flow type %d, skipping.", flow.type);
                flow.component->complete_current_flow();
                break;
        }
    }

    void TCBusDeviceComponent::complete_current_flow()
    {
        if (current_flow_ == FLOW_NONE)
        {
            ESP_LOGW(TAG, "complete_current_flow() called but no flow was running!");
            return;
        }

        s_running_flow = false;
        current_flow_ = FLOW_NONE;

        s_last_flow_completion_time = millis();
    }

    void TCBusDeviceComponent::read_memory()
    {
        if (this->virtual_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        if (this->serial_number_ == 0)
        {
            ESP_LOGE(TAG, "Unable to read device memory without a serial number!");
            return;
        }

        if (this->model_ == MODEL_NONE)
        {
            ESP_LOGE(TAG, "Unable to read device memory without a model!");
            return;
        }

        if (this->model_data_.memory_size == 0)
        {
            ESP_LOGE(TAG, "The model %s (Serial: %i) does not support reading memory!", model_to_string(this->model_), this->serial_number_);
            // Call timeout callback for unsupported models
            #ifdef USE_READ_MEMORY_FAILED_CALLBACK
            this->read_memory_failed_callback_.call();
            #endif
            return;
        }

        // Check if flow is already running
        enqueue_flow(FLOW_READ_MEMORY, this);
    }

    void TCBusDeviceComponent::execute_read_memory()
    {
        ESP_LOGI(TAG,   "Read memory of %s:\n"
                        "  Model: %s\n"
                        "  Serial Number: %i\n"
                        "  Sides: %i",
                        device_group_to_string(this->model_data_.device_group), model_to_string(this->model_), this->serial_number_, this->model_data_.sides);

        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group, 0, 280);

        // Clear memory
        clear_memory_buffer();

        // Reset page state
        reading_memory_current_page_ = 0;
        reading_memory_max_page_ = 1 + this->model_data_.sides; // base page + side pages

        read_selected_memory_page();
    }

    void TCBusDeviceComponent::read_selected_memory_page()
    {
        ESP_LOGD(TAG, "Read memory of page %i", reading_memory_current_page_);

        send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, reading_memory_current_page_);

        reading_memory_try_ = 0;
        reading_memory_count_ = 0;
        reading_memory_max_ = (reading_memory_current_page_ == 0) ? (this->model_data_.memory_size / 4) : (this->model_data_.memory_size_side / 4);

        // Read current memory block
        read_selected_memory_block();
    }

    void TCBusDeviceComponent::read_memory_update(uint8_t index)
    {
        if (this->virtual_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return;
        }

        if (this->serial_number_ == 0)
        {
            ESP_LOGE(TAG, "Unable to read device memory without a serial number!");
            return;
        }

        if (this->model_ == MODEL_NONE)
        {
            ESP_LOGE(TAG, "Unable to read device memory without a model!");
            return;
        }

        if (this->model_data_.memory_size == 0)
        {
            ESP_LOGE(TAG, "The model %s (Serial: %i) does not support reading memory!", model_to_string(this->model_), this->serial_number_);
            // Call failed callback for unsupported models
            #ifdef USE_READ_MEMORY_FAILED_CALLBACK
            this->read_memory_failed_callback_.call();
            #endif
            return;
        }

        enqueue_flow(FLOW_READ_MEMORY_UPDATE, this, index);
    }

    void TCBusDeviceComponent::execute_read_memory_update(uint8_t index)
    {
        ESP_LOGI(TAG,   "Read memory of %s after update:\n"
                        "  Model: %s\n"
                        "  Serial Number: %i\n"
                        "  Sides: %i",
                        device_group_to_string(this->model_data_.device_group), model_to_string(this->model_), this->serial_number_, this->model_data_.sides);

        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group, 0, 280);
        send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, 0);

        reading_memory_try_ = 0;
        reading_memory_count_ = (index / 4);

        // Read current memory block
        read_selected_memory_block();
    }

    uint8_t TCBusDeviceComponent::get_doorbell_button_memory_index(uint8_t row, uint8_t col, uint8_t side)
    {
        // Model-specific button index mappings
        static const uint8_t dsp_indices[][10] = {
            {}, {20}, {20, 26}, {14, 20, 26}, {8, 14, 20, 26, 32, 38, 44, 50, 56, 62}
        };

        static const uint8_t puk_indices[][10] = {
            {}, {20}, {20, 14}, {26, 20, 14}, {32, 26, 20, 14, 8, 38, 44, 50, 56, 62}
        };

        static const uint8_t pakv2_indices[][8] = {
            {}, {20}, {20, 26}, {14, 20, 26}, {8, 14, 20, 26, 32, 38, 44, 50}
        };

        static const uint8_t pakv3_indices[][8] = {
            {}, {14}, {14, 20}, {8, 14, 20}, {8, 14, 20, 26, 32, 38, 44, 50}
        };

        static const uint8_t pds0x_indices[][3] = {
            {}, {104}, {98, 104}, {98, 104, 110}
        };
        static const uint8_t pds_indices[][3] = {
            {}, {44}
        };
        static const uint8_t pds0x04_indices[][3] = {
            {}, {86}, {92, 86}, {92, 86, 80}
        };

        uint8_t button_rows = get_setting(SETTING_BUTTON_ROWS);

        // Exceptions
        if (this->model_ == MODEL_AS_TCU2)
        {
            if(side == 0)
            {
                if (row >= 1 && row <= 4 && col >= 1 && col <= 4)
                {
                    return 98 - (row - 1) * 24 - (col - 5) * 6;
                }
            }
            return 255;
        }
        
        if (
            this->model_ == MODEL_AS_TCU3 || this->model_ == MODEL_AS_TCU3_EX1 || 
            this->model_ == MODEL_AS_TCU3_EX2 || this->model_ == MODEL_AS_TCU3_EX3 || 
            this->model_ == MODEL_AS_TCU3_EX4 || this->model_ == MODEL_AS_TCU3_EX5 || 
            this->model_ == MODEL_AS_TCU3_EX6 || this->model_ == MODEL_AS_TCU4 || 
            this->model_ == MODEL_AS_TCU4_EX1 || this->model_ == MODEL_AS_TCU4_EX2 || 
            this->model_ == MODEL_AS_TCU4_EX3 || this->model_ == MODEL_AS_TCU4_EX4 || 
            this->model_ == MODEL_AS_TCU4_EX5 || this->model_ == MODEL_AS_TCU4_EX6
        )
        {
            if(side == 0)
            {
                if (row >= 1 && row <= 4 && col >= 1 && col <= 4)
                {
                    return (row - 1) * 24 + (col - 1) * 6 + 8;
                }
            }
            else
            {
                // tcke3 extensions
                if (row >= 1 && row <= 4 && col >= 1 && col <= 8)
                {
                    return (col - 1) * 24 + (row - 1) * 6;
                }
            }
            return 255;
        }
        
        if (this->model_ == MODEL_AS_PES)
        {
            if (col < 1 || col > 2 || row < 1)
            {
                return 255;
            }

            uint8_t offset = 0;
            
            // If button_rows <= 4, use offset to shift rows
            if (button_rows <= 4)
            {
                offset = 4;
            }
            else
            {
                // For button_rows > 4, manually map first 4 rows
                if (row <= 4)
                {
                    static const uint8_t pes_first_4[][2] = {
                        {92, 32}, {86, 26}, {80, 20}, {74, 14}
                    };
                    return pes_first_4[row - 1][col - 1];
                }
            }

            // Map remaining rows (5-10) or all rows (1-6) if offset=4
            static const uint8_t pes_dynamic[][2] = {
                {68, 8}, {98, 38}, {104, 44}, {110, 50}, {116, 56}, {122, 62}
            };

            uint8_t dynamic_row = row + offset - 5;  // Convert to 0-based index
            if (dynamic_row < 6)
            {
                return pes_dynamic[dynamic_row][col - 1];
            }

            return 255;
        }

        if(this->model_ == MODEL_AS_TKM_AS || this->model_ == MODEL_AS_TKM_ASV)
        {
            uint8_t station_length = get_memory_bit(7, 0) ? 1 : 0;
            uint8_t layout_byte = get_memory_bits(112, 6, 7);
            uint8_t max_buttons = (station_length == 0) ? 4 : 8;
            uint8_t total_bits  = max_buttons - 1;
            uint8_t button_index = 1;
            uint8_t width = 1;

            button_rows = 0;
            for (uint8_t i = 0; i <= total_bits && button_index <= max_buttons; i++)
            {
                bool bit = (layout_byte >> i) & 1;

                if (bit == 1)
                {
                    width++;
                }
                else
                {
                    if (row >= button_index && row < button_index + width)
                    {
                        return 8 + (button_index - 1) * 6;
                    }

                    button_index += width;
                    width = 1;
                }
            }

            return 255;
        }

        // Other models
        const uint8_t* indices = nullptr;
        uint8_t max_buttons = 0;

        switch(this->model_)
        {
            case MODEL_AS_PUK_DSP:
                indices = &dsp_indices[0][0];
                max_buttons = 10;
                break;
            case MODEL_AS_PUK:
                indices = &puk_indices[0][0];
                max_buttons = 10;
                break;
            case MODEL_AS_PAKV2:
                indices = &pakv2_indices[0][0];
                max_buttons = 8;
                break;
            case MODEL_AS_PAKV3:
                indices = &pakv3_indices[0][0];
                max_buttons = 8;
                break;
            case MODEL_AS_PDS0X:
                if (get_setting(SETTING_HAS_CODE_LOCK) == 254)
                {
                    indices = &pds_indices[0][0];
                    max_buttons = 1;
                }
                else
                {
                    indices = &pds0x04_indices[0][0];
                    max_buttons = 3;
                }
                break;
            case MODEL_AS_PDS0X04:
                if (get_setting(SETTING_HAS_CODE_LOCK) == 254)
                {
                    indices = &pds_indices[0][0];
                    max_buttons = 1;
                }
                else
                {
                    indices = &pds0x04_indices[0][0];
                    max_buttons = 3;
                }
                break;
            default:
                return 255;
        }

        if (indices && row > 0 && row <= max_buttons)
        {
            uint8_t config = (button_rows <= 3) ? button_rows : 4;
            return indices[config * max_buttons + row - 1];
        }

        return 255;
    }

    DoorbellButtonConfig TCBusDeviceComponent::get_doorbell_button(uint8_t row, uint8_t col, uint8_t side)
    {
        DoorbellButtonConfig button{};

        if (!this->memory_buffer_ready_)
        {
            ESP_LOGE(TAG, "Memory buffer is empty. Please read memory before proceeding!");
            return button;
        }

        if (this->model_ == MODEL_NONE || this->model_data_.device_group != DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGE(TAG, "This device is not an outdoor station or not unsupported!");
            return button;
        }

        uint8_t base_index = get_doorbell_button_memory_index(row, col, side);
        if (base_index == 255)
        {
            ESP_LOGW(TAG, "No memory index for button found!");
            return button;
        }

        // Apply page offset
        size_t offset = get_page_offset(side);
        size_t absolute_index = offset + base_index;

        // First action
        uint8_t primary_action_value = (this->memory_buffer_[absolute_index] >> 4) & 0x0F;
        if (primary_action_value == 0xF || primary_action_value == 0x0 || primary_action_value == 0x1 || primary_action_value == 0x2)
        {
            button.primary_action = static_cast<DoorbellButtonAction>(primary_action_value);
        }
        else
        {
            button.primary_action = DOORBELL_BUTTON_ACTION_NONE;
        }
        button.primary_payload = ((this->memory_buffer_[absolute_index] & 0x0F) << 16) | (this->memory_buffer_[absolute_index + 1] << 8) | this->memory_buffer_[absolute_index + 2];

        // Second action
        uint8_t secondary_action_value = (this->memory_buffer_[absolute_index + 3] >> 4) & 0x0F;
        if (secondary_action_value == 0xF || secondary_action_value == 0x0 || secondary_action_value == 0x1 || secondary_action_value == 0x2)
        {
            button.secondary_action = static_cast<DoorbellButtonAction>(secondary_action_value);
        }
        else
        {
            button.secondary_action = DOORBELL_BUTTON_ACTION_NONE;
        }
        button.secondary_payload = ((this->memory_buffer_[absolute_index + 3] & 0x0F) << 16) | (this->memory_buffer_[absolute_index + 4] << 8) | this->memory_buffer_[absolute_index + 5];

        return button;
    }

    bool TCBusDeviceComponent::update_doorbell_button(uint8_t row, uint8_t col, const DoorbellButtonConfig& button, uint8_t side)
    {
        if (!this->memory_buffer_ready_)
        {
            ESP_LOGE(TAG, "Memory buffer is empty. Please read memory before proceeding!");
            return false;
        }

        if (this->model_ == MODEL_NONE || this->model_data_.device_group != DEVICE_GROUP_OUTDOOR_STATION)
        {
            ESP_LOGE(TAG, "This device is not an outdoor station or not unsupported!");
            return false;
        }

        if (!(this->model_data_.capabilities & CAP_UPDATE_DOORBELL_BUTTON))
        {
            ESP_LOGE(TAG, "For your own safety, the feature is disabled on this device!");
            return false;
        }

        uint8_t base_index = get_doorbell_button_memory_index(row, col, side);
        if (base_index == 255)
        {
            ESP_LOGE(TAG, "No memory index for button found!");
            return false;
        }

        // Apply page offset
        size_t offset = get_page_offset(side);
        size_t absolute_index = offset + base_index;

        // First action (bytes 0-2)
        memory_buffer_[absolute_index] = (button.primary_action << 4) | ((button.primary_payload >> 16) & 0x0F);
        memory_buffer_[absolute_index + 1] = (button.primary_payload >> 8) & 0xFF;
        memory_buffer_[absolute_index + 2] = button.primary_payload & 0xFF;

        // Second action (bytes 3-5)
        memory_buffer_[absolute_index + 3] = (button.secondary_action << 4) | ((button.secondary_payload >> 16) & 0x0F);
        memory_buffer_[absolute_index + 4] = (button.secondary_payload >> 8) & 0xFF;
        memory_buffer_[absolute_index + 5] = button.secondary_payload & 0xFF;

        if(!this->virtual_)
        {
            // Prepare Transmission
            // Select device group
            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group, 0, 280);

            // Select memory page %i of serial number %i
            send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, side);

            // Transfer new button assignments (2 bytes per transmission)
            uint16_t value1 = (memory_buffer_[absolute_index] << 8) | memory_buffer_[absolute_index + 1];
            uint16_t value2 = (memory_buffer_[absolute_index + 2] << 8) | memory_buffer_[absolute_index + 3];
            uint16_t value3 = (memory_buffer_[absolute_index + 4] << 8) | memory_buffer_[absolute_index + 5];

            send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, absolute_index, value1);
            send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, absolute_index + 2, value2);
            send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, absolute_index + 4, value3);

            // Reset
            if (!(this->model_data_.capabilities & CAP_INDIVIDUAL_RESET))
            {
                send_telegram(TELEGRAM_TYPE_RESET);
            }
            else
            {
                send_telegram(TELEGRAM_TYPE_CONTROL_FUNCTION, 0, 0xD7);
            }
        }
        
        return true;
    }

    uint8_t TCBusDeviceComponent::get_memory_byte(uint8_t index, uint8_t side)
    {
        if (!this->memory_buffer_ready_)
        {
            return 0xFF;
        }

        size_t offset = get_page_offset(side);
        size_t absolute_index = offset + index;

        if (absolute_index >= this->memory_buffer_size_)
        {
            return 0xFF;
        }

        return this->memory_buffer_[absolute_index];
    }

    
    uint32_t TCBusDeviceComponent::get_memory_bits(uint8_t index, uint8_t start_bit, uint8_t length, uint8_t side)
    {
        if (this->memory_buffer_ == nullptr)
        {
            return 0xFFFFFFFF;
        }

        if (start_bit > 7 || length == 0)
        {
            return 0xFFFFFFFF;
        }

        size_t offset = get_page_offset(side);

        uint8_t  byte_index = index;
        uint8_t  bits_in_first_byte = start_bit + 1;
        int32_t  bits_remaining = length;
        uint32_t value = 0;

        while(bits_remaining > 0)
        {
            uint8_t bits_this_byte = (bits_remaining < bits_in_first_byte) ? bits_remaining : bits_in_first_byte;

            uint8_t start_bit_in_byte = bits_in_first_byte - 1;
            uint8_t shift_in_byte = start_bit_in_byte - bits_this_byte + 1;
            uint8_t byte_mask = ((1 << bits_this_byte) - 1) << shift_in_byte;

            size_t absolute_index = offset + byte_index;

            if (absolute_index >= this->memory_buffer_size_)
            {
                return 0xFFFFFFFF;
            }

            uint8_t raw = this->memory_buffer_[absolute_index];
            uint8_t field_bits = (raw & byte_mask) >> shift_in_byte;

            value = (value << bits_this_byte) | field_bits;

            bits_remaining -= bits_this_byte;
            byte_index++;
            bits_in_first_byte  = 8;
        }

        return value;
    }

    bool TCBusDeviceComponent::get_memory_bit(uint8_t index, uint8_t start_bit, uint8_t side)
    {
        if (this->memory_buffer_ == nullptr)
        {
            return false;
        }

        if (start_bit > 7)
        {
            return false;
        }

        size_t offset = get_page_offset(side);
        size_t absolute_index = offset + index;

        if (absolute_index >= this->memory_buffer_size_)
        {
            return false;
        }

        return (this->memory_buffer_[absolute_index] >> start_bit) & 0x01;
    }

    uint8_t TCBusDeviceComponent::set_memory_bits(uint8_t index, uint8_t start_bit, uint8_t length, uint32_t value, uint8_t side)
    {
        if (this->memory_buffer_ == nullptr)
        {
            return 0;
        }

        if (start_bit > 7 || length == 0 || length > 32)
        {
            return 0;
        }

        // clamp value to maximum representable by 'length' bits
        if (length < 32 && value > ((1u << length) - 1))
        {
            value = (1u << length) - 1;
        }

        value &= (length < 32) ? ((1u << length) - 1) : 0xFFFFFFFFu;

        // Pre-check: will all bytes fit in the buffer?
        uint8_t bits_in_first_byte = start_bit + 1;
        uint32_t total_bytes_needed = 1 + ((length > bits_in_first_byte) ? ((length - bits_in_first_byte + 7) / 8) : 0);
        size_t offset = get_page_offset(side);

        if (total_bytes_needed > 256 - index)  // prevent wrap-around
        {
            return 0;
        }

        if (offset + index + total_bytes_needed > this->memory_buffer_size_)
        {
            return 0;
        }

        // Walk through bits from MSB of field to LSB
        uint8_t bits_remaining = length;
        uint8_t byte_index = index;
        uint8_t bits_in_cur_byte = bits_in_first_byte;

        while (bits_remaining > 0)
        {
            uint8_t bits_this_byte = (bits_remaining < bits_in_cur_byte) ? bits_remaining : bits_in_cur_byte;

            uint8_t shift_in_value = bits_remaining - bits_this_byte;
            uint8_t field_bits = (value >> shift_in_value) & ((1u << bits_this_byte) - 1u);

            uint8_t start_bit_in_byte = bits_in_cur_byte - 1u;
            uint8_t shift_in_byte = start_bit_in_byte - bits_this_byte + 1u;
            uint8_t byte_mask = ((1u << bits_this_byte) - 1u) << shift_in_byte;

            uint8_t &target = this->memory_buffer_[offset + byte_index];
            target &= ~byte_mask;
            target |= field_bits << shift_in_byte;

            bits_remaining -= bits_this_byte;
            byte_index++;
            bits_in_cur_byte = 8;
        }

        return byte_index;
    }

    bool TCBusDeviceComponent::supports_setting(SettingType type)
    {
        if (this->virtual_)
        {
            return true;
        }

        if (this->model_ == MODEL_NONE)
        {
            return false;
        }

        // Get Setting Cell Data by Model
        const SettingCellData& cellData = getSettingCellData(type, this->model_data_);
        if (!cellData.valid)
        {
            return false;
        }

        return true;
    }

    uint32_t TCBusDeviceComponent::get_setting(SettingType type, bool translate)
    {
        // Get internal state for virtual device first,
        // because it can support all settings independent
        // of model and memory buffer state
        if (this->virtual_)
        {
            if (type == SETTING_ADDRESS)
            {
                return this->address_;
            }
            else if (type == SETTING_PARALLEL_SERIAL_NUMBER)
            {
                return this->parallel_serial_number_;
            }
            else if (type == SETTING_ADDRESS_LOCK)
            {
                return this->address_lock_ ? 1 : 0;
            }
            else if (type == SETTING_AS_ADDRESS_DIVIDER)
            {
                return this->address_divider_;
            }
            else if (type == SETTING_DOOR_READINESS_DURATION)
            {
                return this->door_readiness_duration_;
            }
            else if (type == SETTING_CALL_TIME_DURATION)
            {
                return this->call_time_duration_;
            }
            else if (type == SETTING_DOOR_OPENER_DURATION)
            {
                return this->door_opener_duration_;
            }
            else if (type == SETTING_USE_LONG_DOOR_OPENER_PROTOCOL)
            {
                return this->use_long_door_opener_protocol_ ? 1 : 0;
            }
            else if (type == SETTING_AUTO_ANSWER_CALL)
            {
                return this->auto_answer_call_ ? 1 : 0;
            }
            else if (type == SETTING_CALL_TIME_UNLIMITED)
            {
                return this->call_time_unlimited_ ? 1 : 0;
            }
            else if (type == SETTING_CALLING_REQUIRES_DOOR_READINESS)
            {
                return this->calling_requires_door_readiness_ ? 1 : 0;
            }
            else if (type == SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS)
            {
                return this->door_opener_requires_door_readiness_ ? 1 : 0;
            }
            else if (type == SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL)
            {
                return this->door_opener_requires_active_call_ ? 1 : 0;
            }
        }

        if (!this->memory_buffer_ready_)
        {
            return 0;
        }

        if (this->model_ == MODEL_NONE)
        {
            ESP_LOGVV(TAG, "Model is not specified (NONE). Returning 0.");
            return 0;
        }

        // Get Setting Cell Data by Model
        const SettingCellData& cellData = getSettingCellData(type, this->model_data_);
        if (!cellData.valid)
        {
            ESP_LOGV(TAG, "The setting '%s' is not available for model '%s'.", setting_type_to_string(type), model_to_string(this->model_));
            return 0;
        }

        uint32_t raw_value = get_memory_bits(cellData.index, cellData.start_bit, cellData.length);

        // Translate raw memory value to setting value if necessary
        if(translate)
        {
            return translate_memory_to_setting(type, this->model_, raw_value);
        }
        return raw_value;
    }

    bool TCBusDeviceComponent::update_setting(SettingType type, uint32_t new_value, bool reset, bool translate)
    {
        if (this->virtual_)
        {
            if (type == SETTING_PARALLEL_SERIAL_NUMBER)
            {
                if (new_value > 1000000)
                {
                    ESP_LOGW(TAG, "Invalid Serial Number, reset to 1000000.");
                    new_value = 1000000;
                }

                this->parallel_serial_number_ = new_value;

                // Update Entities
                #ifdef USE_NUMBER
                if (this->parallel_serial_number_number_ != nullptr)
                {
                    this->parallel_serial_number_number_->publish_state(new_value);
                }
                #endif
            }
            else if (type == SETTING_ADDRESS)
            {
                if (new_value > 0xFF)
                {
                    ESP_LOGW(TAG, "Invalid Address, reset to 0.");
                    new_value = 0;
                }
                this->address_ = new_value;

                // Update Entities
                #ifdef USE_NUMBER
                if (this->address_number_ != nullptr)
                {
                    this->address_number_->publish_state(new_value);
                }
                #endif
            }
            else if (type == SETTING_ADDRESS_LOCK)
            {
                this->address_lock_ = (new_value != 0);
            }
            if (type == SETTING_AS_ADDRESS_DIVIDER)
            {
                this->address_divider_ = new_value;
            }
            else if (type == SETTING_DOOR_READINESS_DURATION)
            {
                this->door_readiness_duration_ = new_value;
            }
            else if (type == SETTING_CALL_TIME_DURATION)
            {
                this->call_time_duration_ = new_value;
            }
            else if (type == SETTING_DOOR_OPENER_DURATION)
            {
                this->door_opener_duration_ = new_value;
            }
            else if (type == SETTING_USE_LONG_DOOR_OPENER_PROTOCOL)
            {
                this->use_long_door_opener_protocol_ = (new_value != 0);
            }
            else if (type == SETTING_AUTO_ANSWER_CALL)
            {
                this->auto_answer_call_ = (new_value != 0);
            }
            else if (type == SETTING_CALL_TIME_UNLIMITED)
            {
                this->call_time_unlimited_ = (new_value != 0);
            }
            else if (type == SETTING_CALLING_REQUIRES_DOOR_READINESS)
            {
                this->calling_requires_door_readiness_ = (new_value != 0);
            }
            else if (type == SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS)
            {
                this->door_opener_requires_door_readiness_ = (new_value != 0);
            }
            else if (type == SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL)
            {
                this->door_opener_requires_active_call_ = (new_value != 0);
            }

            this->save_preferences();
        }


        if (!this->memory_buffer_ready_)
        {
            ESP_LOGE(TAG, "Memory buffer is empty. Please read memory before proceeding!");
            return false;
        }

        if (this->serial_number_ == 0)
        {
            ESP_LOGE(TAG, "Unable to update device setting without a serial number!");
            return false;
        }

        if (this->model_ == MODEL_NONE)
        {
            ESP_LOGE(TAG, "Unable to update device setting without a model!");
            return false;
        }

        // Get Setting Cell Data by Model
        const SettingCellData& cellData = getSettingCellData(type, this->model_data_);
        if (!cellData.valid)
        {
            if (!this->virtual_)
            {
                ESP_LOGE(TAG, "Cannot write because setting %s is not available for model %s!", setting_type_to_string(type), model_to_string(this->model_));
            }
            return false;
        }

        // Translate setting value to memory format if necessary
        if(translate)
        {
            new_value = translate_setting_to_memory(type, this->model_, new_value);
        }

        uint8_t byte_index = set_memory_bits(cellData.index, cellData.start_bit, cellData.length, new_value, cellData.page);

        // Update memory of physical device
        if (this->virtual_ == false)
        {
            ESP_LOGI(TAG,   "Write setting to %s:\n"
                            "  Model: %s\n"
                            "  Serial Number: %i\n"
                            "  Setting: %s\n"
                            "  Value: %X",
                            device_group_to_string(this->model_data_.device_group), model_to_string(this->model_), this->serial_number_, setting_type_to_string(type), new_value);

            // Prepare Transmission
            // Select device group
            send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group, 0, 280);

            // Select memory page %i of serial number %i
            send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, cellData.page);

            size_t page_offset = get_page_offset(cellData.page);

            // Determine which 2-byte blocks (even-aligned pairs) are dirty and send one
            // WRITE_MEMORY telegram per block.
            // The protocol always writes two consecutive bytes at an even address.
            uint8_t first_byte = cellData.index;
            uint8_t last_byte  = byte_index - 1;  // inclusive

            // Align down to even address for first block
            uint8_t block_start = first_byte & ~0x01;
            uint8_t block_end   = last_byte  & ~0x01;

            for (uint8_t blk = block_start; blk <= block_end; blk += 2)
            {
                size_t i0 = page_offset + blk;
                size_t i1 = page_offset + blk + 1;

                // bounds check
                if (i1 >= this->memory_buffer_size_)
                {
                    break;
                }

                uint16_t word = (this->memory_buffer_[i0] << 8) | this->memory_buffer_[i1];

                send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, blk, word);
            }

            // Reset
            if (!(this->model_data_.capabilities & CAP_INDIVIDUAL_RESET))
            {
                send_telegram(TELEGRAM_TYPE_RESET);
            }
            else
            {
                send_telegram(TELEGRAM_TYPE_CONTROL_FUNCTION, 0, 0xD7);
            }
        }
        else
        {
            ESP_LOGV(TAG, "Memory buffer updated without sending telegrams.");
        }

        return true;
    }

    bool TCBusDeviceComponent::write_memory()
    {
        if (this->virtual_)
        {
            ESP_LOGW(TAG, "This method is only available for physical devices");
            return false;
        }

        if (!this->memory_buffer_ready_)
        {
            ESP_LOGE(TAG, "Memory buffer is empty! Please read memory first before proceeding.");
            return false;
        }

        if (this->serial_number_ == 0)
        {
            ESP_LOGE(TAG, "Unable to write memory without a serial number!");
            return false;
        }

        if (this->model_ == MODEL_NONE)
        {
            ESP_LOGE(TAG, "Unable to write memory without a model!");
            return false;
        }

        ESP_LOGI(TAG,   "Write complete memory to %s:\n"
                        "  Model: %s\n"
                        "  Serial Number: %i\n"
                        "  Size: %i Bytes",
                        device_group_to_string(this->model_data_.device_group),
                        model_to_string(this->model_),
                        this->serial_number_,
                        this->model_data_.memory_size + (this->model_data_.sides * this->model_data_.memory_size_side));

        // Prepare Transmission
        send_telegram(TELEGRAM_TYPE_SELECT_DEVICE_GROUP, 0, this->model_data_.device_group, 0, 280);

        // Write page by page
        size_t offset = 0;
        for (uint8_t current_page = 0; current_page < this->model_data_.sides + 1; current_page++)
        {
            const size_t page_size = (current_page == 0) ? this->model_data_.memory_size : this->model_data_.memory_size_side;

            const size_t safe_page_size = page_size & ~1;

            // maybe add some delay after telegram
            send_telegram(TELEGRAM_TYPE_SELECT_MEMORY_PAGE, current_page);

            // Transmit Memory
            for (size_t address = 0; address < safe_page_size; address += 2)
            {
                if (offset + address + 1 >= this->memory_buffer_size_)
                {
                    break;
                }

                uint16_t new_value = (this->memory_buffer_[offset + address] << 8) | this->memory_buffer_[offset + address + 1];

                send_telegram(TELEGRAM_TYPE_WRITE_MEMORY, address, new_value);
            }

            offset += page_size;
        }

        // Reset
        if (!(this->model_data_.capabilities & CAP_INDIVIDUAL_RESET))
        {
            send_telegram(TELEGRAM_TYPE_RESET);
        }
        else
        {
            send_telegram(TELEGRAM_TYPE_CONTROL_FUNCTION, 0, 0xD7);
        }

        return true;
    }
}