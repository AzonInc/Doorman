#include "tc_bus_serial_component.h"

#include "esphome/core/application.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

#include "esphome/components/logger/logger.h"

#include "esphome/components/tc_bus/tc_bus.h"
#include "esphome/components/tc_bus/protocol.h"

namespace esphome::tc_bus_serial
{
    static const char *const TAG = "tc_bus_serial";

    void TCBusSerialComponent::setup()
    {

      ESP_LOGCONFIG(TAG, "Running setup");

      if(this->base_component_->is_failed())
      {
        this->mark_failed(LOG_STR("TC:BUS is not setup yet!"));
        return;
      }

      if(!this->base_component_->is_ready())
      {
        this->mark_failed(LOG_STR("TC:BUS is not setup yet!"));
        return;
      }

    #ifdef USE_ESP32
      this->uart_num_ = logger::global_logger->get_uart_num();
    #elif defined(USE_ARDUINO)
      this->hw_serial_ = logger::global_logger->get_hw_serial();
    #endif

      // Register remote listener
      this->base_component_->register_remote_listener(this);
    }

    void TCBusSerialComponent::dump_config()
    {
      ESP_LOGCONFIG(TAG, "TC:BUS Serial:");
    }

    optional<uint8_t> TCBusSerialComponent::read_byte_()
    {
      optional<uint8_t> byte;
      uint8_t data = 0;

    #ifdef USE_ESP32
      switch (logger::global_logger->get_uart())
      {
        case logger::UART_SELECTION_UART0:
        case logger::UART_SELECTION_UART1:
    #if !defined(USE_ESP32_VARIANT_ESP32C3) && !defined(USE_ESP32_VARIANT_ESP32C6) && \
        !defined(USE_ESP32_VARIANT_ESP32S2) && !defined(USE_ESP32_VARIANT_ESP32S3)
        case logger::UART_SELECTION_UART2:
    #endif  // !USE_ESP32_VARIANT_ESP32C3 && !USE_ESP32_VARIANT_ESP32S2 && !USE_ESP32_VARIANT_ESP32S3
          if (this->uart_num_ >= 0)
          {
            size_t available;
            uart_get_buffered_data_len(this->uart_num_, &available);
            if (available)
            {
              uart_read_bytes(this->uart_num_, &data, 1, 0);
              byte = data;
            }
          }
          break;
    #if defined(USE_LOGGER_USB_CDC) && defined(CONFIG_ESP_CONSOLE_USB_CDC)
        case logger::UART_SELECTION_USB_CDC:
          if (esp_usb_console_available_for_read())
          {
            esp_usb_console_read_buf((char *) &data, 1);
            byte = data;
          }
          break;
    #endif  // USE_LOGGER_USB_CDC
    #ifdef USE_LOGGER_USB_SERIAL_JTAG
        case logger::UART_SELECTION_USB_SERIAL_JTAG: {
          if (usb_serial_jtag_read_bytes((char *) &data, 1, 0))
          {
            byte = data;
          }
          break;
        }
    #endif  // USE_LOGGER_USB_SERIAL_JTAG
        default:
          break;
      }
    #elif defined(USE_ARDUINO)
      if (this->hw_serial_->available())
      {
        this->hw_serial_->readBytes(&data, 1);
        byte = data;
      }
    #endif

      return byte;
    }

    void TCBusSerialComponent::write_data_(std::vector<uint8_t> &data)
    {
      ESP_LOGI(TAG, "Send data to Serial: \"%.*s\"", (int)data.size(), data.data());

      data.insert(data.begin(), 0x01);
      data.push_back(0x04);
      data.push_back('\n');

    #ifdef USE_ESP32
      switch (logger::global_logger->get_uart())
      {
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
    #elif defined(USE_ARDUINO)
      this->hw_serial_->write(data.data(), data.size());
    #endif
    }

    // Received data from tc_bus
    bool TCBusSerialComponent::on_receive(tc_bus::TelegramData telegram_data, bool received)
    {
      if(telegram_data.is_long)
      {
        ESP_LOGD(TAG, "TC:BUS telegram received: %08X", telegram_data.raw);
      }
      else
      {
        ESP_LOGD(TAG, "TC:BUS telegram received: %04X", telegram_data.raw);
      }

      char hex_buf[9];
      format_telegram_hex(telegram_data.raw, telegram_data.is_long, telegram_data.type, hex_buf);
      size_t len = strlen(hex_buf);
      std::vector<uint8_t> vec(hex_buf, hex_buf + len);
      vec.insert(vec.begin(), received ? '$' : ' ');
      this->write_data_(vec);

      return true;
    }

    // Received data from serial input
    bool TCBusSerialComponent::parse_byte_(uint8_t byte)
    {
      ESP_LOGV(TAG, "Parse Byte: 0x%02X", byte);

      if(byte == 0x01)
      {
        ESP_LOGV(TAG, "Clear RX Buffer");
        this->rx_buffer_.clear();
      }
      else if(byte == 0x04)
      {
        ESP_LOGV(TAG, "Telegram complete");

        const char* buf = reinterpret_cast<const char*>(this->rx_buffer_.data());
        size_t len = this->rx_buffer_.size();

        if(len >= 2 && buf[0] == ' ')
        {
          ESP_LOGI(TAG, "Data received from Serial: \"%.*s\"", (int)len, buf);

          // Parse telegram
          const char* hex_str = buf + 1; // Skip leading space
          size_t hex_len = len - 1;

          uint32_t raw = 0;
          for (size_t i = 0; i < hex_len; ++i)
          {
              char c = hex_str[i];
              uint8_t value = 0;
              if (c >= '0' && c <= '9') value = c - '0';
              else if (c >= 'A' && c <= 'F') value = 10 + (c - 'A');
              else if (c >= 'a' && c <= 'f') value = 10 + (c - 'a');
              else break; // invalid char, stop
              raw = (raw << 4) | value;
          }

          bool is_long = (hex_len == 8);

          tc_bus::TelegramData telegram_data = tc_bus::parseTelegram(raw, is_long, false);

          // Send telegram from serial to TC:BUS
          this->base_component_->send_telegram(telegram_data);
        }
        else
        {
          ESP_LOGW(TAG, "Unknown message format received: \"%.*s\"", (int)len, buf);
        }

        ESP_LOGV(TAG, "Clear RX Buffer");
        this->rx_buffer_.clear();
      }
      else
      {
        this->rx_buffer_.push_back(byte);
      }
      return true;
    }

    void TCBusSerialComponent::loop()
    {
      auto byte = this->read_byte_();
      while (byte.has_value())
      {
        this->parse_byte_(byte.value());
        byte = this->read_byte_();
      }
    }
}