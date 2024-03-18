#include "tcs_intercom.h"
#include "TCSBus.h"
#include "esphome/core/log.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/core/application.h"
#include <Arduino.h>
#include <cstddef>

#define TCS_MSG_START_MS 6 // a new message
#define TCS_ONE_BIT_MS 4   // a 1-bit is 4ms long
#define TCS_ZERO_BIT_MS 2  // a 0-bit is 2ms long

namespace esphome
{
    namespace tcs_intercom
    {
        static const char *const TAG = "tcs_intercom";

        TCSComponent *global_tcs_intercom = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        bool writing = false;

        void TCSComponent::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up TCS Intercom...");

            this->rx_pin_->setup();
            this->rx_pin_->pin_mode(gpio::FLAG_INPUT);

            this->tx_pin_->setup();
            this->tx_pin_->pin_mode(gpio::FLAG_OUTPUT);
            this->tx_pin_->digital_write(false);
            
            auto &s = this->store_;

            s.rx_pin = this->rx_pin_->to_isr();
            this->rx_pin_->attach_interrupt(TCSComponentStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);

            for (auto &listener : listeners_)
            {
                listener->turn_off(&listener->timer_);
            }
        }

        void TCSComponent::dump_config()
        {
            ESP_LOGCONFIG(TAG, "TCS Intercom:");
            LOG_PIN("  Pin RX: ", this->rx_pin_);
            LOG_PIN("  Pin TX: ", this->tx_pin_);

            if (strcmp(event_, "esphome.none") != 0) {
                ESP_LOGCONFIG(TAG, "  Event: %s", event_);
            } else {
                ESP_LOGCONFIG(TAG, "  Event: disabled");
            }
        }

        void TCSComponent::loop()
        {
            uint32_t now_millis = millis();

            for (auto &listener : listeners_)
            { 
                if (listener->timer_ && now_millis > listener->timer_)
                {
                    listener->turn_off(&listener->timer_);
                }
            }

            if (this->sending)
            {
                sending_loop();
                return;
            }

            auto &s = this->store_;

            if(s.s_cmdReady)
            {
                this->command = s.s_cmd;

                s.s_cmdReady = false;
                s.s_cmd = 0;

                ESP_LOGD(TAG, "Received command %x", this->command);

                if (strcmp(event_, "esphome.none") != 0)
                {
                    auto capi = new esphome::api::CustomAPIDevice();

                    ESP_LOGD(TAG, "Send event to home assistant on %s", event_);
                    capi->fire_homeassistant_event(event_, {{"command", std::to_string(id(this->command))}});
                }

                for (auto &listener : listeners_)
                {
                    if (listener->command_ == this->command)
                    {
                        ESP_LOGD(TAG, "Binary sensor fired! %x", listener->command_);
                        listener->turn_on(&listener->timer_, listener->auto_off_);
                    }
                }
            }
        }

        void TCSComponent::register_listener(TCSIntercomListener *listener)
        {
            this->listeners_.push_back(listener); 
        }

        volatile uint32_t TCSComponentStore::s_cmd = 0;
        volatile uint8_t TCSComponentStore::s_cmdLength = 0;
        volatile bool TCSComponentStore::s_cmdReady = false;

        void IRAM_ATTR HOT TCSComponentStore::gpio_intr(TCSComponentStore *arg)
        {
            static uint32_t curCMD;
            static uint32_t usLast;
            static byte curCRC;
            static byte calCRC;
            static byte curLength;
            static byte cmdIntReady;
            static byte curPos;
            uint32_t usNow = micros();
            uint32_t timeInUS = usNow - usLast;
            usLast = usNow;
            byte curBit = 4;


            if (timeInUS >= 1000 && timeInUS <= 2999)
            {
                curBit = 0;
            }
            else if (timeInUS >= 3000 && timeInUS <= 4999)
            {
                curBit = 1;
            }
            else if (timeInUS >= 5000 && timeInUS <= 6999)
            {
                curBit = 2;
            }
            else if (timeInUS >= 7000 && timeInUS <= 24000)
            {
                curBit = 3;
                curPos = 0;
            }


            if (curPos == 0)
            {
                if (curBit == 2)
                {
                    curPos++;
                }
                curCMD = 0;
                curCRC = 0;
                calCRC = 1;
                curLength = 0;
            }
            else if (curBit == 0 || curBit == 1)
            {
                if (curPos == 1)
                {
                    curLength = curBit;
                    curPos++;
                }
                else if (curPos >= 2 && curPos <= 17)
                {
                    if (curBit)
                    {
                        bitSet(curCMD, (curLength ? 33 : 17) - curPos);
                    }
                    calCRC ^= curBit;
                    curPos++;
                }
                else if (curPos == 18)
                {
                    if (curLength)
                    {
                        if (curBit)
                        {
                            bitSet(curCMD, 33 - curPos);
                        }
                        calCRC ^= curBit;
                        curPos++;
                    }
                    else
                    {
                        curCRC = curBit;
                        cmdIntReady = 1;
                    }
                }
                else if (curPos >= 19 && curPos <= 33)
                {
                    if (curBit)
                    {
                        bitSet(curCMD, 33 - curPos);
                    }
                    calCRC ^= curBit;
                    curPos++;
                }
                else if (curPos == 34)
                {
                    curCRC = curBit;
                    cmdIntReady = 1;
                }
            }
            else
            {
                curPos = 0;
            }

            
            if (cmdIntReady)
            {
                cmdIntReady = 0;
                if (curCRC == calCRC)
                {
                    arg->s_cmdReady = true;
                    arg->s_cmd = curCMD;
                }
                curCMD = 0;
                curPos = 0;
            }
        }


