#include "bus_telegram_listener_binary_sensor.h"

namespace esphome::tc_bus
{
    void BusTelegramListenerBinarySensor::turn_on(uint32_t *timer, uint16_t auto_reset)
    {
        this->publish_state(true);
        if (auto_reset > 0) *timer = millis() + auto_reset;
    }

    void BusTelegramListenerBinarySensor::turn_off(uint32_t *timer)
    {
        this->publish_state(false);
        *timer = 0;
    }
}