#include "../../new_esp-now_utils.h"

void app_main(void) {
    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();

    // Attendere che l'altro MAC sia ricevuto prima di procedere
    while(!got_other_mac) {
        vTaskDelay(10 / portTICK_PERIOD_MS); // Attendere brevemente per evitare un loop troppo serrato
    }
    send_mac("MAC_REPLY");

    while(1) {        
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        esp_sleep_enable_timer_wakeup(2 * 1000000);
        esp_light_sleep_start();

        //init_esp_now();
        what_to_do = 0;

        // Attendere una decisione prima di entrare nuovamente in light sleep
        while(!what_to_do) {
            send_message("WAKE_REQ", "request on what to do");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        ESP_LOGI(APP_NAME, "Decision taken, I'll do something then go back to sleep");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
