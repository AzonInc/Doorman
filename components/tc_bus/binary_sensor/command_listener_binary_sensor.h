#pragma once

#include "../tc_bus.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome
{
    namespace tc_bus
    {
        class CommandListenerBinarySensor : public binary_sensor::BinarySensor, public TCBusListener
        {
            public:
                void turn_on(uint32_t *timer, uint16_t auto_off) override;
                void turn_off(uint32_t *timer) override;
        };

    }  // namespace tc_bus
}  // namespace esphome