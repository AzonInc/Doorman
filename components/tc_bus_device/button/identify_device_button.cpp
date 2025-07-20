#include "identify_device_button.h"

namespace esphome
{
    namespace tc_bus
    {
        void IdentifyDeviceButton::press_action()
        {
            this->parent_->request_version();
        }
    }  // namespace tc_bus
}  // namespace esphome