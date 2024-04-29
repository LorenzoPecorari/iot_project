#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <stdio.h>
#include <string.h>

#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_now.h"
#include "nvs_flash.h"

#define APP_NAME "[COMMUNICATION_TEST] "

#define Q_LENGTH 10
#define SIZE sizeof(uint8_t)
QueueHandle_t received_queue;

#define SSID "<INSERT HERE AP NAME>"
#define PWD "<INSERT HERE PWD OF AP>"

// channel for using esp-now
#define CHANNEL 0

// variables for wifi and error handling
wifi_init_config_t init_conf;
wifi_config_t wifi_conf;
esp_err_t error;
esp_now_peer_info_t peer_info;

// mac addresses variables
uint8_t peer_mac[ESP_NOW_ETH_ALEN];
//uint8_t peer_mac[ESP_NOW_ETH_ALEN] = {0xf4, 0x12, 0xfa, 0x9f, 0xf4, 0x70} //esp32 lorenzo
//uint8_t peer_mac[ESP_NOW_ETH_ALEN] = {0x48, 0x27, 0xe2, 0xe1, 0xe0, 0xf8}; // esp32 michele
uint8_t this_mac[6];

uint8_t data[];

int uninitialized_connection = 1;

/*
void event_handler(void* arg, esp_event_base_t base, int32_t id, void* data){
    printf("id: %ld - base : %s\n", id, base);
    if(id == WIFI_EVENT_STA_DISCONNECTED){
        printf("Disconnected from the network, riconnecting...\n");
        esp_wifi_connect();
    }
    else if(id == WIFI_EVENT_STA_CONNECTED)
        printf("Connected to network");
}
*/

// initialization of structure for knowing node to send data (peer)
void mac_peer_init(uint8_t *mac){

        memcpy(peer_info.peer_addr, mac, sizeof(*(mac)));
        peer_info.channel = CHANNEL;
        peer_info.encrypt = false;

        error = esp_now_add_peer(&peer_info);
        if(error != ESP_OK)
            ESP_LOGE(APP_NAME, "ESP-NOW peer adding - error : %d", error);

}

// function for sending messages through esp-now protocol
void send_now_msg(uint8_t *data){

    error = esp_now_send(peer_info.peer_addr, data, sizeof(*(data)));
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "ESP-NOW send data - error : %d", error);
    else{
        printf("Data : %s\n", (char*) (data));
        ESP_LOGI(APP_NAME, "Sended: %d", *(data));
    }

}

// callback function for handling messages received
void callback_function(uint8_t *mac, uint8_t *data, int len){

    ESP_LOGI(APP_NAME, "Sending : '%d' to the queue", *(data));
    xQueueSend(received_queue, data, 0);

    // decomment this part only for advanced testing
    /*
    // if uninitialized_connection == 1 it means that it does not know which device is the other peer
    if(uninitialized_connection){
        mac_peer_init(data);
        send_now_msg(this_mac);
        uninitialized_connection++;
    }
    // it will act "normally"
    else{
        // do some other stuff...
        ;
    }
    */

}

// wifi configuration function
void wifi_config(){

    // it initializes network interfaces and their configuration    
    error = esp_netif_init(); 
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Netif initialization - error : %d", error);

    ESP_LOGI(APP_NAME, "Netif initialization done");

    // it uses this macro to "automatically" configure the structure
    init_conf = (wifi_init_config_t) WIFI_INIT_CONFIG_DEFAULT();
    ESP_LOGI(APP_NAME, "Structure configured");

    // it uses the previously configured structure to initialize the wifi module
    error = esp_wifi_init(&init_conf);
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi initialization - error : %d", error);

    //esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    
    // structure for making the connection the connection to the access point (station mode!)
    wifi_conf = (wifi_config_t){
        .sta = {
            .ssid = SSID,
            .password = PWD
        },
    };

    error = esp_wifi_set_mode(WIFI_MODE_STA);
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi set mode - error : %d", error);

    // other parameters for station mode connection
    error = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_conf);
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi set config - error : %d", error);

    // it reads the mac of the device in station mode
    error = esp_read_mac(this_mac, ESP_MAC_WIFI_STA);
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Esp read mac - error : %d", error);
    else
        ESP_LOGI(APP_NAME, "MAC address : %02x:%02x:%02x:%02x:%02x:%02x\n", this_mac[0], this_mac[1], this_mac[2], this_mac[3], this_mac[4], this_mac[5]);

}

// function for initializing esp-now protocol and setting the remaining wifi parameters
void init_esp_now(){
    
    error = esp_now_init(); 
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "ESP-NOW init - error : %d", error);
    
    // it sets the given function as callback function for handling incoming messages
    error = esp_now_register_recv_cb(callback_function);
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "ESP-NOW callback function registration - error : %d", error);
        
}

void wifi_start(){

    error = esp_wifi_start() ; 
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi start - error : %d", error);
    
    error = esp_wifi_connect();
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi connect - error : %d", error);

    error = esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi set channel - error : %d", error);

}

void wifi_end(){
    
    error = esp_wifi_disconnect();
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi disconnect error : %d", error);

    error = esp_wifi_stop();
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi stop - error : %d", error);

    error = esp_wifi_deinit();
    if(error != ESP_OK)
        ESP_LOGE(APP_NAME, "Wifi deinit - error : %d", error);

}

void app_main(){

    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(APP_NAME, "Starting wifi configuration...");
    wifi_config();
    ESP_LOGI(APP_NAME, "Wifi configuration completed");

    ESP_LOGI(APP_NAME, "ESP-NOW configuration started...");
    init_esp_now();
    ESP_LOGI(APP_NAME, "ESP-NOW configuration completed");
    
    int cnt = 0;

    ESP_LOGI(APP_NAME, "MAC peering configuration started...");
    mac_peer_init(peer_mac);
    ESP_LOGI(APP_NAME, "completed");    

    // part of code for knowing if it is really connected to the access point
    // (NOT working)
    /*
    wifi_config_t wifi_config;
    esp_err_t ret = esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config);
    char ssid[33];
    strncpy(ssid, (char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
    ssid[sizeof(wifi_config.sta.ssid)] = '\0'; 
    printf("Connected to: %s\n", ssid);
    */

   /*
    printf("\tWaiting 10s...");
    while(cnt < 100){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        cnt++;
    }
    printf(" done\n");
    */

   // part for sending data
   // in the end, it should send an array of 10 'ones' of size = sizeof(uint8_t)
   /*
    while(cnt < Q_LENGTH){
        data[cnt]++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        send_now_msg(data);
        cnt++;
    }
    */
   
   // part for receiving data
   /*
    received_queue = xQueueCreate(Q_LENGTH, SIZE);

    uint8_t item;

    while(1){
        if(xQueueReceive(received_queue, &item, portMAX_DELAY)){
            ESP_LOGI(APP_NAME, "Received data: %d", item);
        }
    }
    */

    ESP_LOGI(APP_NAME, "Closing the connection...");
    wifi_end();
    ESP_LOGI(APP_NAME, "Connection closed");

}