#pragma once

#include "../tcs_intercom.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome
{
    namespace tcs_intercom
    {
        class TCSIntercomBinarySensor : public binary_sensor::BinarySensor, public TCSIntercomListener
        {
            public:
                void turn_on(uint32_t *timer, uint16_t auto_off) override;
                void turn_off(uint32_t *timer) override;
        };

    }  // namespace tcs_intercom
}  // namespace esphome