#pragma once

#include "protocol.h"
#include "queue.h"

#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

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
#ifdef USE_LOCK
#include "esphome/components/lock/lock.h"
#endif

#include <queue>

namespace esphome::tc_bus
{
    static const char *const TAG = "tc_bus";
    static uint32_t global_tc_bus_id = 1911044085ULL;

    struct TCBusTelegramQueueItem
    {
        TelegramData telegram_data;
        uint32_t wait_duration;
        uint8_t sender_listener_id;
    };
    
    static constexpr uint16_t PULSE_FILTER              = 1500;
    static constexpr uint16_t PULSE_START               = 6000;
    static constexpr uint16_t PULSE_START_MIN_US        = 5700;
    static constexpr uint16_t PULSE_START_MAX_US        = 7200;

    static constexpr uint16_t PULSE_BIT_0               = 2000;
    static constexpr uint16_t PULSE_BIT_0_MIN_US        = 1850;
    static constexpr uint16_t PULSE_BIT_0_MAX_US        = 2800;

    static constexpr uint16_t PULSE_BIT_1               = 4000;
    static constexpr uint16_t PULSE_BIT_1_MIN_US        = 3850;
    static constexpr uint16_t PULSE_BIT_1_MAX_US        = 4800;

    static constexpr uint32_t RETRANSMISSION_GAP_US     = 19900;
    static constexpr uint16_t RETRANSMISSION_GAP_MIN_US = 19900;
    static constexpr uint16_t RETRANSMISSION_GAP_MAX_US = 22800;

    static constexpr uint16_t ACK_TIMEOUT_US            = 7000;

    enum class TelegramSource : uint8_t
    {
        BUS_RECEIVED = 0,
        LOCAL_SENT   = 1,
        PEER_SENT    = 2,
    };

    class TCBusBaseListener
    {
        public:
            virtual ~TCBusBaseListener() = default;

            template<typename T> void set_serial_number(T serial_number) { this->serial_number_ = serial_number; }
            template<typename T> void set_telegram(T telegram) { this->telegram_ = telegram; }
            template<typename T> void set_type(T type) { this->type_ = type; }
            template<typename T> void set_address(T address) { this->address_ = address; }
            template<typename T> void set_payload(T payload) { this->payload_ = payload; }

            void set_auto_reset(uint16_t auto_reset) { this->auto_reset_ = auto_reset; }

            uint32_t timer_{0};
            uint16_t auto_reset_{0};
            
            TemplatableValue<uint32_t> serial_number_{};
            TemplatableValue<TelegramType> type_{};
            TemplatableValue<uint32_t> telegram_{};
            TemplatableValue<uint8_t> address_{};
            TemplatableValue<uint32_t> payload_{};
    };



#ifdef USE_BINARY_SENSOR
    class TCBusBinarySensorListener : public TCBusBaseListener
    {
        public:
            virtual void turn_on(uint32_t *timer, uint16_t auto_reset) {};
            virtual void turn_off(uint32_t *timer) {};
    };
#endif


#ifdef USE_LOCK
    class TCBusLockListener : public TCBusBaseListener
    {
        public:
            virtual void setup() {};
            virtual void control(const lock::LockCall& call) {};

            virtual void unlock(uint32_t *timer, uint16_t auto_reset) {};
            virtual void lock(uint32_t *timer) {};
            virtual void open() {};
    };
#endif

    class TCBusRemoteListener
    {
        public:
            virtual bool on_receive(TelegramData data, TelegramSource source) = 0;
            void set_listener_id(uint8_t listener_id) { this->listener_id_ = listener_id; }
            uint8_t get_listener_id() { return this->listener_id_; }

        protected:
            uint8_t listener_id_{0};
    };

    struct PrioritizedListener
    {
        TCBusRemoteListener *listener;
        uint8_t priority;
    };

    struct TCBusComponentStore
    {
        static void gpio_intr(TCBusComponentStore *arg);
        volatile uint32_t last_bit_change{0};
        volatile uint32_t last_telegram_raw{0};
        volatile bool retransmission_pending{false};
        volatile bool sending{false};
        volatile bool expect_echo{false};
        volatile bool expect_echo_isr{false};
        ISRInternalGPIOPin rx_pin;

        volatile uint32_t debug_buffer[255];
        volatile uint8_t debug_buffer_index{0};
    };

    struct TCBusSettings
    {
        uint8_t entrance_address{0};
        uint8_t second_entrance_address{0};
    };
    
    class TCBusComponent: public Component
    {
#ifdef USE_TEXT_SENSOR
        SUB_TEXT_SENSOR(bus_telegram);
#endif
#ifdef USE_NUMBER
        SUB_NUMBER(entrance_address);
        SUB_NUMBER(second_entrance_address);
#endif
#ifdef USE_BINARY_SENSOR
        SUB_BINARY_SENSOR(door_readiness);
#endif
#ifdef USE_BUTTON
        SUB_BUTTON(system_discovery);
#endif

    public:
        void set_rx_pin(InternalGPIOPin *pin) { this->rx_pin_ = pin; }
        void set_tx_pin(InternalGPIOPin *pin) { this->tx_pin_ = pin; }

