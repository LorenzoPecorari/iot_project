#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_task.h"
#include "esp_mac.h"
#include "esp_log.h"

#define APP_NAME "[COMMUNICATION_TEST] "

#define Q_LENGTH 10
#define SIZE sizeof(uint8_t)

QueueHandle_t queue;

#define CHANNEL 0

//uint8_t this_mac[ESP_NOW_ETH_ALEN] = {0};

//uint8_t mac_addr[ESP_NOW_ETH_ALEN] = {0xf4, 0x12, 0xfa, 0x9f, 0xf4, 0x70}; //esp32 lorenzo
//uint8_t mac_addr[ESP_NOW_ETH_ALEN] = {0x48, 0x27, 0xe2, 0xe1, 0xe0, 0xf8}; // esp32 michele
uint8_t mac_addr[ESP_NOW_ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // broadcast mac

// "handles" the return value of a function
void handle_error(esp_err_t err){

    if(err != ESP_OK){
        ESP_LOGE(APP_NAME, "Error %d", err);
        //ESP_ERROR_CHECK(err);
    }

}

// initializes the wifi driver in order to have station mode and sets only the first channel to use for communication
void wifi_init(){

    handle_error(esp_netif_init());
    
    wifi_init_config_t init_conf = WIFI_INIT_CONFIG_DEFAULT();

    handle_error(esp_wifi_init(&init_conf));
    handle_error(esp_wifi_set_mode(WIFI_MODE_STA));
    handle_error(esp_wifi_start());
    
    handle_error(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));

}

// callback function for messges receiving
void recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len){

    xQueueSend(queue, data, len);
    ESP_LOGI(APP_NAME, "Received : %d", data[0]);

}

// callback function for messages sending
void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    
    if (status == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGI(APP_NAME, "Send success");
    }
    else {
        ESP_LOGE(APP_NAME, "Send failed");
    }

}

// sends messages throug esp-now
void send_esp_now_msg(uint8_t *data, int len){

    handle_error(esp_now_send(mac_addr, data, len));
    ESP_LOGI(APP_NAME, "Sent: %d", *(data));

}

// retrives the mac address of the device (wifi module)
void retrieve_mac(uint8_t* mac, int len){
    if(!mac){
        ESP_LOGI(APP_NAME, "mac retrieving error");
    }

    handle_error(esp_read_mac(mac, ESP_MAC_WIFI_STA));
    ESP_LOGI(APP_NAME, "MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

};

//  creates the queue, initializes esp-now, registers the callback functions, sets the peer to which send data
void init_esp_now(){

    queue = xQueueCreate(Q_LENGTH, SIZE);

    if (queue == NULL) {
        ESP_LOGE(APP_NAME, "Failed to create queue");
        return;
    }

    handle_error(esp_now_init());
    handle_error(esp_now_register_recv_cb(recv_cb));
    handle_error(esp_now_register_send_cb(send_cb));

    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    
    if (peer == NULL) {
        ESP_LOGE(APP_NAME, "Memory allocation : fail");
        return;
    }

    memset(peer, 0, sizeof(esp_now_peer_info_t));
    
    memcpy(peer->peer_addr, mac_addr, ESP_NOW_ETH_ALEN);
    peer->channel = CHANNEL;
    peer->encrypt = false;
    
    handle_error(esp_now_add_peer(peer));
    
    free(peer);
}

void app_main(void){
    
    handle_error(nvs_flash_init());
    wifi_init();
    init_esp_now();

    int cnt = 0;

    uint8_t* data = (uint8_t*) malloc(sizeof(uint8_t) * ESP_NOW_ETH_ALEN);
    if(data){
        retrieve_mac(data, ESP_NOW_ETH_ALEN);
        
        // sends data containing the mac addr of the device (wifi module)
        while(cnt < ESP_NOW_ETH_ALEN){
            send_esp_now_msg(&data[cnt], SIZE);
            vTaskDelay(2500 / portTICK_PERIOD_MS);
            cnt++;
        }

        cnt = 0;
        memset(data, 0, sizeof(uint8_t) * ESP_NOW_ETH_ALEN);
        memset(mac_addr, 0, sizeof(uint8_t) * ESP_NOW_ETH_ALEN);

        // receives the mac address from the other peer
        while(cnt < ESP_NOW_ETH_ALEN){
            xQueueReceive(queue, &data[cnt], portMAX_DELAY);
            cnt++;
        }

        memcpy(mac_addr, data, sizeof(uint8_t) * ESP_NOW_ETH_ALEN);
        free(data);

        ESP_LOGI(APP_NAME, "Mac address received= %02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);


    }
    else
        ESP_LOGE(APP_NAME, "Memory allocation: error!");

}