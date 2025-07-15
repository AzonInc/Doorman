#include "esphome/core/log.h"
#include "doorman_hardware_text_sensor.h"

#if defined(USE_ESP_IDF) || (defined(USE_ARDUINO) && defined(ESP32))
#include "soc/efuse_reg.h"
#include "soc/efuse_periph.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#endif

namespace esphome
{
    namespace doorman_hardware_text_sensor
    {
        static const char *TAG = "doorman_hardware.text_sensor";

        void DoormanHardwareTextSensor::setup()
        {
            ESP_LOGCONFIG(TAG, "Running setup");

            #if defined(USE_ESP_IDF) || (defined(USE_ARDUINO) && defined(ESP32))
            ESP_LOGD(TAG, "Checking for Doorman hardware");

            // Doorman Hardware Revision
            uint8_t ver[3];
            uint32_t value;
            esp_efuse_read_block(EFUSE_BLK3, &value, 0, 24);
            ver[0] = value >> 0;
            ver[1] = value >> 8;
            ver[2] = value >> 16;

            if (ver[0] > 0)
            {
                ESP_LOGI(TAG, "Detected Doorman Hardware: Revision %i.%i.%i.", ver[0], ver[1], ver[2]);
                this->hardware_version_str_ = "Doorman-S3 " + std::to_string(ver[0]) + "." + std::to_string(ver[1]) + "." + std::to_string(ver[2]);
            }
            #endif

            this->publish_state(this->hardware_version_str_);
        }

        void DoormanHardwareTextSensor::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Doorman Hardware");
        }

    }  // namespace doorman_hardware_text_sensor
}  // namespace esphome