        void set_entrance_address(uint32_t address) { this->entrance_address_ = address; }
        void set_second_entrance_address(uint32_t address) { this->second_entrance_address_ = address; }

        float get_setup_priority() const override { return setup_priority::BUS; }
        void setup() override;
        void dump_config() override;
        void loop() override;

        void register_remote_listener(TCBusRemoteListener *listener, uint8_t priority = 0)
        {
            listener->set_listener_id(++remote_listener_count_);
            PrioritizedListener entry{listener, priority};
            auto it = std::lower_bound(remote_listeners_.begin(), remote_listeners_.end(), entry,
                [](const PrioritizedListener &a, const PrioritizedListener &b) {
                    return a.priority > b.priority;
                });
            remote_listeners_.insert(it, entry);
        }

        TelegramData send_telegram(uint32_t telegram, uint32_t wait_duration = 250);
        TelegramData send_telegram(uint32_t telegram, bool is_long, uint32_t wait_duration = 250);
        TelegramData send_telegram(TelegramType type, uint8_t address = 0, uint32_t payload = 0, uint32_t serial_number = 0, uint32_t wait_duration = 250);
        TelegramData send_telegram(TelegramData telegram_data, uint32_t wait_duration = 250, uint8_t sender_listener_id = 0);
        TelegramData send_telegram(TelegramType type, uint8_t address, uint32_t payload, uint32_t serial_number, uint32_t wait_duration, uint8_t sender_listener_id);

        void process_telegram_queue();
        void transmit_telegram(TelegramData telegram_data, uint8_t sender_listener_id = 0);
        void handle_telegram(TelegramData telegram_data, TelegramSource source = TelegramSource::BUS_RECEIVED);
        void notify_peer_listeners(TelegramData telegram_data, uint8_t sender_listener_id);

        void discover_system_devices(uint8_t device_group = 255);
        void finish_system_discovery();

        // Telegram binary listeners
        #ifdef USE_BINARY_SENSOR
        void register_listener(TCBusBinarySensorListener *listener);
        #endif

        // Telegram lock listeners
        #ifdef USE_LOCK
        void register_listener(TCBusLockListener *listener);
        #endif

        // Preferences
        void save_preferences();

        ESPPreferenceObject &get_pref()
        {
            return this->pref_;
        }

        // Automation Callbacks
        void add_received_telegram_callback(std::function<void(TelegramData)> &&callback)
        {
            this->received_telegram_callback_.add(std::move(callback));
        }

        void add_system_discovery_complete_callback(std::function<void()> &&callback)
        {
            this->system_discovery_complete_callback_.add(std::move(callback));
        }

        QueueHandle_t telegram_receive_queue{nullptr};
        FixedQueue<TCBusTelegramQueueItem, 16> telegram_transmit_queue;

        // Misc
        bool is_programming_mode_active() const { return this->programming_mode_; }
        bool is_door_readiness_active() const { return this->door_readiness_active_; }
        uint8_t get_selected_device_group() const { return this->selected_device_group_; }

    protected:
        // Telegram handling
        InternalGPIOPin *rx_pin_;
        InternalGPIOPin *tx_pin_;
        TCBusComponentStore store_;
        std::vector<PrioritizedListener> remote_listeners_;

        uint32_t retransmit_wait_start_us_{0};
        TelegramData retransmit_telegram_{};
        uint8_t retransmit_sender_listener_id_{0};

        // Telegram binary listeners
        #ifdef USE_BINARY_SENSOR
        std::vector<TCBusBinarySensorListener *> listeners_{};
        #endif

        // Telegram lock listeners
        #ifdef USE_LOCK
        std::vector<TCBusLockListener *> lock_listeners_{};
        #endif

        // Preferences
        ESPPreferenceObject pref_;

        // Automation Callbacks
        CallbackManager<void(TelegramData)> received_telegram_callback_{};
        CallbackManager<void()> system_discovery_complete_callback_{};

        // Misc
        uint32_t system_discovery_as_[5] = {0};
        uint32_t system_discovery_is_classic_[50] = {0};
        uint32_t system_discovery_is_handsfree_[50] = {0};
        uint32_t system_discovery_ext_[5] = {0};
        uint32_t system_discovery_ctr_[5] = {0};
        uint32_t system_discovery_acc_[5] = {0};

        uint8_t system_discovery_as_cnt_{0};
        uint8_t system_discovery_is_classic_cnt_{0};
        uint8_t system_discovery_is_handsfree_cnt_{0};
        uint8_t system_discovery_ext_cnt_{0};
        uint8_t system_discovery_ctr_cnt_{0};
        uint8_t system_discovery_acc_cnt_{0};
        uint8_t remote_listener_count_{0};
        uint8_t entrance_address_{63};
        uint8_t second_entrance_address_{63};
        uint8_t selected_device_group_{2};

        bool programming_mode_{false};
        bool door_readiness_active_{false};
        bool error_protocol_pending_{false};
        bool system_discovery_active_{false};
        bool system_discovery_full_scan_{false};

        HighFrequencyLoopRequester high_freq_;

    private:
        void log_device_list(const char* name, const uint32_t* list, uint8_t count);
    };

    static TCBusComponent *global_tc_bus = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}