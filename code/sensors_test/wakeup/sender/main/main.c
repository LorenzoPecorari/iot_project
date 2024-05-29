#include "../../new_esp-now_utils.h"

void app_main(void) {

    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();

    send_mac();

    int del = 1000;
    char string[16];

    while(1){
        memset(string, '\0', 16);
        sprintf(string, "%d", del);
        send_message("HELO", string);
        vTaskDelay(del / portTICK_PERIOD_MS);
        del += 500;
    }

}