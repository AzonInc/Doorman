#pragma once

#include <string>
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

#define LOG_SIEDLE_MESSAGE(prefix, msg) \
  if ((msg) != nullptr) { \
    ESP_LOGCONFIG(TAG, "%sMessage:", prefix); \
    ESP_LOGCONFIG(TAG, "%s  Command:     0x%02X", prefix, (msg)->get_command()); \
    ESP_LOGCONFIG(TAG, "%s  Destination: 0x%X:0x%02X", prefix, (msg)->get_destination_bus(), (msg)->get_destination()); \
    ESP_LOGCONFIG(TAG, "%s  Source:      0x%X:0x%02X", prefix, (msg)->get_source_bus(), (msg)->get_source()); \
  }

namespace esphome::siedle_in_home_bus {

class SiedleInHomeBusMessage {
 public:
  SiedleInHomeBusMessage(uint8_t command, uint8_t destination, uint8_t destination_bus,
                          uint8_t source, uint8_t source_bus);
  explicit SiedleInHomeBusMessage(uint32_t raw) : raw_(raw) {}

  uint32_t get_raw() const { return this->raw_; }
  std::string to_string() const;
  bool has_unexpected_bits() const;

  uint8_t get_command() const         { return (this->raw_ >> 23) & 0x3F; }
  uint8_t get_destination() const     { return (this->raw_ >> 18) & 0x1F; }
  uint8_t get_destination_bus() const { return (this->raw_ >> 14) & 0x0F; }
  uint8_t get_source() const          { return (this->raw_ >>  6) & 0x1F; }
  uint8_t get_source_bus() const      { return (this->raw_ >>  2) & 0x0F; }

 protected:
  uint32_t raw_{0};

  uint8_t get_prolog() const { return (this->raw_ >> 29) & 0x07; }
  uint8_t get_middle() const { return (this->raw_ >> 11) & 0x07; }
  uint8_t get_epilog() const { return  this->raw_        & 0x03; }

  static constexpr uint8_t PROLOG = 0b010;
  static constexpr uint8_t MIDDLE = 0b010;
  static constexpr uint8_t EPILOG = 0b00;
};

}  // namespace esphome::siedle_in_home_bus
