#include "common.h"

#define APP_NAME "[WI-FI]"

void wifi_init(){
    ESP_ERROR_CHECK(esp_netif_init());
    
    wifi_init_config_t init_config=WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE));
}