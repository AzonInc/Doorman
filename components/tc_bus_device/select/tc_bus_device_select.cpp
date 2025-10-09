#include "../util.h"
#include "tc_bus_device_select.h"

namespace esphome
{
    namespace tc_bus
    {
        void ModelSelect::control(const std::string &value)
        {
            Model model = string_to_model(value);
            this->parent_->set_model(model);
        }

        void RingtoneEntranceDoorCallSelect::control(const std::string &value)
        {
            this->publish_state(value);
            uint8_t ringtone = ringtone_to_int(value);
            this->parent_->update_setting(SETTING_RINGTONE_ENTRANCE_DOOR_CALL, ringtone);
        }

        void RingtoneSecondEntranceDoorCallSelect::control(const std::string &value)
        {
            this->publish_state(value);
            uint8_t ringtone = ringtone_to_int(value);
            this->parent_->update_setting(SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL, ringtone);
        }

        void RingtoneFloorCallSelect::control(const std::string &value)
        {
            this->publish_state(value);
            uint8_t ringtone = ringtone_to_int(value);
            this->parent_->update_setting(SETTING_RINGTONE_FLOOR_CALL, ringtone);
        }

        void RingtoneInternalCallSelect::control(const std::string &value)
        {
            this->publish_state(value);
            uint8_t ringtone = ringtone_to_int(value);
            this->parent_->update_setting(SETTING_RINGTONE_INTERNAL_CALL, ringtone);
        }

    }  // namespace tc_bus
}  // namespace esphome