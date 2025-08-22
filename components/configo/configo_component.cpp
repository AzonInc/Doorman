#include "configo_component.h"

#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

#include "esphome/components/logger/logger.h"

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"

namespace esphome
{
  namespace configo
  {
    static const char *const TAG = "configo";

    void ConfigoComponent::setup() {

      ESP_LOGCONFIG(TAG, "Running setup");

      if(this->base_component_->is_failed()) {
        std::string failed_msg = "TC:BUS failed to setup!";
        this->mark_failed(failed_msg.c_str());
        return;
      }

      if(!this->base_component_->is_ready()) {
        std::string not_ready_msg = "TC:BUS is not setup yet!";
        this->mark_failed(not_ready_msg.c_str());
        return;
      }

    #ifdef USE_ARDUINO
      this->hw_serial_ = logger::global_logger->get_hw_serial();
    #endif

    #ifdef USE_ESP_IDF
      this->uart_num_ = logger::global_logger->get_uart_num();
    #endif

      // Register remote listener
      this->base_component_->register_remote_listener(this);
    }

    void ConfigoComponent::dump_config() {
      ESP_LOGCONFIG(TAG, "Configo Interface:");
      ESP_LOGCONFIG(TAG, "  Serial Number: %i (0x%05X)", DEVICE_SERIAL, DEVICE_SERIAL);
      ESP_LOGCONFIG(TAG, "  Key: ehruggrrrg");
    }

