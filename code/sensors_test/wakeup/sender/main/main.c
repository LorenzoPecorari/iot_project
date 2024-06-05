#include "../../new_esp-now_utils.h"

void app_main(void) {

    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();

    // esp_sleep_enable_timer_wakeup(5000000);

    // // esp_sleep_enable_timer_wakeup(2 * 1000000);
    // esp_light_sleep_start();
    // // esp_sleep_enable_timer_wakeup(2 * 1000000);
    // // esp_light_sleep_start();
    
    send_mac("MAC_ID");

    // while(!got_other_mac)
    //     vTaskDelay(100 / portTICK_PERIOD_MS);

    // ESP_LOGI(APP_NAME, "Other mac registered");
    while(1){
        consume_message();
    }
}