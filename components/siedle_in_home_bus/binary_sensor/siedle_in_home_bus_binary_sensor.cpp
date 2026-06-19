#include "siedle_in_home_bus_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome::siedle_in_home_bus
{

    static const char *const TAG = "siedle_in_home_bus.binary_sensor";

    void SiedleInHomeBusBinarySensor::on_message(const SiedleInHomeBusMessage &msg)
    {
        if (this->command_.has_value()         && this->command_.value()         != msg.get_command())         { return; }
        if (this->destination_.has_value()     && this->destination_.value()     != msg.get_destination())     { return; }
        if (this->destination_bus_.has_value() && this->destination_bus_.value() != msg.get_destination_bus()) { return; }
        if (this->source_.has_value()          && this->source_.value()          != msg.get_source())          { return; }
        if (this->source_bus_.has_value()      && this->source_bus_.value()      != msg.get_source_bus())      { return; }

        this->publish_state(true);
        if (this->auto_off_ms_ > 0)
        {
            this->auto_off_timer_ = millis() + this->auto_off_ms_;
        }
    }

    void SiedleInHomeBusBinarySensor::on_loop()
    {
        if (!this->has_state() || !this->state || this->auto_off_ms_ == 0)
        {
            return;
        }
        if (millis() >= this->auto_off_timer_)
        {
            this->publish_state(false);
        }
    }

    void SiedleInHomeBusBinarySensor::dump_config()
    {
        LOG_BINARY_SENSOR("", "Siedle In-Home Bus Binary Sensor", this);
        if (this->command_.has_value())
        {
            ESP_LOGCONFIG(TAG, "  Command:     0x%02X", this->command_.value());
        }
        if (this->destination_.has_value() || this->destination_bus_.has_value())
        {
            ESP_LOGCONFIG(TAG, "  Destination: %s:0x%02X",
                this->destination_bus_.has_value() ? str_sprintf("0x%X", this->destination_bus_.value()).c_str() : "*",
                this->destination_.has_value() ? this->destination_.value() : 0xFF);
        }
        if (this->source_.has_value() || this->source_bus_.has_value())
        {
            ESP_LOGCONFIG(TAG, "  Source:      %s:0x%02X",
                this->source_bus_.has_value() ? str_sprintf("0x%X", this->source_bus_.value()).c_str() : "*",
                this->source_.has_value() ? this->source_.value() : 0xFF);
        }
    }

}  // namespace esphome::siedle_in_home_bus
