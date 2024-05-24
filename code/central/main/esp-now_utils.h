#include "common.h"

#include "esp_now.h"
#include "esp_task.h"
#include "esp_mac.h"

#define APP_NAME "[ESP-NOW]"
#define QUEUE_LENGTH 10
#define Q_ELEMENT_SIZE sizeof(uint8_t)
#define DELAY (2500 / portTICK_PERIOD_MS)

uint8_t central_mac[ESP_NOW_ETH_ALEN]={0};
uint8_t helper_mac[ESP_NOW_ETH_ALEN]={0};

QueueHandle_t queue;

void esp_now_mac_tx(){
    ESP_LOGE(APP_NAME, "Mac transmission");
    int count=0;
    while(count<ESP_NOW_ETH_ALEN){
        send_esp_now_msg(&central_mac[count], Q_ELEMENT_SIZE);
        vTaskDelay(DELAY);
        count++;
    }
}

void esp_now_mac_rx(){
    memset(helper_mac, 0, Q_ELEMENT_SIZE*ESP_NOW_ETH_ALEN);
    int count=0;
    while(count<ESP_NOW_ETH_ALEN){
        xQueuReceive(queue, &helper_mac[count], portMAX_DELAY);
        count++;
    }
    ESP_LOGI(APP_NAME, "Mac address received: %02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void retrieve_mac(){
    if(!central_mac){
        ESP_LOGE(APP_NAME, "Mac retrieving error");
    }

    ESP_ERROR_CHECK(esp_read_mac(central_mac, ESP_MAC_WIFI_STA));
    ESP_LOGI(APP_NAME, "CENTRAL MAC: %02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
}

void set_peer(esp_now_peer_info_t* peer,const uint8_t* mac_address){
    if(peer && mac){
        memcpy(peer->peer_addr, helper_mac, ESP_NOW_ETH_ALEN);
        peer->channel=CHANNEL;
        peer->encrypt=false;

        ESP_ERROR_CHECK(esp_now_add_peer(peer));
    }
}

void set_mac(const uint8_t* mac_address){
    if(!mac){
        ESP_LOGE(APP_NAME, "Invalid mac pointer");
    }else{
        memcpy(helper_mac, mac_address, ESP_NOW_ETH_ALEN);

        esp_now_peer_info_t* peer=(esp_now_peer_info_t*)malloc(sizeof(esp_now_peer_info_t));
        if(peer==NULL){
            ESP_LOGE(APP_NAME, "Memory allocation failed");
            return;
        }
        memset(peer, 0, sizeof(esp_now_peer_info_t));
        set_peer(peer, mac_address);
        free(peer);
    }
}

void set_broadcast_trasmission(){
    uint8_t address[ESP_NOW_ETH_ALEN]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    set_mac(address);
}

//CALLBACK FUNCTION FOR MESSAGES REICEVING
void receiver_cb(const uint8_t* queue,const uint8_t* data, int len){
    xQueueSend(queue, data, len);
    ESP_LOGI(APP_NAME, "Received: %d", data[0]);
}

//CALLBACK FUNCTION FOR MESSAGES SENDING
void sender_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    if(status==ESP_NOW_SEND_SUCCESS){
        ESP_LOGI(APP_NAME, "Message sent");
    }else{
        ESP_LOGE(APP_NAME, "Failed to send message");
    }
}

void esp_now_init(){
    queue=xQueueCreate(QUEUE_LENGTH, Q_ELEMENT_SIZE);
    if(queue==NULL){
        ESP_LOGE(APP_NAME, "Failed to create queue");
        return;
    }
    ESP_LOGI(APP_NAME, "Queue created");

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(mac_receiver));
    ESP_ERROR_CHECK(esp_now_register_send_cb(mac_sender));

    set_broadcast_trasmission();
    memset(central_mac, 0, Q_ELEMENT_SIZE*ESP_NOW_ETH_ALEN);
    retrieve_mac();
}