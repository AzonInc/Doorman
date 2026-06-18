#include "protocol.h"

namespace esphome::siedle_in_home_bus {

static const char *const TAG = "siedle_in_home_bus.protocol";

SiedleInHomeBusMessage::SiedleInHomeBusMessage(uint8_t command, uint8_t destination,
                                                uint8_t destination_bus, uint8_t source,
                                                uint8_t source_bus) {
  this->raw_ = (static_cast<uint32_t>(PROLOG)               << 29) |
               (static_cast<uint32_t>(command        & 0x3F) << 23) |
               (static_cast<uint32_t>(destination    & 0x1F) << 18) |
               (static_cast<uint32_t>(destination_bus & 0x0F) << 14) |
               (static_cast<uint32_t>(MIDDLE)               << 11) |
               (static_cast<uint32_t>(source          & 0x1F) <<  6) |
               (static_cast<uint32_t>(source_bus      & 0x0F) <<  2) |
               static_cast<uint32_t>(EPILOG);
}

std::string SiedleInHomeBusMessage::to_string() const {
  return str_sprintf("Command: 0x%02X, Destination: 0x%X:0x%02X, Source: 0x%X:0x%02X",
    this->get_command(), this->get_destination_bus(), this->get_destination(),
    this->get_source_bus(), this->get_source());
}

bool SiedleInHomeBusMessage::has_unexpected_bits() const {
  bool unexpected = false;
  if (this->get_prolog() != PROLOG) {
    ESP_LOGW(TAG, "Unexpected prolog: 0x%02X (expected 0x%02X)", this->get_prolog(), PROLOG);
    unexpected = true;
  }
  if (this->get_middle() != MIDDLE) {
    ESP_LOGW(TAG, "Unexpected middle: 0x%02X (expected 0x%02X)", this->get_middle(), MIDDLE);
    unexpected = true;
  }
  if (this->get_epilog() != EPILOG) {
    ESP_LOGW(TAG, "Unexpected epilog: 0x%02X (expected 0x%02X)", this->get_epilog(), EPILOG);
    unexpected = true;
  }
  if (unexpected)
    ESP_LOGW(TAG, "  in message: %s", this->to_string().c_str());
  return unexpected;
}

}  // namespace esphome::siedle_in_home_bus
