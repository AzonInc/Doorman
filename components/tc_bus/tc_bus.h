#pragma once

#include "protocol.h"

#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#include "esphome/components/remote_transmitter/remote_transmitter.h"
#include "esphome/components/remote_receiver/remote_receiver.h"

#ifdef USE_API
#include "esphome/components/api/custom_api_device.h"
#endif

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

#include <queue>

namespace esphome
{
    namespace tc_bus
    {
        static const char *const TAG = "tc_bus";
        static uint32_t global_tc_bus_id = 1911044085ULL;

        struct TCBusTelegramQueueItem
        {
            TelegramData telegram_data;
            uint32_t wait_duration;
        };

#ifdef USE_BINARY_SENSOR
        class TCBusListener
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

        class TCBusRemoteListener {
            public:
                virtual bool on_receive(TelegramData data, bool received) = 0;
        };

        struct TCBusSettings
        {
            Model model;
            uint32_t serial_number;
            bool force_long_door_opener;
        };

        class TCBusComponent :
            public Component,
            public remote_base::RemoteReceiverListener
#ifdef USE_API
          , public api::CustomAPIDevice
#endif
        {
#ifdef USE_TEXT_SENSOR
            SUB_TEXT_SENSOR(bus_telegram);
            SUB_TEXT_SENSOR(hardware_version);
#endif
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
            void set_tx(remote_transmitter::RemoteTransmitterComponent *tx) { this->tx_ = tx; }
            void set_rx(remote_receiver::RemoteReceiverComponent *rx) { this->rx_ = rx; }

            void set_event(const char *event) { this->event_ = event; }
            void set_model(Model model) { this->model_ = model; }
            void set_serial_number(uint32_t serial_number) { this->serial_number_ = serial_number; }
            void set_force_long_door_opener(bool force_long_door_opener) { this->force_long_door_opener_ = force_long_door_opener; }

            void setup() override;
            void dump_config() override;
            void loop() override;
            bool on_receive(remote_base::RemoteReceiveData data) override;

            void register_remote_listener(TCBusRemoteListener*listener) { this->remote_listeners_.push_back(listener); }

#ifdef USE_BINARY_SENSOR
            void register_listener(TCBusListener *listener);
#endif

            // Telegram handling
            static constexpr uint32_t BUS_CMD_START_MS = 5985;
            static constexpr uint32_t BUS_ACK_START_MS = 6200;

            static constexpr uint32_t BUS_ONE_BIT_MS = 4000;
            static constexpr uint32_t BUS_ZERO_BIT_MS = 2000;

            static constexpr uint32_t BIT_0_MIN = 1000;
            static constexpr uint32_t BIT_0_MAX = 2999;

            static constexpr uint32_t BIT_1_MIN = 3000;
            static constexpr uint32_t BIT_1_MAX = 4999;

            static constexpr uint32_t START_CMD = 5800;
            static constexpr uint32_t START_RSP = 6090;

            static constexpr uint32_t START_MAX = 6900;

            void send_telegram(uint32_t telegram, uint32_t wait_duration = 200);
            void send_telegram(uint32_t telegram, bool is_long, uint32_t wait_duration = 200);
            void send_telegram(TelegramType type, uint8_t address = 0, uint32_t payload = 0, uint32_t serial_number = 0, uint32_t wait_duration = 200);
            void send_telegram(TelegramData telegram_data, uint32_t wait_duration = 200);

            void process_telegram_queue();
            void transmit_telegram(TelegramData telegram_data);
            void received_telegram(TelegramData telegram_data, bool received = true);

            // Memory reading
            void read_memory(uint32_t serial_number, Model model = MODEL_NONE);
            void read_memory_block();
            bool write_memory(uint32_t serial_number = 0, Model model = MODEL_NONE);

            bool supports_setting(SettingType type, Model model = MODEL_NONE);
            uint8_t get_setting(SettingType type, Model model = MODEL_NONE);
            bool update_setting(SettingType type, uint8_t new_value, uint32_t serial_number = 0, Model model = MODEL_NONE);

            void publish_settings();
            void save_settings();

            // Automation Actions
            void add_received_telegram_callback(std::function<void(TelegramData)> &&callback)
            {
                this->received_telegram_callback_.add(std::move(callback));
            }

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

            // Misc
            void set_programming_mode(bool enabled);
            void request_version(uint32_t serial_number, uint8_t device_group);

            ESPPreferenceObject &get_pref()
            {
                return this->pref_;
            }

        protected:
            // Telegram handling
            remote_transmitter::RemoteTransmitterComponent *tx_{nullptr};
            remote_receiver::RemoteReceiverComponent *rx_{nullptr};

            std::queue<TCBusTelegramQueueItem> telegram_queue_;
            uint32_t last_telegram_time_ = 0;
            int32_t last_sent_telegram_ = -1;

            std::vector<TCBusRemoteListener *> remote_listeners_;

#ifdef USE_BINARY_SENSOR
            std::vector<TCBusListener *> listeners_{};
#endif

            // Indoor station data
            Model model_;
            uint32_t serial_number_;
            bool force_long_door_opener_;

            // Automation Actions
            CallbackManager<void(TelegramData)> received_telegram_callback_{};
            CallbackManager<void(std::vector<uint8_t>)> read_memory_complete_callback_{};
            CallbackManager<void()> read_memory_timeout_callback_{};
            CallbackManager<void(ModelData)> identify_complete_callback_{};
            CallbackManager<void()> identify_unknown_callback_{};
            CallbackManager<void()> identify_timeout_callback_{};

            // Memory reading
            std::vector<uint8_t> memory_buffer_;
            bool read_memory_flow_ = false;
            bool wait_for_memory_block_telegram_ = false;
            uint8_t reading_memory_count_ = 0;
            uint8_t reading_memory_max_ = 0;
            uint32_t reading_memory_serial_number_ = 0;

            // Identification
            bool identify_model_flow_ = false;
            bool wait_for_identification_telegram_ = false;

            // Misc
            const char *event_;
            std::string hardware_version_str_ = "Generic";
            bool programming_mode_ = false;
            uint8_t selected_device_group_ = 2;

            ESPPreferenceObject pref_;
        };

        static TCBusComponent *global_tc_bus = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    } // namespace tc_bus

} // namespace esphome