#include "esp-now_utils_helper.h"

void app_main(void) {

    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();
    init_adc();
    fan_init();
    servo_initialization();

    send_mac(CENTRAL_MAC);

    while(!got_other_mac) {
         vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    while(1) {      
        // other stuff and bla bla bla
        send_message(CENTRAL_WAKE, "1");
        what_to_do = 0;

        while(!what_to_do)
             vTaskDelay(10 / portTICK_PERIOD_MS);

        // take a long break and give time to helper to wake up from light sleep!!
        vTaskDelay(4000 / portTICK_PERIOD_MS);

    }
}
