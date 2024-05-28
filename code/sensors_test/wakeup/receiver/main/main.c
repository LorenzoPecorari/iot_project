#include "new_esp-now_utils.h"

void app_main(void) {
    // Inizializza NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Inizializza Wi-Fi
    wifi_init();

    // Inizializza ESP-NOW
    init_esp_now();

    // Entra in modalità light sleep
    ESP_LOGI(APP_NAME, "Entering light sleep mode");
    esp_light_sleep_start();

    // Questo punto viene raggiunto solo al risveglio
    ESP_LOGI(APP_NAME, "Woke up from light sleep");

    // Eventuali azioni da eseguire al risveglio
    // ...
}