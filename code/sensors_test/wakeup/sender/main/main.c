#include "new_esp-now.h"

void app_main(void){
    
    esp_now_utils_handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();

    send_mac();
    send_message("MSG", "It's time to push this code!");
}