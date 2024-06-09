#include "common.h"

#define WIFI "[WI-FI]"
#define WIFI_SSID "S10 di Lorenzo"
#define WIFI_PASSWORD "1W1llH4ckY0ur4cc0unt"

wifi_config_t wifi_struct;
EventGroupHandle_t event_group;

wifi_config_t wifi_conf = {
    .sta.ssid = WIFI_SSID,
    .sta.password = WIFI_PASSWORD,
};

esp_event_handler_instance_t any_id;
esp_event_handler_instance_t got_ip;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if(event_base == WIFI_EVENT){
        if (event_id == WIFI_EVENT_STA_START){
            // ESP_LOGI(WIFI, "Connecting to wifi...");
            // if (esp_wifi_connect() == ESP_OK){
            //     ESP_LOGI(WIFI, "Connection success!");
            //     // esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
            // }else{
            //     ESP_LOGE(WIFI, "Cannot connect to wifi");

            // }
        }
        else if(event_id == WIFI_EVENT_STA_DISCONNECTED){
            ESP_LOGE(WIFI, "Disconnected, reconnection will be tried");
            ESP_LOGI(WIFI, "Reconnecting to wifi...");
            if (esp_wifi_connect() == ESP_OK){
                ESP_LOGI(WIFI, "Connection success!");
            }
        }
    }
}

void wifi_init(){
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    event_group=xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &got_ip));
    
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t init_config=WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_conf));
    ESP_ERROR_CHECK(esp_wifi_start());

    

    // EventBits_t notification_bits = xEventGroupWaitBits(group, CONN_BIT | FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    // if(notification_bits & CONN_BIT)
    //     ESP_LOGI(WIFI, "Connected to AP");
    // else if (notification_bits & FAIL_BIT)
    //     ESP_LOGE(WIFI, "Failed to connect to AP");
    // else
    //     ESP_LOGE(WIFI, "Unknown event");

    // ESP_LOGI(WIFI, "Init completed");
}

void wifi_reinit(){
    // ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    //event_group=xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &got_ip));
    
    // esp_netif_create_default_wifi_sta();
    wifi_init_config_t init_config=WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_conf));
    ESP_ERROR_CHECK(esp_wifi_start());
}