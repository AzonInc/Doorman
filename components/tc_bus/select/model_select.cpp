#include "../protocol.h"
#include "model_select.h"

namespace esphome {
namespace tc_bus {

void ModelSelect::control(const std::string &value)
{
    this->publish_state(value);

    Model model = string_to_model(value);
    this->parent_->set_model(model);
    this->parent_->save_settings();
    this->parent_->publish_settings();
}

}  // namespace tc_bus
}  // namespace esphome