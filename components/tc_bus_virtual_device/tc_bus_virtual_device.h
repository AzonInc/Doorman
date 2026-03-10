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
    static constexpr uint16_t CALL_TIMEOUT_MS = 60000;

    enum class CallState : uint8_t {
        IDLE = 0,
        OUT_CHECK_DST = 1,
        OUT_RINGING = 2,
        IN_RINGING = 3,
        IN_WAIT_FOR_INIT = 4,
        CONNECTED = 5,
        LINE_BUSY = 6,
    };

    struct TCBusVirtualDeviceSettings
    {
        uint32_t serial_number = 0;
        uint8_t address = 0;
        uint8_t address_divider = 0;
        uint8_t door_readiness_duration = 7;
        uint8_t call_time_duration = 7;
        uint8_t door_opener_duration = 4;
        bool force_long_door_opener_protocol = false;
        bool auto_answer_call = false;
    };

    class TCBusVirtualDeviceComponent : public Component, public TCBusRemoteListener
    {
#ifdef USE_NUMBER
        SUB_NUMBER(serial_number);
        SUB_NUMBER(address);
        SUB_NUMBER(address_divider);
        SUB_NUMBER(door_readiness_duration);
        SUB_NUMBER(call_time_duration);
        SUB_NUMBER(door_opener_duration);
#endif
#ifdef USE_SWITCH
        SUB_SWITCH(force_long_door_opener_protocol);
        SUB_SWITCH(auto_answer_call);
#endif

    public:
        const char* TAG = "tc_bus_virtual_device";

        void set_internal_id(const std::string &internal_id) { this->internal_id_.assign(internal_id); }
        void set_tc_bus_component(TCBusComponent *bus) { this->tc_bus_ = bus; }
        void set_device_group(VirtualDeviceGroup device_group) { this->device_group_ = device_group; }
        void set_serial_number(uint32_t serial_number, bool save = true);
        void set_address(uint8_t address, bool save = true);
        
        void set_force_long_door_opener_protocol(bool force_long_door_opener_protocol) { this->force_long_door_opener_protocol_ = force_long_door_opener_protocol; }
        void set_auto_answer_call(bool auto_answer_call) { this->auto_answer_call_ = auto_answer_call; }
        void set_address_divider(uint8_t address_divider) { this->address_divider_ = address_divider; }
        void set_door_readiness_duration(uint8_t door_readiness_duration) { this->door_readiness_duration_ = door_readiness_duration; }
        void set_call_time_duration(uint8_t call_time_duration) { this->call_time_duration_ = call_time_duration; }
        void set_door_opener_duration(uint8_t door_opener_duration) { this->door_opener_duration_ = door_opener_duration; }

        float get_setup_priority() const override { return setup_priority::BUS - 1.0f; }
        void setup() override;
        void dump_config() override;
        void loop() override;

        // Telegram handling
        bool on_receive(TelegramData telegram_data, bool received) override;
        TelegramData send_telegram(TelegramType type, uint8_t address = 0, uint32_t payload = 0, uint32_t wait_duration = 250);

        void call(uint32_t destination, bool internal);
        void answer_call();
        void end_call();
        bool reset_call();

        // Preferences
        void save_preferences();

        ESPPreferenceObject &get_pref()
        {
            return this->pref_;
        }

        // Automation Callbacks
        void add_incoming_call_callback(std::function<void(TelegramData)> &&callback)
        {
            this->incoming_call_callback_.add(std::move(callback));
        }

        void add_call_started_callback(std::function<void(TelegramData)> &&callback)
        {
            this->call_started_callback_.add(std::move(callback));
        }

        void add_call_ended_callback(std::function<void(TelegramData)> &&callback)
        {
            this->call_ended_callback_.add(std::move(callback));
        }

        void add_call_failed_callback(std::function<void()> &&callback)
        {
            this->call_failed_callback_.add(std::move(callback));
        }

    protected:
        uint32_t serial_number_;
        uint8_t address_;
        VirtualDeviceGroup device_group_;

        std::vector<uint8_t> memory_ = std::vector<uint8_t>(128, 0);

        bool force_long_door_opener_protocol_;
        bool auto_answer_call_;
        uint8_t address_divider_;
        uint8_t door_readiness_duration_; // unlimited (0), 8 (1) / ... / 120 (15)
        uint8_t call_time_duration_; // unlimited (0), 8 (1) / ... / 120 (15)
        uint8_t door_opener_duration_; // 0 - 15s

        bool door_readiness_active_;

        bool call_internal_;
        uint32_t call_address_;
        CallState call_state_;

        bool memory_mode_;

        // Preferences
        ESPPreferenceObject pref_;

        // Automation Callbacks
        CallbackManager<void(TelegramData)> incoming_call_callback_{};
        CallbackManager<void(TelegramData)> call_started_callback_{};
        CallbackManager<void(TelegramData)> call_ended_callback_{};
        CallbackManager<void()> call_failed_callback_{};

        // Misc
        std::string internal_id_;
        TCBusComponent *tc_bus_{nullptr};

        HighFrequencyLoopRequester high_freq_;
    };
}