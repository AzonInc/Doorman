#include "../util.h"
#include "model_select.h"

namespace esphome {
namespace tc_bus {

void ModelSelect::control(const std::string &value)
{
    Model model = string_to_model(value);
    this->parent_->set_model(model);
}

}  // namespace tc_bus
}  // namespace esphome