#pragma once

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"
#include "esphome/components/tc_bus/queue.h"
#include "util.h"

#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome::tc_bus
{
    // Queue configuration - max pending flows across all instances
    static constexpr size_t FLOW_QUEUE_SIZE = 10;

    static const char* FLOW_QUEUE_TAG = "tc_bus_device.flow_queue";

    static constexpr uint32_t CALL_TIMEOUT_MS = 60000;
    static constexpr uint32_t CALL_TIME_LIMIT_MS = 120000;

    enum FlowType
    {
        FLOW_NONE,
        FLOW_READ_MEMORY,
        FLOW_READ_MEMORY_UPDATE,
        FLOW_IDENTIFY_DEVICE
    };

    enum class CallState : uint8_t {
        IDLE = 0,
        OUT_CHECK_DST = 1,
        OUT_RINGING = 2,
        IN_RINGING = 3,
        IN_WAIT_FOR_INIT = 4,
        CONNECTED = 5,
        LINE_BUSY = 6,
    };

#ifdef USE_BINARY_SENSOR
    class TCBusDeviceListener
    {
    public:
        template<typename T> void set_type(T type) { this->type_ = type; }
        template<typename T> void set_address(T address) { this->address_ = address; }
        template<typename T> void set_payload(T payload) { this->payload_ = payload; }

        void set_auto_off(uint16_t auto_off) { this->auto_off_ = auto_off; }

        virtual void turn_on(uint32_t *timer, uint16_t auto_off) {};
        virtual void turn_off(uint32_t *timer) {};

        uint32_t timer_;
        uint16_t auto_off_;

        TemplatableValue<TelegramType> type_{};
        TemplatableValue<uint8_t> address_{};
        TemplatableValue<uint32_t> payload_{};
    };
#endif

    struct TCBusDeviceSettings
    {
        Model model;
        uint32_t serial_number = 0;
        uint32_t parallel_serial_number = 0;
        uint8_t address = 0;
        uint8_t address_divider = 0;
        uint8_t door_readiness_duration = 7;
        uint8_t call_time_duration = 7;
        uint8_t door_opener_duration = 4;
        bool address_lock = false;
        bool force_long_door_opener_protocol = false;
        bool auto_answer_call = false;
        bool calling_requires_door_readiness = false;
        bool door_opener_requires_door_readiness = false;
        bool door_opener_requires_active_call = false;
        bool call_time_unlimited = false;
    };

    class TCBusDeviceComponent : public Component, public TCBusRemoteListener
    {
#ifdef USE_SELECT
        SUB_SELECT(model);
        SUB_SELECT(ringtone_entrance_door_call);
        SUB_SELECT(ringtone_second_entrance_door_call);
        SUB_SELECT(ringtone_floor_call);
        SUB_SELECT(ringtone_internal_call);
#endif
#ifdef USE_NUMBER
        SUB_NUMBER(serial_number);
        SUB_NUMBER(parallel_serial_number);
        SUB_NUMBER(address);

        SUB_NUMBER(volume_handset_door_call);
        SUB_NUMBER(volume_handset_internal_call);
        SUB_NUMBER(volume_ringtone);

        SUB_NUMBER(address_divider);
        SUB_NUMBER(door_readiness_duration);
        SUB_NUMBER(call_time_duration);
        SUB_NUMBER(door_opener_duration);
#endif
#ifdef USE_SWITCH
        SUB_SWITCH(force_long_door_opener_protocol);
        SUB_SWITCH(ringtone_mute);
        SUB_SWITCH(auto_answer_call);
        SUB_SWITCH(calling_requires_door_readiness);
        SUB_SWITCH(door_opener_requires_door_readiness);
        SUB_SWITCH(door_opener_requires_active_call);
        SUB_SWITCH(address_lock);
        SUB_SWITCH(call_time_unlimited);
#endif
#ifdef USE_BUTTON
        SUB_BUTTON(read_memory);
        SUB_BUTTON(identify_device);
#endif

    public:
        const char* TAG = "tc_bus_device";

        void set_internal_id(const std::string &internal_id) { this->internal_id_.assign(internal_id); }
        void set_tc_bus_component(TCBusComponent *bus) { this->tc_bus_ = bus; }
        
        void set_auto_configuration(bool auto_configuration) { this->auto_configuration_ = auto_configuration; }
        void set_virtual(bool virtual_device) { this->virtual_ = virtual_device; }

        void set_serial_number(uint32_t serial_number, bool save = true);
        void set_model(Model model, bool save = true);
        void set_device_group(DeviceGroup device_group) { this->device_group_ = device_group; }

        void set_current_flow(FlowType type) { this->current_flow_ = type; }

        float get_setup_priority() const override { return setup_priority::BUS - 1.0f; }
        void setup() override;
        void dump_config() override;
        void loop() override;

        // Telegram handling
        bool on_receive(TelegramData telegram_data, bool received) override;
        TelegramData send_telegram(TelegramType type, uint8_t address = 0, uint32_t payload = 0, uint32_t wait_duration = 250);

        // Telegram binary listeners
        #ifdef USE_BINARY_SENSOR
        void register_listener(TCBusDeviceListener *listener);
        #endif

        // Call handling
        void call(uint32_t destination, bool internal);
        void answer_call();
        void end_call();
        bool reset_call();

        // Flow Queue Management
        static void process_flow_queue();
        static void enqueue_flow(FlowType type, TCBusDeviceComponent *component, uint8_t index = 0);
        void complete_current_flow();

        // Flow: Memory reading
        void read_memory();
        void read_memory_update(uint8_t index);
        void read_selected_memory_block();
        bool write_memory();
        bool memory_buffer_empty() { return this->memory_buffer_.empty(); }

        // Flow: Identify model
        void identify_device();

        // Bus Device Settings
        bool supports_setting(SettingType type);
        uint32_t get_setting(SettingType type);
        bool update_setting(SettingType type, uint32_t new_value);
        void publish_settings();

        uint8_t get_memory_byte(uint8_t index);

        uint8_t get_doorbell_button_memory_index(uint8_t row, uint8_t col);
        DoorbellButtonConfig get_doorbell_button(uint8_t row);
        DoorbellButtonConfig get_doorbell_button(uint8_t row, uint8_t col);
        bool update_doorbell_button(uint8_t row, DoorbellButtonConfig data);
        bool update_doorbell_button(uint8_t row, uint8_t col, DoorbellButtonConfig data);

        // Preferences
        void save_preferences();

        ESPPreferenceObject &get_pref()
        {
            return this->pref_;
        }

        // Automation Callbacks
        #ifdef USE_READ_MEMORY_COMPLETE_CALLBACK
        void add_read_memory_complete_callback(std::function<void(std::vector<uint8_t>)> &&callback)
        {
            this->read_memory_complete_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_READ_MEMORY_TIMEOUT_CALLBACK
        void add_read_memory_timeout_callback(std::function<void()> &&callback)
        {
            this->read_memory_timeout_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_IDENTIFY_COMPLETE_CALLBACK
        void add_identify_complete_callback(std::function<void(ModelData)> &&callback)
        {
            this->identify_complete_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_IDENTIFY_UNKNOWN_CALLBACK
        void add_identify_unknown_callback(std::function<void()> &&callback)
        {
            this->identify_unknown_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_IDENTIFY_TIMEOUT_CALLBACK
        void add_identify_timeout_callback(std::function<void()> &&callback)
        {
            this->identify_timeout_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_INCOMING_CALL_CALLBACK
        void add_incoming_call_callback(std::function<void(TelegramData)> &&callback)
        {
            this->incoming_call_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_CALL_STARTED_CALLBACK
        void add_call_started_callback(std::function<void(TelegramData)> &&callback)
        {
            this->call_started_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_CALL_ENDED_CALLBACK
        void add_call_ended_callback(std::function<void(TelegramData)> &&callback)
        {
            this->call_ended_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_CALL_FAILED_CALLBACK
        void add_call_failed_callback(std::function<void()> &&callback)
        {
            this->call_failed_callback_.add(std::move(callback));
        }
        #endif

        #ifdef USE_DOOR_OPENER_CALLBACK
        void add_door_opener_callback(std::function<void(bool)> &&callback)
        {
            this->door_opener_callback_.add(std::move(callback));
        }
        #endif
        
    protected:
        // Telegram binary listeners
        #ifdef USE_BINARY_SENSOR
        std::vector<TCBusDeviceListener *> listeners_{};
        #endif

        bool auto_configuration_{false};

        // General device data
        Model model_{MODEL_NONE};
        ModelData model_data_;
        uint32_t serial_number_{0};
        uint32_t parallel_serial_number_{1000000};
        uint8_t address_{0};
        DeviceGroup device_group_{DEVICE_GROUP_INDOOR_STATION};
        bool virtual_{false};
        
        bool address_lock_{false};
        bool force_long_door_opener_protocol_{false};
        bool auto_answer_call_{false};
        bool calling_requires_door_readiness_{false};
        bool door_opener_requires_door_readiness_{false};
        bool door_opener_requires_active_call_{false};
        bool call_time_unlimited_{false};
        uint8_t address_divider_{0};
        uint8_t door_readiness_duration_{7}; // unlimited (0), 8 (1) / ... / 120 (15)
        uint8_t call_time_duration_{7}; // unlimited (0), 8 (1) / ... / 120 (15)
        uint8_t door_opener_duration_{4}; // 0 - 15s

        // Call handling
        bool call_internal_{false};
        bool call_from_parallel_sn_{false};
        uint32_t call_address_{0};
        CallState call_state_{CallState::IDLE};

        // Flow Queue Management
        FlowType current_flow_{FLOW_NONE};

        // Memory reading
        bool memory_mode_{false};
        std::vector<uint8_t> memory_buffer_;
        uint8_t reading_memory_count_{0};
        uint8_t reading_memory_try_{0};
        uint8_t reading_memory_max_{0};

        // Preferences
        ESPPreferenceObject pref_;

        // Automation Callbacks
        #ifdef USE_READ_MEMORY_COMPLETE_CALLBACK
        CallbackManager<void(std::vector<uint8_t>)> read_memory_complete_callback_{};
        #endif
        #ifdef USE_READ_MEMORY_TIMEOUT_CALLBACK
        CallbackManager<void()> read_memory_timeout_callback_{};
        #endif
        #ifdef USE_IDENTIFY_COMPLETE_CALLBACK
        CallbackManager<void(ModelData)> identify_complete_callback_{};
        #endif
        #ifdef USE_IDENTIFY_UNKNOWN_CALLBACK
        CallbackManager<void()> identify_unknown_callback_{};
        #endif
        #ifdef USE_IDENTIFY_TIMEOUT_CALLBACK
        CallbackManager<void()> identify_timeout_callback_{};
        #endif

        #ifdef USE_INCOMING_CALL_CALLBACK
        CallbackManager<void(TelegramData)> incoming_call_callback_{};
        #endif
        #ifdef USE_CALL_STARTED_CALLBACK
        CallbackManager<void(TelegramData)> call_started_callback_{};
        #endif
        #ifdef USE_CALL_ENDED_CALLBACK
        CallbackManager<void(TelegramData)> call_ended_callback_{};
        #endif
        #ifdef USE_CALL_FAILED_CALLBACK
        CallbackManager<void()> call_failed_callback_{};
        #endif
        #ifdef USE_DOOR_OPENER_CALLBACK
        CallbackManager<void(bool)> door_opener_callback_{};
        #endif

        // Misc
        std::string internal_id_;
        TCBusComponent *tc_bus_{nullptr};

        HighFrequencyLoopRequester high_freq_;

    private:
        // Flow Queue Management
        void execute_read_memory();
        void execute_read_memory_update(uint8_t index);
        void execute_identify_device();
    };

    // Flow Queue
    struct TCBusDeviceFlowQueueItem
    {
        FlowType type;
        TCBusDeviceComponent *component;
        uint8_t index; // Only used for READ_MEMORY_UPDATE
    };

    static bool s_running_flow = false;
    static uint32_t s_last_flow_completion_time = 0;
    static FixedQueue<TCBusDeviceFlowQueueItem, FLOW_QUEUE_SIZE> s_flow_queue;
}