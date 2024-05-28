#include "../../new_esp-now_utils.h"

void app_main(void) {

    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();

    send_mac();

    ESP_LOGI(APP_NAME, "sleep");
    esp_light_sleep_start();
    ESP_LOGI(APP_NAME, "awake");
}