#include "common.h"

#include "esp_now.h"
#include "esp_task.h"
#include "esp_mac.h"

#define ESPNOW "[ESP-NOW]"
#define CENTRAL_MAC 1
#define CENTRAL_WAKE 2
#define CENTRAL_VALUE 3
#define HELPER_MAC 4
#define HELPER_VALUE 5

#define QUEUE_LENGTH 10
#define DELAY (2500 / portTICK_PERIOD_MS)
#define TYPE_SIZE sizeof(int)
#define MESSAGE_SIZE 128

uint8_t central_mac[ESP_NOW_ETH_ALEN]={0};
uint8_t helper_mac[ESP_NOW_ETH_ALEN]={0};

QueueHandle_t queue;

typedef struct{
    int type;
    char* payload;
} message_t;
#define MSG_STRUCT_SIZE sizeof(message_t)
#define Q_ELEMENT_SIZE MSG_STRUCT_SIZE

char* payload="";
message_t packet_send={0};
message_t packet_received={0};

void esp_now_utils_handle_error(esp_err_t err){
    if(err != ESP_OK){
        ESP_LOGE(ESPNOW, "Error %d", err);
    }
}

void packet_build(message_t* packet, const int type, const char* payload){
    // memset(packet, 0, MSG_STRUCT_SIZE);
    packet->type=type;
    strcpy(packet->payload, payload);
    // packet->payload[MESSAGE_SIZE-1]='\0';
    ESP_LOGI(ESPNOW, "Sending message: [%d - %s]", packet->type, packet->payload);
}

void esp_now_tx(void* params){
    // memset(&packet_send, 0, MSG_STRUCT_SIZE);
    packet_send=*((message_t*) params);
    switch(packet_send.type){
        case CENTRAL_MAC:
            ESP_LOGI(ESPNOW, "Sending central MAC address");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
            ESP_LOGI(ESPNOW, "Waitinig callback function...");
            break;
        case CENTRAL_WAKE:
            ESP_LOGI(ESPNOW, "Sending sampling instructions");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
            break;
        case CENTRAL_VALUE:
            ESP_LOGI(ESPNOW, "Sending average data sampled");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
            vTaskDelete(NULL);
            break;
        default:
            ESP_LOGW(ESPNOW, "Sending unexpected packet");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
            break;
    }
}

// void value_tx(){
//     //IF DONE ON A TASK, CREATE THE STRUCT FOR PASSING THE VALUE
//     ESP_LOGI(ESPNOW, "Communicating the average value of data sampled");
//     memset(payload, 0, MAX_MSG_LEN);
//     sprintf(payload, "value");
//     packet_build(CENTRAL_VALUE, payload);
//     esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));

// }

// void start_sample_tx(){
//     ESP_LOGI(ESPNOW, "Starting sample communication to helper");
//     memset(payload, 0, MAX_MSG_LEN);
//     sprintf(payload, "Start sampling");
//     packet_build(payload, CENTRAL_WAKE);
//     esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
// }

// void esp_now_mac_tx(){
//     ESP_LOGE(ESPNOW, "Mac transmission");
//     memset(payload, 0, MESSAGE_SIZE);
//     sprintf(payload, "%02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
//     packet_build(payload, CENTRAL_MAC);
//     esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)&packet_send, MSG_STRUCT_SIZE));
// }

void esp_now_rx(void* helper_avg){
    memset(&packet_received, 0, MSG_STRUCT_SIZE);
    if(xQueueReceive(queue, &packet_received, portMAX_DELAY)){
        ESP_LOGI(ESPNOW, "Packet correctly received");
        switch(packet_received.type){
            case HELPER_MAC:
                ESP_LOGI(ESPNOW, "Received packet with helper MAC address");
                char* token=strtok(packet_received.payload, ":");
                uint8_t value=0;
                for(int i=0; i<ESP_NOW_ETH_ALEN; i++){
                    value=atoi(token);
                    helper_mac[i]=value;
                    token=strtok(NULL, ":");
                }
                ESP_LOGI(ESPNOW, "Received helper mac address %02x:%02x:%02x:%02x:%02x:%02x", helper_mac[0], helper_mac[1], helper_mac[2], helper_mac[3], helper_mac[4], helper_mac[5]);
                break;
            case HELPER_VALUE:
                ESP_LOGI(ESPNOW, "Received packet with helper data sampled average");
                float* helper_average=(float*)helper_avg;
                *helper_average=atof(packet_received.payload);
                vTaskDelete(NULL);
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
void receiver_cb(const uint8_t* mac, const uint8_t* data, int len){
    // memcpy(&packet_received, data, sizeof(*data));
    xQueueSend(queue, data, portMAX_DELAY);
    // ESP_LOGI(ESPNOW, "Received: [%d - %s]", data->type, data->payload);
}

//CALLBACK FUNCTION FOR MESSAGES SENDING
void sender_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    if(status==ESP_NOW_SEND_SUCCESS){
        ESP_LOGI(ESPNOW, "Message sent");
        // memset(msg, 0, MAX_MSG_LEN);
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
    esp_now_utils_handle_error(esp_now_register_recv_cb(receiver_cb));
    esp_now_utils_handle_error(esp_now_register_send_cb(sender_cb));

    set_broadcast_trasmission();
    memset(&central_mac, 0, sizeof(uint8_t)*ESP_NOW_ETH_ALEN);
    retrieve_mac();
}