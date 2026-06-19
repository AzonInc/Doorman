#include "siedle_in_home_bus_button.h"
#include "esphome/core/log.h"

namespace esphome::siedle_in_home_bus
{

    static const char *const TAG = "siedle_in_home_bus.button";

    void SiedleInHomeBusButton::press_action()
    {
        this->parent_->send_message(SiedleInHomeBusMessage(
            this->command_.value(),
            this->destination_.value(),
            this->destination_bus_.value(),
            this->source_.value(),
            this->source_bus_.value()));
    }

    void SiedleInHomeBusButton::dump_config()
    {
        LOG_BUTTON("", "Siedle In-Home Bus Button", this);
        if (this->command_.has_value())
        {
            ESP_LOGCONFIG(TAG, "  Command:     0x%02X", this->command_.value());
        }
        if (this->destination_.has_value() || this->destination_bus_.has_value())
        {
            ESP_LOGCONFIG(TAG, "  Destination: 0x%X:0x%02X",
                this->destination_bus_.has_value() ? this->destination_bus_.value() : 0x08,
                this->destination_.has_value() ? this->destination_.value() : 0x00);
        }
        if (this->source_.has_value() || this->source_bus_.has_value())
        {
            ESP_LOGCONFIG(TAG, "  Source:      0x%X:0x%02X",
                this->source_bus_.has_value() ? this->source_bus_.value() : 0x08,
                this->source_.has_value() ? this->source_.value() : 0x00);
        }
    }

}  // namespace esphome::siedle_in_home_bus
