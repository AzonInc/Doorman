#include "read_memory_button.h"

namespace esphome
{
    namespace tc_bus
    {
        void ReadMemoryButton::press_action()
        {
            this->parent_->read_memory();
        }
    }  // namespace tc_bus
}  // namespace esphome