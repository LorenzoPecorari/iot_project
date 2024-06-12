#include "common.h"

#define APP_NAME_WIFI "[WI-FI] "

// "handles" the return value of a function
void utils_handle_error(esp_err_t err){
    if(err != ESP_OK){
        ESP_LOGE(APP_NAME_WIFI, "Error %d", err);
        //ESP_ERROR_CHECK(err);
    }
}

// initializes the wifi driver in order to have station mode and sets only the first channel to use for communication
void wifi_init(){
    nvs_flash_erase();
    nvs_flash_init();
    utils_handle_error(esp_netif_init());
    
    wifi_init_config_t init_conf = WIFI_INIT_CONFIG_DEFAULT();

    utils_handle_error(esp_wifi_init(&init_conf));
    utils_handle_error(esp_wifi_set_mode(WIFI_MODE_STA));
    utils_handle_error(esp_wifi_start());
    
    utils_handle_error(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));
}
