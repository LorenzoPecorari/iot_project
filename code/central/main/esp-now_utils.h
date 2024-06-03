#include "common.h"

#include "esp_now.h"
#include "esp_task.h"
#include "esp_mac.h"

#define ESPNOW "[ESP-NOW]"
#define QUEUE_LENGTH 10
#define Q_ELEMENT_SIZE sizeof(uint8_t)
#define DELAY (2500 / portTICK_PERIOD_MS)
#define MAX_MSG_LEN 250

uint8_t central_mac[ESP_NOW_ETH_ALEN]={0};
uint8_t helper_mac[ESP_NOW_ETH_ALEN]={0};

QueueHandle_t queue;
char* msg=calloc(MAX_MSG_LEN, sizeof(char));
char* data_rx=calloc(MAX_MSG_LEN, sizeof(char));

void esp_now_utils_handle_error(esp_err_t err){
    if(err != ESP_OK){
        ESP_LOGE(ESPNOW, "Error %d", err);
    }
}

void helpers_notify(){
    // ESP_LOGI(ESPNOW, "Notifying helpers devices with data computed");
    // sprintf(msg, "");
    // esp_now_utils_handle_error(esp_now_send(helper_mac, msg, MAX_MSG_LEN));
}

void start_sample_tx(){
    ESP_LOGI(ESPNOW, "Starting sample communication");
    sprintf(msg, "Start sample");
    esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t)msg, MAX_MSG_LEN));
}

void helpers_data_rx(){
    memset(data_rx, 0, MAX_MSG_LEN);
    xQueueReceive(queue, &data_rx, portMAX_DELAY);
    ESP_LOGI(ESPNOW, "Helpers data received: %s", data_rx);
}

void esp_now_mac_tx(){
    ESP_LOGE(ESPNOW, "Mac transmission");
    sprintf(msg, "%02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
    esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t)msg, MAX_MSG_LEN));
    // int count=0;
    // while(count<ESP_NOW_ETH_ALEN){
    //     esp_now_send(helper_mac, central_mac[count], Q_ELEMENT_SIZE);
    //     vTaskDelay(DELAY);
    //     count++;
    // }
}

void esp_now_mac_rx(){
    memset(helper_mac, 0, Q_ELEMENT_SIZE*ESP_NOW_ETH_ALEN);
    // int count=0;
    // while(count<ESP_NOW_ETH_ALEN){
    //     xQueueReceive(queue, &helper_mac[count], portMAX_DELAY);
    //     count++;
    // }
    xQueueReceive(queue, &helper_mac, portMAX_DELAY);
    ESP_LOGI(ESPNOW, "Mac address received: %02x:%02x:%02x:%02x:%02x:%02x\n", helper_mac[0], helper_mac[1], helper_mac[2], helper_mac[3], helper_mac[4], helper_mac[5]);
}

void retrieve_mac(){
    // if(!central_mac){
    //     ESP_LOGE(ESPNOW, "Mac retrieving error");
    // }

    esp_now_utils_handle_error(esp_read_mac(central_mac, ESP_MAC_WIFI_STA));
    ESP_LOGI(ESPNOW, "CENTRAL MAC: %02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
}

void set_peer(esp_now_peer_info_t* peer,const uint8_t* mac_address){
    if(peer && mac_address){
        memcpy(peer->peer_addr, helper_mac, ESP_NOW_ETH_ALEN);
        peer->channel=ESP_NOW_CHANNEL;
        peer->encrypt=false;

        esp_now_utils_handle_error(esp_now_add_peer(peer));
    }
}

void set_mac(const uint8_t* mac_address){
    if(!mac_address){
        ESP_LOGE(ESPNOW, "Invalid mac pointer");
    }else{
        memcpy(helper_mac, mac_address, ESP_NOW_ETH_ALEN);

        esp_now_peer_info_t* peer=(esp_now_peer_info_t*)malloc(sizeof(esp_now_peer_info_t));
        if(peer==NULL){
            ESP_LOGE(ESPNOW, "Memory allocation failed");
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
    ESP_LOGI(ESPNOW, "Received: %d", data[0]);
}

//CALLBACK FUNCTION FOR MESSAGES SENDING
void sender_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    if(status==ESP_NOW_SEND_SUCCESS){
        ESP_LOGI(ESPNOW, "Message sent");
        memset(msg, 0, MAX_MSG_LEN);
    }else{
        ESP_LOGE(ESPNOW, "Failed to send message");
    }
}

void espnow_init(){
    queue=xQueueCreate(QUEUE_LENGTH, Q_ELEMENT_SIZE);
    if(queue==NULL){
        ESP_LOGE(ESPNOW, "Failed to create queue");
        return;
    }
    ESP_LOGI(ESPNOW, "Queue created");

    esp_now_utils_handle_error(esp_now_init());
    esp_now_utils_handle_error(esp_now_register_recv_cb(sender_cb));
    esp_now_utils_handle_error(esp_now_register_send_cb(receiver_cb));

    set_broadcast_trasmission();
    memset(central_mac, 0, Q_ELEMENT_SIZE*ESP_NOW_ETH_ALEN);
    retrieve_mac();
}