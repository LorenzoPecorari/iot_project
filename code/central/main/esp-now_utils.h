#include "common.h"

#include "esp_now.h"
#include "esp_task.h"
#include "esp_mac.h"

#define ESPNOW "[ESP-NOW]"
#define CENTRAL_MAC 1
#define HELPER_MAC 2
#define CENTRAL_WAKE 3
#define HELPER_WAKE 4
#define CENTRAL_VALUE 5
#define HELPER_VALUE 6

#define QUEUE_LENGTH 10
#define Q_ELEMENT_SIZE sizeof(uint8_t)
#define DELAY (2500 / portTICK_PERIOD_MS)
#define TYPE_SIZE sizeof(int)
#define MESSAGE_SIZE 128

uint8_t central_mac[ESP_NOW_ETH_ALEN]={0};
uint8_t helper_mac[ESP_NOW_ETH_ALEN]={0};

QueueHandle_t queue;

struct message_t{
    char type[TYPE_SIZE];
    char payload[MESSAGE_SIZE];
} message_t;
#define MSG_STRUCT_SIZE sizeof(message_t)

char* payload;
message_t packet_send={0};
message_t packet_received={0};

void esp_now_utils_handle_error(esp_err_t err){
    if(err != ESP_OK){
        ESP_LOGE(ESPNOW, "Error %d", err);
    }
}

void packet_build(const char* type, const char* payload){
    memset(packet_send, 0, MSG_STRUCT_SIZE);
    strncpy(packet_send.type, type, TYPE_SIZE-1);
    packet_send.type[TYPE_SIZE-1]='\0';
    strncpy(packet_send.payload, payload, MESSAGE_SIZE);
    packet_send.payload[MESSAGE_SIZE-1]='\0';
    ESP_LOGI(ESPNOW, "Sending message: [%s - %s]", packet_send.type, packet_send.payload);
}

void esp_now_tx(){
    
}

void value_tx(){
    //IF DONE ON A TASK, CREATE THE STRUCT FOR PASSING THE VALUE
    ESP_LOGI(ESPNOW, "Communicating the average value of data sampled");
    memset(payload, 0, MAX_MSG_LEN);
    sprintf(payload, "value");
    packet_build(CENTRAL_VALUE, payload);
    esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));

}

void start_sample_tx(){
    ESP_LOGI(ESPNOW, "Starting sample communication to helper");
    memset(payload, 0, MAX_MSG_LEN);
    sprintf(payload, "Start sampling");
    packet_build(payload, CENTRAL_WAKE);
    esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
}

void esp_now_mac_tx(){
    ESP_LOGE(ESPNOW, "Mac transmission");
    memset(payload, 0, MESSAGE_SIZE);
    sprintf(payload, "%02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
    packet_build(payload, CENTRAL_MAC);
    esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
}

void esp_now_rx(){
    memset(packet_received, 0, MSG_STRUCT_SIZE);
    if(xQueueReceive(queue, &packet_received, portMAX_DELAY)){
        ESP_LOGI(ESPNOW, "Packet correctly received");
        switch(packet_received.type){
            case HELPER_MAC:
                char* token=strtok(packet_received.payload, ":");
                uint8_t value=0;
                for(int i=0; i<ESP_NOW_ETH_ALEN; i++){
                    sscanf(token, "%02x", &t);
                    helper_mac[i]=t;
                    token=strtok(NULL, ":");
                }
                ESP_LOGI(ESPNOW, "Received hellper mac address %02x:%02x:%02x:%02x:%02x:%02x", helper_mac[0], helper_mac[1], helper_mac[2], helper_mac[3], helper_mac[4], helper_mac[5]);
                break;
            case HELPER_WAKE:
                //CHECK IF START THE SAMPLING
                break;
            case HELPER_VALUE:
                break;
            default:
                ESP_LOGW(ESPNOW, "Received an unexpected packet");
                break;
        }
    }else{
        ESP_LOGE(ESPNOW, "Can't received a packet");
    }
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