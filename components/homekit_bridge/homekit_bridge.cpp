#include "homekit_bridge.h"
#include "util.h"
#include <esp_event.h>

namespace esphome
{
    namespace homekit
    {
        /* Mandatory identify routine for the accessory.
        * In a real accessory, something like LED blink should be implemented
        * got visual identification
        */
        int HomeKitBridgeComponent::static_identify_handler(hap_acc_t *ha) {
            ESP_LOGI("homekit_bridge", "Accessory identified");
            global_homekit_bridge->identify_callback_.call();
            return HAP_SUCCESS;
        }

        void HomeKitBridgeComponent::static_hap_event_handler(hap_event_t event, void *data) {
            if(global_homekit_bridge) {
                if(event == HAP_EVENT_PAIRING_STARTED) {
                    ESP_LOGD(TAG, "Pairing Started");
                    global_homekit_bridge->pairing_started_callback_.call();
                    global_homekit_bridge->report_heap();
                } else if(event == HAP_EVENT_PAIRING_ABORTED) {
                    ESP_LOGD(TAG, "Pairing Aborted");
                    global_homekit_bridge->pairing_aborted_callback_.call();
                    global_homekit_bridge->report_heap();
                } else if(event == HAP_EVENT_PAIRING_MODE_TIMED_OUT) {
                    ESP_LOGD(TAG, "Pairing Timed Out");
                    global_homekit_bridge->pairing_timeout_callback_.call();
                    global_homekit_bridge->report_heap();
                } else if(event == HAP_EVENT_CTRL_PAIRED) {
                    ESP_LOGD(TAG, "Controller %s Paired. Controller count: %d", (char *)data, hap_get_paired_controller_count());
                    global_homekit_bridge->pairing_completed_callback_.call(std::string(static_cast<char*>(data)));
                    global_homekit_bridge->report_heap();
                } else if(event == HAP_EVENT_CTRL_CONNECTED) {
                    global_homekit_bridge->connection_count_++;
                    ESP_LOGD(TAG, "Controller %s connected. Controller count: %d", (char *)data, global_homekit_bridge->connection_count_);
                    global_homekit_bridge->controller_connected_callback_.call(std::string(static_cast<char*>(data)));
                    global_homekit_bridge->report_heap();
                } else if(event == HAP_EVENT_CTRL_DISCONNECTED) {
                    global_homekit_bridge->connection_count_--;
                    ESP_LOGD(TAG, "Controller %s disconnected. Controller count: %d", (char *)data, global_homekit_bridge->connection_count_);
                    global_homekit_bridge->controller_disconnected_callback_.call(std::string(static_cast<char*>(data)));
                    global_homekit_bridge->report_heap();
                }
            }
        }

        void HomeKitBridgeComponent::report_heap() {
            ESP_LOGD(TAG, "Internal heap: %u", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
            ESP_LOGD(TAG, "Total free heap: %u", esp_get_free_heap_size());
        }

        void HomeKitBridgeComponent::factory_reset() {
            int ret = hap_reset_pairings();

            int count = hap_get_paired_controller_count();
            ESP_LOGI(TAG, "Reset Result: %d, paired: %d", ret, count);
        }

        bool HomeKitBridgeComponent::is_paired() {
            return is_accessory_paired();
        }

        bool HomeKitBridgeComponent::is_connected() {
            return global_homekit_bridge->connection_count_ > 0;
        }

        void HomeKitBridgeComponent::setup() {

            ESP_LOGCONFIG(TAG, "Running setup");

            global_homekit_bridge = this;

            report_heap();

            ESP_LOGD(TAG, "IDF version: %s", esp_get_idf_version());
            ESP_LOGD(TAG, "NVS init: %s", esp_err_to_name(nvs_flash_init()));

            /* Initialize the HAP core */
            hap_init(HAP_TRANSPORT_WIFI);

            /* Initialise the mandatory parameters for Accessory which will be added as
            * the mandatory services internally
            */
            hap_cfg_t hap_cfg;
            hap_get_config(&hap_cfg);
            hap_cfg.task_stack_size = this->task_stack_size_;
            hap_cfg.task_priority = 2;
            hap_set_config(&hap_cfg);

            hap_acc_cfg_t cfg = {
                .name = (char*)this->name_,
                .model = (char*)this->model_,
                .manufacturer = (char*)this->manufacturer_,
                .serial_num = (char*)get_mac_address().c_str(),
                .fw_rev = (char*)"1.0.0",
                .hw_rev = (char*)"1.0.0",
                .pv = (char*)"1.1.0",
                .cid = HAP_CID_BRIDGE,
                .identify_routine = static_identify_handler,
            };

            /* Create accessory object */
            hap_acc_t* accessory = hap_acc_create(&cfg);
            if (!accessory) {
                hap_acc_delete(accessory);

                std::string hap_acc_create_fail_msg = "Failed to create accessory";
                this->mark_failed(hap_acc_create_fail_msg.c_str());
                return;
            }

            /* Add a dummy Product Data */
            uint8_t product_data[] = {'E','S','P','3','2','H','A','P'};
            hap_acc_add_product_data(accessory, product_data, sizeof(product_data));

            /* Add Wi-Fi Transport service required for HAP Spec R16 */
            hap_acc_add_wifi_transport_service(accessory, 0);

            /* Add the Accessory to the HomeKit Database */
            hap_add_accessory(accessory);
            /* Unique Setup code of the format xxx-xx-xxx. Default: 111-22-333 */
            hap_set_setup_code(this->setup_code_);
            /* Unique four character Setup Id. Default: ES32 */
            hap_set_setup_id(this->setup_id_);

            hap_register_event_handler(static_hap_event_handler);

            report_heap();

            // hap_http_debug_enable();
            // hap_set_debug_level(HAP_DEBUG_LEVEL_INFO);
            // esp_log_level_set("HAP", ESP_LOG_DEBUG);

            hap_set_debug_level(HAP_DEBUG_LEVEL_ERR);
            esp_log_level_set("HAP", ESP_LOG_ERROR);

            int start_status = hap_start();

            if(start_status == HAP_SUCCESS) {
                this->is_ready_ = true;

                ESP_LOGI(TAG, "HAP Bridge started successfully!");

                int pairing_count = hap_get_paired_controller_count();
                ESP_LOGI(TAG, "Paired controllers: %d", pairing_count);

                report_heap();
            } else {
                hap_acc_delete(accessory);

                std::string hap_start_fail_msg = "hap_start() failed with status " + std::to_string(start_status);
                this->mark_failed(hap_start_fail_msg.c_str());
            }
        }

        void HomeKitBridgeComponent::loop() {
        }

        void HomeKitBridgeComponent::dump_config() {
            ESP_LOGCONFIG(TAG, "homekit_bridge:");
            ESP_LOGCONFIG(TAG, "  Task Stack Size: %i", this->task_stack_size_);
            ESP_LOGCONFIG(TAG, "  Setup Code: %s", this->setup_code_);
            ESP_LOGCONFIG(TAG, "  Setup ID: %s", this->setup_id_);
            ESP_LOGCONFIG(TAG, "  Name: %s", this->name_);
            ESP_LOGCONFIG(TAG, "  Model: %s", this->model_);
            ESP_LOGCONFIG(TAG, "  Manufacturer: %s", this->manufacturer_);

#ifdef USE_BUTTON
            ESP_LOGCONFIG(TAG, "  Buttons:");
            LOG_BUTTON("    ", "Reset HomeKit Pairing", this->reset_button_);
#endif
        }

    }  // namespace homekit
}  // namespace esphome