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

namespace esphome
{
    namespace tc_bus
    {
        #ifdef USE_BINARY_SENSOR
        class TCBusListener
        {
            public:
                void set_command_lambda(std::function<optional<uint32_t>()> &&f) { this->command_lambda_ = f; }
                void set_command(uint32_t command) { this->command_ = command; }

                void set_address_lambda(std::function<optional<uint8_t>()> &&f) { this->address_lambda_ = f; }
                void set_address(uint8_t address) { this->address_ = address; }
                
                void set_payload_lambda(std::function<optional<uint32_t>()> &&f) { this->payload_lambda_ = f; }
                void set_payload(uint32_t payload) { this->payload_ = payload; }
                
                void set_command_type(CommandType type) { this->type_ = type; }
                
                void set_serial_number(uint32_t serial_number) { this->serial_number_ = serial_number; }
                
                void set_auto_off(uint16_t auto_off) { this->auto_off_ = auto_off; }

                virtual void turn_on(uint32_t *timer, uint16_t auto_off) {};
                virtual void turn_off(uint32_t *timer) {};

                uint32_t timer_;
                uint16_t auto_off_;
                
                optional<uint32_t> serial_number_;
                optional<CommandType> type_;

                optional<uint32_t> command_;
                optional<std::function<optional<uint32_t>()>> command_lambda_;

                optional<uint8_t> address_;
                optional<std::function<optional<uint8_t>()>> address_lambda_;

                optional<uint32_t> payload_;
                optional<std::function<optional<uint32_t>()>> payload_lambda_;
        };
        #endif

        struct TCBusSettings
        {
            Model model;
            uint32_t serial_number;
        };

        class TCBusComponent : public Component, public remote_base::RemoteReceiverListener
        {
            #ifdef USE_TEXT_SENSOR
            SUB_TEXT_SENSOR(bus_command)
            SUB_TEXT_SENSOR(hardware_version)
            #endif
            #ifdef USE_SELECT
            SUB_SELECT(model)
            SUB_SELECT(ringtone_entrance_door_call)
            SUB_SELECT(ringtone_second_entrance_door_call)
            SUB_SELECT(ringtone_floor_call)
            SUB_SELECT(ringtone_internal_call)
            #endif
            #ifdef USE_NUMBER
            SUB_NUMBER(serial_number)
            SUB_NUMBER(volume_handset_door_call)
            SUB_NUMBER(volume_handset_internal_call)
            SUB_NUMBER(volume_ringtone)
            #endif

            public:
                void set_tx(remote_transmitter::RemoteTransmitterComponent *tx) { this->tx_ = tx; }
                void set_rx(remote_receiver::RemoteReceiverComponent *rx) { this->rx_ = rx; }

                void set_event(const char *event) { this->event_ = event; }
                void set_model(Model model) { this->model_ = model; }
                void set_serial_number(uint32_t serial_number) { this->serial_number_ = serial_number; }

                void setup() override;
                void dump_config() override;
                void loop() override;
                bool on_receive(remote_base::RemoteReceiveData data) override;

                #ifdef USE_BINARY_SENSOR
                void register_listener(TCBusListener *listener);
                #endif

                void on_command(CommandData cmd_data);
                void on_acknowledge(uint8_t type);

                void send_command(uint32_t command);
                void send_command(uint32_t command, bool is_long);
                void send_command(CommandType type, uint8_t address = 0, uint32_t payload = 0, uint32_t serial_number = 0);

                void set_programming_mode(bool enabled);
                void request_version(uint32_t serial_number);

                void read_memory(uint32_t serial_number, Model model = MODEL_NONE);
                void request_memory_blocks(uint8_t start_address);
                bool write_memory(uint32_t serial_number = 0, Model model = MODEL_NONE);

                uint8_t get_setting(SettingType type, Model model = MODEL_NONE);
                bool update_setting(SettingType type, uint8_t new_value, uint32_t serial_number = 0, Model model = MODEL_NONE);

                void on_command(CommandData cmd_data);

                void publish_command(CommandData cmd_data, bool fire_events);

                void publish_settings();
                void save_settings();

                void add_received_command_callback(std::function<void(CommandData)> &&callback);
                CallbackManager<void(CommandData)> received_command_callback_{};
                void add_read_memory_complete_callback(std::function<void(std::vector<uint8_t>)> &&callback);
                CallbackManager<void(std::vector<uint8_t>)> read_memory_complete_callback_{};
                void add_read_memory_timeout_callback(std::function<void()> &&callback);
                CallbackManager<void()> read_memory_timeout_callback_{};
                void add_identify_complete_callback(std::function<void(ModelData)> &&callback);
                CallbackManager<void(ModelData)> identify_complete_callback_{};
                void add_identify_timeout_callback(std::function<void()> &&callback);
                CallbackManager<void()> identify_timeout_callback_{};

                static constexpr uint32_t BIT_0_MIN = 1000;
                static constexpr uint32_t BIT_0_MAX = 2999;
                static constexpr uint32_t BIT_1_MIN = 3000;
                static constexpr uint32_t BIT_1_MAX = 4999;
                static constexpr uint32_t START_MIN = 5000;
                static constexpr uint32_t START_MAX = 6999;

                ESPPreferenceObject &get_pref() {
                    return this->pref_;
                }

            protected:
                remote_transmitter::RemoteTransmitterComponent *tx_{nullptr};
                remote_receiver::RemoteReceiverComponent *rx_{nullptr};

                const char* event_;

                Model model_;
                uint32_t serial_number_;

                #ifdef USE_BINARY_SENSOR
                std::vector<TCBusListener *> listeners_{};
                #endif

                std::string hardware_version_str_ = "Generic";

                bool programming_mode_ = false;
                bool identify_model_ = false;

                bool reading_memory_ = false;
                uint8_t reading_memory_count_ = 0;
                uint8_t reading_memory_max_ = 0;
                uint32_t reading_memory_serial_number_ = 0;
                std::vector<uint8_t> memory_buffer_;

                uint32_t last_sent_command_ = 0;

                ESPPreferenceObject pref_;
        };

    }  // namespace tc_bus

}  // namespace esphome