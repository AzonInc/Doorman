#include "command_listener_binary_sensor.h"

namespace esphome
{
    namespace tc_bus
    {
        void CommandListenerBinarySensor::turn_on(uint32_t *timer, uint16_t auto_off)
        {
            this->publish_state(true);
            if (auto_off > 0) *timer = millis() + auto_off;
        }

        void CommandListenerBinarySensor::turn_off(uint32_t *timer)
        {
            this->publish_state(false);
            *timer = 0;
        }
    }  // namespace tc_bus
}  // namespace esphome