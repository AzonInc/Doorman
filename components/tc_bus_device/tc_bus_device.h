#pragma once

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"
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

namespace esphome
{
    namespace tc_bus
    {
        static bool running_flow_ = false;

#ifdef USE_BINARY_SENSOR
        class TCBusDeviceListener
        {
        public:
            void set_telegram_lambda(std::function<optional<uint32_t>()> &&f) { this->telegram_lambda_ = f; }
            void set_telegram(uint32_t telegram) { this->telegram_ = telegram; }

            void set_address_lambda(std::function<optional<uint8_t>()> &&f) { this->address_lambda_ = f; }
            void set_address(uint8_t address) { this->address_ = address; }

            void set_payload_lambda(std::function<optional<uint32_t>()> &&f) { this->payload_lambda_ = f; }
            void set_payload(uint32_t payload) { this->payload_ = payload; }

            void set_telegram_type(TelegramType type) { this->type_ = type; }

            void set_serial_number(uint32_t serial_number) { this->serial_number_ = serial_number; }

            void set_auto_off(uint16_t auto_off) { this->auto_off_ = auto_off; }

            virtual void turn_on(uint32_t *timer, uint16_t auto_off) {};
            virtual void turn_off(uint32_t *timer) {};

            uint32_t timer_;
            uint16_t auto_off_;

            optional<uint32_t> serial_number_;
            optional<TelegramType> type_;

            optional<uint32_t> telegram_;
            optional<std::function<optional<uint32_t>()>> telegram_lambda_;

            optional<uint8_t> address_;
            optional<std::function<optional<uint8_t>()>> address_lambda_;

            optional<uint32_t> payload_;
            optional<std::function<optional<uint32_t>()>> payload_lambda_;
        };
#endif

        struct TCBusDeviceSettings
        {
            Model model;
            uint32_t serial_number;
            bool force_long_door_opener;
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
            SUB_NUMBER(volume_handset_door_call);
            SUB_NUMBER(volume_handset_internal_call);
            SUB_NUMBER(volume_ringtone);
#endif
#ifdef USE_SWITCH
            SUB_SWITCH(force_long_door_opener);
#endif

        public:
            const char* TAG = "tc_bus_device";

            void set_internal_id(const std::string &internal_id) { this->internal_id_.assign(internal_id); }
            void set_tc_bus_component(TCBusComponent *bus) { this->tc_bus_ = bus; }
            void set_device_group(DeviceGroup device_group) { this->device_group_ = device_group; }
            void set_serial_number(uint32_t serial_number) { this->serial_number_ = serial_number; }
            void set_force_long_door_opener(bool force_long_door_opener) { this->force_long_door_opener_ = force_long_door_opener; }
            
            void set_model(Model model, bool save = true);

            void setup() override;
            void dump_config() override;
            void loop() override;

            // Telegram handling
            bool on_receive(TelegramData telegram_data, bool received) override;
            void send_telegram(TelegramType type, uint8_t address = 0, uint32_t payload = 0, uint32_t wait_duration = 200);

            // Telegram binary listeners
            #ifdef USE_BINARY_SENSOR
            void register_listener(TCBusDeviceListener *listener);
            #endif

            // Memory reading
            void read_memory();
            void read_memory_block();
            bool write_memory();

            // Bus Device Settings
            bool supports_setting(SettingType type);
            uint8_t get_setting(SettingType type);
            bool update_setting(SettingType type, uint8_t new_value);
            void publish_settings();

            // Identification
            void request_version();

            // Preferences
            void save_preferences();

            ESPPreferenceObject &get_pref()
            {
                return this->pref_;
            }

            // Automation Callbacks
            void add_read_memory_complete_callback(std::function<void(std::vector<uint8_t>)> &&callback)
            {
                this->read_memory_complete_callback_.add(std::move(callback));
            }

            void add_read_memory_timeout_callback(std::function<void()> &&callback)
            {
                this->read_memory_timeout_callback_.add(std::move(callback));
            }

            void add_identify_complete_callback(std::function<void(ModelData)> &&callback)
            {
                this->identify_complete_callback_.add(std::move(callback));
            }

            void add_identify_unknown_callback(std::function<void()> &&callback)
            {
                this->identify_unknown_callback_.add(std::move(callback));
            }

            void add_identify_timeout_callback(std::function<void()> &&callback)
            {
                this->identify_timeout_callback_.add(std::move(callback));
            }
            
        protected:
            // Telegram binary listeners
            #ifdef USE_BINARY_SENSOR
            std::vector<TCBusDeviceListener *> listeners_{};
            #endif

            // Indoor station data
            Model model_;
            ModelData model_data_;
            uint32_t serial_number_;
            DeviceGroup device_group_;
            bool force_long_door_opener_;

            // Memory reading
            bool read_memory_flow_ = false;
            std::vector<uint8_t> memory_buffer_;
            uint8_t reading_memory_count_ = 0;
            uint8_t reading_memory_max_ = 0;

            // Identification
            bool identify_model_flow_ = false;

            // Preferences
            ESPPreferenceObject pref_;

            // Automation Callbacks
            CallbackManager<void(std::vector<uint8_t>)> read_memory_complete_callback_{};
            CallbackManager<void()> read_memory_timeout_callback_{};
            CallbackManager<void(ModelData)> identify_complete_callback_{};
            CallbackManager<void()> identify_unknown_callback_{};
            CallbackManager<void()> identify_timeout_callback_{};

            // Misc
            std::string internal_id_;
            TCBusComponent *tc_bus_{nullptr};
        };

    } // namespace tc_bus

} // namespace esphome