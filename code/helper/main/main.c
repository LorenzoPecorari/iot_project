#include "esp-now_utils_helper.h"

void app_main(void) {

    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();
    init_adc();
    fan_init();
    servo_initialization();

    while(!got_other_mac) {
         vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    while(1) {        
        // esp_sleep_enable_timer_wakeup(2500000);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // light_sleep_custom();
        
        what_to_do = 0;


        while(!what_to_do)
            vTaskDelay(10 / portTICK_PERIOD_MS);
        
        // ESP_LOGI(APP_NAME_ESPNOW, "Decision taken, I'll send values");
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        // what_to_do = 0;

        // while(!what_to_do)
        //     vTaskDelay(10 / portTICK_PERIOD_MS);

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        esp_sleep_enable_timer_wakeup(600 * 1000 *1000);
        light_sleep_custom();
    }
}
