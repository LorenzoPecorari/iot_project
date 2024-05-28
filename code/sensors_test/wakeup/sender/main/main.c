#include "new_esp-now_utils.h"

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Wi-Fi and ESP-NOW
    wifi_init();
    init_esp_now();

    // Send MAC address (optional, for testing)
    send_mac();

    // Enter light sleep mode
    ESP_LOGI(APP_NAME, "Entering light sleep mode");
    esp_light_sleep_start();

    // This point is reached only upon wake up
    ESP_LOGI(APP_NAME, "Woke up from light sleep");
}