        void TCSComponent::send_command(TCSIntercomData data)
        {
            ESP_LOGD(TAG, "Sending command %x", data.command);

            ESP_LOGD(TAG, "Disable RX");
            this->rx_pin_->detach_interrupt();

            this->sending = true;
            this->command = data.command;
        }

        void TCSComponent::sending_loop()
        {
            uint32_t now = micros();
            
            if(writing)
            {
                ESP_LOGD(TAG, "Block sending loop, sending not complete yet");
                return;
            }

            if (this->sending)
            {

                ESP_LOGD(TAG, "Sending");

                int length = 16;
                byte checksm = 1;
                bool isLongMessage = false;
                if (this->command > 0xFFFF)
                {
                    length = 32;
                    isLongMessage = 1;

                    ESP_LOGD(TAG, "isLongMessage");
                }
                this->tx_pin_->digital_write(true);
                delayMicroseconds(TCS_MSG_START_MS * 1000);
                this->tx_pin_->digital_write(!this->tx_pin_->digital_read());
                delayMicroseconds(isLongMessage ? TCS_ONE_BIT_MS * 1000 : TCS_ZERO_BIT_MS * 1000);
                int curBit = 0;
                for (byte i = length; i > 0; i--)
                {
                    curBit = bitRead(this->command, i - 1);
                    this->tx_pin_->digital_write(!this->tx_pin_->digital_read());
                    delayMicroseconds(curBit ? TCS_ONE_BIT_MS * 1000 : TCS_ZERO_BIT_MS * 1000);
                    checksm ^= curBit;
                }
                this->tx_pin_->digital_write(!this->tx_pin_->digital_read());
                delayMicroseconds(checksm ? TCS_ONE_BIT_MS * 1000 : TCS_ZERO_BIT_MS * 1000);
                this->tx_pin_->digital_write(false);

                writing = false;

                this->sending = false;
                this->rx_pin_->attach_interrupt(TCSComponentStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);

                ESP_LOGD(TAG, "Enable RX");
            }
        }

    }  // namespace tcs_intercom
}  // namespace esphome










/*

#include "tcs_intercom.h"
#include "TCSBus.h"
#include "esphome/core/log.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/core/application.h"
#include <Arduino.h>
#include <cstddef>

namespace esphome
{
    namespace tcs_intercom
    {
        static const char *const TAG = "tcs_intercom";

        TCSComponent *global_tcs_intercom = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        TCSBusWriter tcsWriter = NULL;
        TCSBusReader tcsReader = NULL;

        void TCSComponent::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up TCS Intercom...");

            tcsWriter = TCSBusWriter(this->tx_pin_->get_pin());
            tcsReader = TCSBusReader(this->rx_pin_->get_pin());

            //this->rx_pin_->setup();
            //this->tx_pin_->setup();
            

            for (auto &listener : listeners_)
            {
                listener->turn_off(&listener->timer_);
            }
        }

        void TCSComponent::dump_config()
        {
            ESP_LOGCONFIG(TAG, "TCS Intercom:");
            LOG_PIN("  Pin RX: ", this->rx_pin_);
            LOG_PIN("  Pin TX: ", this->tx_pin_);

            if (strcmp(event_, "esphome.none") != 0) {
                ESP_LOGCONFIG(TAG, "  Event: %s", event_);
            } else {
                ESP_LOGCONFIG(TAG, "  Event: disabled");
            }
        }

        void TCSComponent::loop()
        {
            uint32_t now_millis = millis();

            for (auto &listener : listeners_)
            { 
                if (listener->timer_ && now_millis > listener->timer_)
                {
                    listener->turn_off(&listener->timer_);
                }
            }

            sending_loop();

            if (tcsReader.hasCommand())
            {
                this->command = tcsReader.read();

                ESP_LOGD(TAG, "Received command %x", this->command);

                if (strcmp(event_, "esphome.none") != 0)
                {
                    auto capi = new esphome::api::CustomAPIDevice();

                    ESP_LOGD(TAG, "Send event to home assistant on %s", event_);
                    capi->fire_homeassistant_event(event_, {{"command", std::to_string(id(this->command))}});
                }

                for (auto &listener : listeners_)
                {
                    if (listener->command_ == this->command)
                    {
                        ESP_LOGD(TAG, "Binary sensor fired! %x", listener->command_);
                        listener->turn_on(&listener->timer_, listener->auto_off_);
                    }
                }
            }
        }

        void TCSComponent::register_listener(TCSIntercomListener *listener)
        {
            this->listeners_.push_back(listener); 
        }

        void TCSComponent::send_command(TCSIntercomData data)
        {
            ESP_LOGD(TAG, "Sending command %x", data.command);

            this->sending = true;
            this->command = data.command;
        }

        void TCSComponent::sending_loop()
        {
            uint32_t now = micros();
            
            if (this->sending)
            {
                tcsReader.disable();
                tcsWriter.write(this->command);
                this->sending = false;

                tcsReader.enable();
            }
        }

    }  // namespace tcs_intercom
}  // namespace esphome

*/