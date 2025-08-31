#include "tc_bus_device_button.h"

namespace esphome
{
    namespace tc_bus
    {
        void IdentifyDeviceButton::press_action()
        {
            this->parent_->request_version();
        }

        void ReadMemoryButton::press_action()
        {
            this->parent_->read_memory();
        }
    }  // namespace tc_bus
}  // namespace esphome