    optional<uint8_t> ConfigoComponent::read_byte_() {
      optional<uint8_t> byte;
      uint8_t data = 0;

    #ifdef USE_ARDUINO
      if (this->hw_serial_->available()) {
        this->hw_serial_->readBytes(&data, 1);
        byte = data;
      }
    #endif

    #ifdef USE_ESP_IDF
      switch (logger::global_logger->get_uart()) {
        case logger::UART_SELECTION_UART0:
        case logger::UART_SELECTION_UART1:
    #if !defined(USE_ESP32_VARIANT_ESP32C3) && !defined(USE_ESP32_VARIANT_ESP32C6) && \
        !defined(USE_ESP32_VARIANT_ESP32S2) && !defined(USE_ESP32_VARIANT_ESP32S3)
        case logger::UART_SELECTION_UART2:
    #endif  // !USE_ESP32_VARIANT_ESP32C3 && !USE_ESP32_VARIANT_ESP32S2 && !USE_ESP32_VARIANT_ESP32S3
          if (this->uart_num_ >= 0) {
            size_t available;
            uart_get_buffered_data_len(this->uart_num_, &available);
            if (available) {
              uart_read_bytes(this->uart_num_, &data, 1, 0);
              byte = data;
            }
          }
          break;
    #if defined(USE_LOGGER_USB_CDC) && defined(CONFIG_ESP_CONSOLE_USB_CDC)
        case logger::UART_SELECTION_USB_CDC:
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
          if (esp_usb_console_available_for_read()) {
    #else
          if (esp_usb_console_read_available()) {
    #endif
            esp_usb_console_read_buf((char *) &data, 1);
            byte = data;
          }
          break;
    #endif  // USE_LOGGER_USB_CDC
    #ifdef USE_LOGGER_USB_SERIAL_JTAG
        case logger::UART_SELECTION_USB_SERIAL_JTAG: {
          if (usb_serial_jtag_read_bytes((char *) &data, 1, 0)) {
            byte = data;
          }
          break;
        }
    #endif  // USE_LOGGER_USB_SERIAL_JTAG
        default:
          break;
      }
    #endif

      return byte;
    }

    void ConfigoComponent::write_data_(std::vector<uint8_t> &data, bool linebreak) {
      
      std::string str(data.begin(), data.end());
      ESP_LOGD(TAG, "Send data to Configo Software: \"%s\"", str.c_str());

      data.insert(data.begin(), 0x01);
      data.push_back(0x04);

      if(linebreak)
      {
        data.push_back('\n');
      }

    #ifdef USE_ARDUINO
      this->hw_serial_->write(data.data(), data.size());
    #endif
    #ifdef USE_ESP_IDF
      switch (logger::global_logger->get_uart()) {
        case logger::UART_SELECTION_UART0:
        case logger::UART_SELECTION_UART1:
    #if !defined(USE_ESP32_VARIANT_ESP32C3) && !defined(USE_ESP32_VARIANT_ESP32C6) && \
        !defined(USE_ESP32_VARIANT_ESP32S2) && !defined(USE_ESP32_VARIANT_ESP32S3)
        case logger::UART_SELECTION_UART2:
    #endif  // !USE_ESP32_VARIANT_ESP32C3 && !USE_ESP32_VARIANT_ESP32S2 && !USE_ESP32_VARIANT_ESP32S3
          uart_write_bytes(this->uart_num_, data.data(), data.size());
          break;
    #if defined(USE_LOGGER_USB_CDC) && defined(CONFIG_ESP_CONSOLE_USB_CDC)
        case logger::UART_SELECTION_USB_CDC: {
          const char *msg = (char *) data.data();
          esp_usb_console_write_buf(msg, data.size());
          break;
        }
    #endif  // USE_LOGGER_USB_CDC
    #ifdef USE_LOGGER_USB_SERIAL_JTAG
        case logger::UART_SELECTION_USB_SERIAL_JTAG:
          usb_serial_jtag_write_bytes((char *) data.data(), data.size(), 20 / portTICK_PERIOD_MS);
          delay(10);
          usb_serial_jtag_ll_txfifo_flush();  // fixes for issue in IDF 4.4.7
          break;
    #endif  // USE_LOGGER_USB_SERIAL_JTAG
        default:
          break;
      }
    #endif
    }

    // Received data from bus
    bool ConfigoComponent::on_receive(tc_bus::TelegramData telegram_data, bool received)
    {
      if(telegram_data.is_long)
      {
        ESP_LOGD(TAG, "TC:BUS telegram received: %08X", telegram_data.raw);
      }
      else
      {
        ESP_LOGD(TAG, "TC:BUS telegram received: %04X", telegram_data.raw);
      }

      // Send to serial output -> configo software
      bool qprot = true;

      std::vector<uint8_t> false_types = {
        tc_bus::TELEGRAM_TYPE_DOOR_CALL,
        tc_bus::TELEGRAM_TYPE_FLOOR_CALL,
        tc_bus::TELEGRAM_TYPE_INTERNAL_CALL,
        tc_bus::TELEGRAM_TYPE_CONTROL_FUNCTION,
        tc_bus::TELEGRAM_TYPE_START_TALKING_DOOR_CALL,
        tc_bus::TELEGRAM_TYPE_START_TALKING,
        tc_bus::TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL,
        tc_bus::TELEGRAM_TYPE_STOP_TALKING,
        tc_bus::TELEGRAM_TYPE_OPEN_DOOR,
        tc_bus::TELEGRAM_TYPE_OPEN_DOOR_LONG,
        tc_bus::TELEGRAM_TYPE_LIGHT,
        tc_bus::TELEGRAM_TYPE_DOOR_OPENED,
        tc_bus::TELEGRAM_TYPE_DOOR_CLOSED,
        tc_bus::TELEGRAM_TYPE_END_OF_RINGTONE,
        tc_bus::TELEGRAM_TYPE_END_OF_DOOR_READINESS,
        tc_bus::TELEGRAM_TYPE_INITIALIZE_DOOR_STATION,
        tc_bus::TELEGRAM_TYPE_RESET,
        tc_bus::TELEGRAM_TYPE_SELECT_DEVICE_GROUP,
        tc_bus::TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET,
        tc_bus::TELEGRAM_TYPE_SEARCH_DEVICES,
        tc_bus::TELEGRAM_TYPE_FOUND_DEVICE,
        tc_bus::TELEGRAM_TYPE_FOUND_DEVICE_SUBSYSTEM,
        tc_bus::TELEGRAM_TYPE_PROGRAMMING_MODE,
        tc_bus::TELEGRAM_TYPE_READ_MEMORY_BLOCK,
        tc_bus::TELEGRAM_TYPE_SELECT_MEMORY_PAGE,
        tc_bus::TELEGRAM_TYPE_WRITE_MEMORY,
        tc_bus::TELEGRAM_TYPE_REQUEST_VERSION
      };

      if (std::find(false_types.begin(), false_types.end(), telegram_data.type) != false_types.end()) {
        qprot = false;
      }

      std::vector<uint8_t> vec(telegram_data.hex.begin(), telegram_data.hex.end());
      vec.insert(vec.begin(), received ? (qprot ? '%' : '$') : ' ');
      this->write_data_(vec, true);

      /*if(telegram_data.type == TELEGRAM_TYPE_REQUEST_VERSION)
      {
        waitingForResponse = true;
      }

      waitingForResponse = false;*/

      return true;
    }

    // Received data from configo software
    bool ConfigoComponent::parse_configo_byte_(uint8_t byte)
    {
      ESP_LOGD(TAG, "Parse Configo Byte: 0x%02X", byte);

      if(byte == 0x01)
      {
        this->rx_buffer_.clear();
      }
      else if(byte == 0x04)
      {
        std::string str(this->rx_buffer_.begin(), this->rx_buffer_.end());

        if(str == "(01")
        {
          ESP_LOGI(TAG, "Ping received from Configo Software");

          // Ack
          std::vector<uint8_t> data = { '#' };
          this->write_data_(data);
        }
        else if(str == ".3F")
        {
          ESP_LOGI(TAG, "Device information requested by Configo Software");

          std::vector<uint8_t> data = {'.'};

          char serial_hex[6];
          snprintf(serial_hex, sizeof(serial_hex), "%05X", DEVICE_SERIAL);
          data.insert(data.end(), serial_hex, serial_hex + strlen(serial_hex));

          data.insert(data.end(), DEVICE_SW_VER, DEVICE_SW_VER + sizeof(DEVICE_SW_VER) - 1);
          data.insert(data.end(), DEVICE_PRODUCT, DEVICE_PRODUCT + sizeof(DEVICE_PRODUCT) - 1);

          this->write_data_(data);
        }
        else if(str.substr(0, 1) == " ")
        {
          ESP_LOGI(TAG, "Data received from Configo Software: \"%s\"", str.c_str());

          // Parse telegram
          std::string hex = str.substr(1);
          uint32_t raw = std::stoul(hex, nullptr, 16);
          bool is_long = hex.length() == 8;

          tc_bus::TelegramData telegram_data = tc_bus::parseTelegram(raw, is_long, false);

          // Send telegram from configo software to TC:BUS
          this->base_component_->send_telegram(telegram_data);
        }
        else
        {
          ESP_LOGW(TAG, "Unknown message format received: \"%s\"", str.c_str());
        }
        this->rx_buffer_.clear();
      }
      else
      {
        this->rx_buffer_.push_back(byte);
      }
      return true;
    }

    void ConfigoComponent::loop()
    {
      auto byte = this->read_byte_();
      while (byte.has_value())
      {
        this->parse_configo_byte_(byte.value());
        byte = this->read_byte_();
      }
    }

  }  // namespace configo
}  // namespace esphome