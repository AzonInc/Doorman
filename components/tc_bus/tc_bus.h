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

            void set_serial_number_lambda(std::function<optional<uint32_t>()> &&f) { this->serial_number_lambda_ = f; }
            void set_serial_number(uint32_t serial_number) { this->serial_number_ = serial_number; }

            void set_auto_off(uint16_t auto_off) { this->auto_off_ = auto_off; }

            virtual void turn_on(uint32_t *timer, uint16_t auto_off) {};
            virtual void turn_off(uint32_t *timer) {};

            uint32_t timer_;
            uint16_t auto_off_;

            optional<uint32_t> serial_number_;
            optional<std::function<optional<uint32_t>()>> serial_number_lambda_;

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

        public:
            void set_tx(remote_transmitter::RemoteTransmitterComponent *tx) { this->tx_ = tx; }
            void set_rx(remote_receiver::RemoteReceiverComponent *rx) { this->rx_ = rx; }

            void set_event(const char *event) { this->event_ = event; }

            void setup() override;
            void dump_config() override;
            void loop() override;
            bool on_receive(remote_base::RemoteReceiveData data) override;

            void register_remote_listener(TCBusRemoteListener*listener) { this->remote_listeners_.push_back(listener); }

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

            // Telegram binary listeners
            #ifdef USE_BINARY_SENSOR
            void register_listener(TCBusListener *listener);
            #endif

            // Automation Callbacks
            void add_received_telegram_callback(std::function<void(TelegramData)> &&callback)
            {
                this->received_telegram_callback_.add(std::move(callback));
            }

            // Misc
            void set_programming_mode(bool enabled);
            uint8_t selected_device_group_ = 2;

        protected:
            // Telegram handling
            remote_transmitter::RemoteTransmitterComponent *tx_{nullptr};
            remote_receiver::RemoteReceiverComponent *rx_{nullptr};

            std::vector<TCBusRemoteListener *> remote_listeners_;
            
            std::queue<TCBusTelegramQueueItem> telegram_queue_;
            uint32_t last_telegram_time_ = 0;
            int32_t last_sent_telegram_ = -1;

            // Telegram binary listeners
            #ifdef USE_BINARY_SENSOR
            std::vector<TCBusListener *> listeners_{};
            #endif

            // Automation Callbacks
            CallbackManager<void(TelegramData)> received_telegram_callback_{};

            // Misc
            const char *event_;
            std::string hardware_version_str_ = "Generic";
            bool programming_mode_ = false;
            bool wait_for_data_telegram_ = false;
        };

        static TCBusComponent *global_tc_bus = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    } // namespace tc_bus

} // namespace esphome