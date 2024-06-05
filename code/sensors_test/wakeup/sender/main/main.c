#include "../../new_esp-now_utils.h"

void app_main(void) {

    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();
    
    send_mac("MAC_ID");

    while(1){
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}