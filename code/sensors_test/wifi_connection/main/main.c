#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#define APP_NAME "Wifi test"
#define WIFI_SSID "AndroidAPP"  //NOME WIFI
#define WIFI_PWD "2444666668888888" // PWD WIFI

esp_err_t wifi_return;
esp_mqtt_client_handle_t client;

static EventGroupHandle_t s_wifi_event_group;

#include "esp_http_client.h"

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            break;
        default:
            break;
    }
    return ESP_OK;
}

void test_internet_connectivity() {
    esp_http_client_config_t config = {
        .url = "https://www.google.it",
        .event_handler = http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI("Test", "Internet connectivity test passed");
    } else {
        ESP_LOGE("Test", "Internet connectivity test failed");
    }
    esp_http_client_cleanup(client);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(APP_NAME, "Wifi connected to router");
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI("WiFi", "Connesso alla rete WiFi");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI("WiFi", "Disconnesso dalla rete WiFi");
    }
}

void app_main(void){
    wifi_return=nvs_flash_init();
    if(wifi_return==ESP_ERR_NVS_NO_FREE_PAGES || wifi_return==ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        wifi_return=nvs_flash_init();
    }
    ESP_ERROR_CHECK(wifi_return);
    ESP_LOGI(APP_NAME, "Nvs initialization completed");

    s_wifi_event_group=xEventGroupCreate();

    wifi_return=esp_netif_init();
    if(wifi_return!=ESP_OK){
        ESP_LOGI(APP_NAME,"Error: can't netif init");
    }
    ESP_LOGI(APP_NAME, "Flash init");

    wifi_return=esp_event_loop_create_default();
    if(wifi_return!=ESP_OK){
        ESP_LOGE(APP_NAME,"Error: loop create");
    }
    esp_netif_create_default_wifi_sta();
    ESP_LOGI(APP_NAME, "Default wifi sta created");
    // nvs_handle_t nvs_handle;
    // wifi_return = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    // if (wifi_return != ESP_OK) {
    //     ESP_LOGE(APP_NAME,"Error: can't configure permission access");
    // }

    // // Chiudi la gestione NVS
    // nvs_close(nvs_handle);
    // ESP_LOGI(APP_NAME, "Flash init");

    //initialization of wifi structure
    wifi_init_config_t wifi_connection=WIFI_INIT_CONFIG_DEFAULT();
    ESP_LOGI(APP_NAME, "Flash init");
    wifi_return=esp_wifi_init(&wifi_connection);
    if(wifi_return!=ESP_OK){
        ESP_LOGE(APP_NAME, "Error: wifi initialization error %d", wifi_return);
    }
    ESP_LOGI(APP_NAME, "Wifi init");

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

    //station configuration
    wifi_config_t station_config={
        .sta ={
            .ssid=WIFI_SSID,
            .password=WIFI_PWD,
        },
    };

    
    //setting wifi mode
    wifi_return=esp_wifi_set_mode(WIFI_MODE_STA);
    if(wifi_return!=ESP_OK){
            ESP_LOGE(APP_NAME, "Error: wifi setting mode");
    }
    ESP_LOGI(APP_NAME, "Set mode");

    wifi_return=esp_wifi_set_config(ESP_IF_WIFI_STA, &station_config);
    if(wifi_return!=ESP_OK){
        ESP_LOGE(APP_NAME, "Error: wifi setting configuration");
    }
    ESP_LOGI(APP_NAME, "Set config");

    //starting wifi
    wifi_return=esp_wifi_start();
    if(wifi_return!=ESP_OK){
        ESP_LOGE(APP_NAME, "Error: wifi init");
    }
    ESP_LOGI(APP_NAME, "Wifi start");

    // wifi_return=esp_wifi_connect();
    // if(wifi_return!=ESP_OK){
    //         ESP_LOGE(APP_NAME, "Error: wifi connection %s", esp_err_to_name(wifi_return));
    // }
    ESP_LOGI(APP_NAME, "Wifi init finished");

    EventBits_t bits= xEventGroupWaitBits(s_wifi_event_group, BIT0 | BIT1, pdFALSE, pdFALSE, portMAX_DELAY);

    if(bits & BIT0){
        ESP_LOGI(APP_NAME, "CONNECTION COMPLETED");
    }else if( bits & BIT1){
        ESP_LOGE(APP_NAME, "FAILED TO CONNECT");
    }else{
        ESP_LOGE(APP_NAME, "UNEXPECTED EVENT");
    }

    test_internet_connectivity();
}
