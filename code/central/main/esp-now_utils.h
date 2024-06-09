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
    char payload[MESSAGE_SIZE];
} message_t;
#define MSG_STRUCT_SIZE sizeof(message_t)
#define Q_ELEMENT_SIZE MSG_STRUCT_SIZE

char payload[MESSAGE_SIZE];
message_t packet_send={0};
char* token;

void esp_now_utils_handle_error(esp_err_t err){
    if(err != ESP_OK){
        ESP_LOGE(ESPNOW, "Error %d", err);
    }
}

void packet_build(message_t* packet, const int type, const char* payload){
    // memset(packet, 0, MSG_STRUCT_SIZE);
    packet_send.type=type;
    strcpy(packet_send.payload, payload);
    // packet->payload[MESSAGE_SIZE-1]='\0';
    ESP_LOGI(ESPNOW, "Sending message: [%d - %s]", packet_send.type, packet_send.payload);
}

void esp_now_tx(void* params){
    switch(((message_t*)params)->type){
        case CENTRAL_MAC:
            ESP_LOGI(ESPNOW, "Sending central MAC address");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)params, MSG_STRUCT_SIZE));
            ESP_LOGI(ESPNOW, "Waitinig callback function...");
            break;
        case CENTRAL_WAKE:
            ESP_LOGI(ESPNOW, "Sending sampling instructions");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)params, MSG_STRUCT_SIZE));
            break;
        case CENTRAL_VALUE:
            ESP_LOGI(ESPNOW, "Sending average data sampled");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)params, MSG_STRUCT_SIZE));
            break;
        default:
            ESP_LOGW(ESPNOW, "Sending unexpected packet");
            esp_now_utils_handle_error(esp_now_send(helper_mac, (uint8_t*)params, MSG_STRUCT_SIZE));
            break;
    }
    vTaskDelay(10000/portTICK_PERIOD_MS);
}

void set_peer(esp_now_peer_info_t* peer,const uint8_t* mac_address){
    if(peer && mac_address){
        memcpy(peer->peer_addr, helper_mac, ESP_NOW_ETH_ALEN);
        peer->channel=ESP_NOW_CHANNEL;
        peer->encrypt=false;

        esp_now_utils_handle_error(esp_now_add_peer(peer));
    }
}

void esp_now_rx(float* helper_air, float* helper_tmp){
    message_t packet_received={0};
    if(xQueueReceive(queue, &packet_received, portMAX_DELAY)){
        ESP_LOGI(ESPNOW, "Packet correctly received");
        switch(packet_received.type){
            case HELPER_MAC:
                ESP_LOGI(ESPNOW, "Received packet with helper MAC address");
                token=strtok(packet_received.payload, ":");
                unsigned int value=0;
                for(int i=0; i<ESP_NOW_ETH_ALEN; i++){
                    sscanf(token, "%02x", &value);
                    helper_mac[i]=value;
                    token=strtok(NULL, ":");
                }
                ESP_LOGI(ESPNOW, "Received helper mac address %02x:%02x:%02x:%02x:%02x:%02x", helper_mac[0], helper_mac[1], helper_mac[2], helper_mac[3], helper_mac[4], helper_mac[5]);

                esp_now_peer_info_t* peer=(esp_now_peer_info_t*)malloc(sizeof(esp_now_peer_info_t));
                if(peer==NULL){
                    ESP_LOGE(ESPNOW, "Memory allocation failed");
                    return;
                }
                memset(peer, 0, sizeof(esp_now_peer_info_t));
                set_peer(peer, helper_mac);
                free(peer);
                break;
            case HELPER_VALUE:
                ESP_LOGI(ESPNOW, "Received packet with helper data sampled average");
                token=strtok(packet_received.payload, "$");
                sscanf(token, "%f", helper_air);
                token=strtok(NULL, "$");
                sscanf(token, "%f", helper_tmp);
                ESP_LOGI(ESPNOW, "Values received: %f - %f", (*helper_air), (*helper_tmp));
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
    ESP_LOGI(ESPNOW, "Callback");
    message_t rec;
    memcpy(&rec, data, len);
    xQueueSend(queue, &rec, portMAX_DELAY);
    ESP_LOGI(ESPNOW, "Received: [%d - %s]", rec.type, rec.payload);
}

//CALLBACK FUNCTION FOR MESSAGES SENDING
void sender_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    if(status==ESP_NOW_SEND_SUCCESS){
        ESP_LOGI(ESPNOW, "Message sent");
        // memset(msg, 0, MAX_MSG_LEN);
    }else{
        ESP_LOGE(ESPNOW, "Failed to send message - status: %d", status);
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
    esp_now_utils_handle_error(esp_event_loop_create_default());
    esp_now_utils_handle_error(esp_now_register_recv_cb(receiver_cb));
    esp_now_utils_handle_error(esp_now_register_send_cb(sender_cb));
    esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
}