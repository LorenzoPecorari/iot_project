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

#define APP_NAME "[ESP-NOW] "

#define Q_LENGTH 10
#define SIZE sizeof(uint8_t)
#define DELAY (2500 / portTICK_PERIOD_MS)

QueueHandle_t queue;

#define CHANNEL 0

//uint8_t mac_addr[ESP_NOW_ETH_ALEN] = {0xf4, 0x12, 0xfa, 0x9f, 0xf4, 0x70}; //esp32 lorenzo
//uint8_t mac_addr[ESP_NOW_ETH_ALEN] = {0x48, 0x27, 0xe2, 0xe1, 0xe0, 0xf8}; // esp32 michele

uint8_t mac_addr[ESP_NOW_ETH_ALEN] = {0}; // initially broadcast, later specific address
uint8_t this_mac[ESP_NOW_ETH_ALEN] = {0};

// "handles" the return value of a function
void esp_now_utils_handle_error(esp_err_t err){
    if(err != ESP_OK){
        ESP_LOGE(APP_NAME, "Error %d", err);
        //ESP_ERROR_CHECK(err);
    }
}

// initializes the wifi driver in order to have station mode and sets only the first channel to use for communication
void wifi_init(){
    esp_now_utils_handle_error(esp_netif_init());
    
    wifi_init_config_t init_conf = WIFI_INIT_CONFIG_DEFAULT();

    esp_now_utils_handle_error(esp_wifi_init(&init_conf));
    esp_now_utils_handle_error(esp_wifi_set_mode(WIFI_MODE_STA));
    esp_now_utils_handle_error(esp_wifi_start());
    
    esp_now_utils_handle_error(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));
}

// callback function for messges receiving
void recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len){
    xQueueSend(queue, data, len);
    ESP_LOGI(APP_NAME, "Received : %d", data[0]);
}

// callback function for messages sending
void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    if (status != ESP_NOW_SEND_SUCCESS)
        ESP_LOGE(APP_NAME, "Send failed");
}

// sends messages throug esp-now
void send_esp_now_msg(uint8_t *data, int len){
    esp_now_utils_handle_error(esp_now_send(mac_addr, data, len));
    ESP_LOGI(APP_NAME, "Sent: %d", *(data));
}

// retrives the mac address of the device (wifi module)
void retrieve_mac(uint8_t* mac){
    if(!mac){
        ESP_LOGI(APP_NAME, "mac retrieving error");
    }

    esp_now_utils_handle_error(esp_read_mac(mac, ESP_MAC_WIFI_STA));
    ESP_LOGI(APP_NAME, "MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

};

// sets the peer with certain mac passed as argoument of the function
void set_peer(esp_now_peer_info_t* peer, uint8_t* mac){
    if(peer && mac){
        memcpy(peer->peer_addr, mac_addr, ESP_NOW_ETH_ALEN);
        peer->channel = CHANNEL;
        peer->encrypt = false;
        
        esp_now_utils_handle_error(esp_now_add_peer(peer));
    }
}

// sets the mac of the peer through set_peer()
void set_mac(uint8_t* mac){
    if(!mac)
        ESP_LOGE(APP_NAME, "Invalid mac pointer");
    else{
        memcpy(mac_addr, mac, SIZE * Q_LENGTH);
        
        esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    
        if (peer == NULL) {
            ESP_LOGE(APP_NAME, "Memory allocation : fail");
            return;
        }

        memset(peer, 0, sizeof(esp_now_peer_info_t));
        set_peer(peer, mac_addr);
        free(peer);
    }

}

// sets the peer in order to sends and receive messages in broadcast
void set_broadcast_mac(){
    uint8_t addr[ESP_NOW_ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,}; // broadcast mac
    set_mac(addr);
}

// transmits its own broadcast
void mac_tx(){
    ESP_LOGI(APP_NAME, "Sending mac to... %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    int cnt = 0;

    while(cnt < ESP_NOW_ETH_ALEN){
        send_esp_now_msg(&this_mac[cnt], SIZE);
        vTaskDelay(DELAY);
        cnt++;
    }
}

// receives the mac address of the other peer
void mac_rx(){
    memset(mac_addr, 0, SIZE * ESP_NOW_ETH_ALEN);
    int cnt = 0;

    queue = xQueueCreate(Q_LENGTH, SIZE);

    // receives the mac address from the other peer
    while(cnt < ESP_NOW_ETH_ALEN){
        xQueueReceive(queue, &mac_addr[cnt], portMAX_DELAY);
        cnt++;
    }

    ESP_LOGI(APP_NAME, "Mac address received= %02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    set_mac(mac_addr);
}

//  creates the queue, initializes esp-now, registers the callback functions, sets the peer to which send data
void init_esp_now(){
    queue = xQueueCreate(Q_LENGTH, SIZE);

    if (queue == NULL) {
        ESP_LOGE(APP_NAME, "Failed to create queue");
        return;
    }

    esp_now_utils_handle_error(esp_now_init());
    esp_now_utils_handle_error(esp_now_register_recv_cb(recv_cb));
    esp_now_utils_handle_error(esp_now_register_send_cb(send_cb));

    set_broadcast_mac();

    memset(this_mac, 0, SIZE * ESP_NOW_ETH_ALEN);
    retrieve_mac(this_mac);
}
