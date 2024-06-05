#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_sleep.h"
#include "esp_task.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define APP_NAME "[ESP-NOW] "

#define Q_LENGTH 10

#define SIZE sizeof(uint8_t)
#define DELAY (2500 / portTICK_PERIOD_MS)
#define CHANNEL 0
#define TYPE_SIZE 16
#define MESSAGE_SIZE 128

struct message_str {
    char type[TYPE_SIZE];
    char payload[MESSAGE_SIZE];
} message_str;

#define STRUCT_SIZE sizeof(message_str)

int got_other_mac = 0;
int what_to_do = 0;

uint8_t other_mac[ESP_NOW_ETH_ALEN] = {0};
uint8_t this_mac[ESP_NOW_ETH_ALEN] = {0};

static QueueHandle_t queue = NULL;

void esp_now_utils_handle_error(esp_err_t err) {
    if (err != ESP_OK) {
        ESP_LOGE(APP_NAME, "Error %d", err);
    }
}

void wifi_init() {
    esp_now_utils_handle_error(esp_netif_init());
    wifi_init_config_t init_conf = WIFI_INIT_CONFIG_DEFAULT();
    esp_now_utils_handle_error(esp_wifi_init(&init_conf));
    esp_now_utils_handle_error(esp_wifi_set_mode(WIFI_MODE_STA));
    esp_now_utils_handle_error(esp_wifi_set_ps(WIFI_PS_MIN_MODEM));
    esp_now_utils_handle_error(esp_sleep_enable_wifi_wakeup());
    esp_now_utils_handle_error(esp_wifi_start());
    esp_now_utils_handle_error(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));
}

void send_message(const char* type, const char* payload) {
    struct message_str packet = {0};
    strncpy(packet.type, type, TYPE_SIZE - 1);
    packet.type[TYPE_SIZE - 1] = '\0';
    strncpy(packet.payload, payload, MESSAGE_SIZE - 1);
    packet.payload[MESSAGE_SIZE - 1] = '\0';

    ESP_LOGI(APP_NAME, "Sending: [%s - %s]\n", packet.type, packet.payload);

    esp_err_t result = esp_now_send(other_mac, (uint8_t*)&packet, sizeof(packet));
    if (result == ESP_OK) {
        ESP_LOGI(APP_NAME, "Message sent successfully");
    } else {
        ESP_LOGE(APP_NAME, "Failed to send message: %s", esp_err_to_name(result));
    }
}

void send_mac(const char* type) {
    char mac_as_str[18];
    sprintf(mac_as_str, "%02x:%02x:%02x:%02x:%02x:%02x", this_mac[0], this_mac[1], this_mac[2], this_mac[3], this_mac[4], this_mac[5]);
    send_message(type, mac_as_str);
}

void set_peer(esp_now_peer_info_t* peer, uint8_t* mac) {
    if (peer && mac) {
        memcpy(peer->peer_addr, mac, ESP_NOW_ETH_ALEN);
        peer->channel = CHANNEL;
        peer->encrypt = false;
        esp_now_utils_handle_error(esp_now_add_peer(peer));
    }
}

void set_mac(uint8_t* mac) {
    if (!mac)
        ESP_LOGE(APP_NAME, "Invalid mac pointer");
    else {
        memcpy(other_mac, mac, ESP_NOW_ETH_ALEN);
        esp_now_peer_info_t *peer = (esp_now_peer_info_t*) malloc(sizeof(esp_now_peer_info_t));
        if (peer == NULL) {
            ESP_LOGE(APP_NAME, "Memory allocation : fail");
            return;
        }
        memset(peer, 0, sizeof(esp_now_peer_info_t));
        set_peer(peer, other_mac);
        free(peer);
    }
}

void consume_message() {
    struct message_str received_message;
    if (xQueueReceive(queue, &received_message, portMAX_DELAY)) {
        
        ESP_LOGI(APP_NAME, "Received: [%s - %s]", received_message.type, received_message.payload);
        
        if (!strcmp(received_message.type, "MAC_ID")) {
            got_other_mac = 0;

            char copied[MESSAGE_SIZE];
            strncpy(copied, received_message.payload, MESSAGE_SIZE - 1);
            copied[MESSAGE_SIZE - 1] = '\0';
            
            char* token;
            token = strtok(copied, ":");
            unsigned int t = 0;
            
            for (int i = 0; token != NULL && i < ESP_NOW_ETH_ALEN; i++) {
                sscanf(token, "%02x", &t);
                token = strtok(NULL, ":");
                other_mac[i] = (uint8_t) t;
                printf("%02x\n", other_mac[i]);
            }

            // it waits a second after receiving MAC then it sends its own
            set_mac(other_mac);
            got_other_mac = 1;
        } 
        else if(!strcmp(received_message.type, "MAC_REPLY")){
            char copied[MESSAGE_SIZE];
            strncpy(copied, received_message.payload, MESSAGE_SIZE - 1);
            copied[MESSAGE_SIZE - 1] = '\0';
            
            char* token;
            token = strtok(copied, ":");
            unsigned int t = 0;
            
            for (int i = 0; token != NULL && i < ESP_NOW_ETH_ALEN; i++) {
                sscanf(token, "%02x", &t);
                token = strtok(NULL, ":");
                other_mac[i] = (uint8_t) t;
            }

            set_mac(other_mac);
            got_other_mac = 1;
        }
        else if (!strcmp(received_message.type, "WAKE_REQ")) {
            if(1){  // TODO : it is needed to define the variable related to noises
                send_message("WAKE_REP", "This has to contain something about values");
            }
        }
        else if (!strcmp(received_message.type, "WAKE_REP")) {
            if(1){
                ;// TODO : implement something here
            }
            what_to_do = 1;
        }
        else if (!strcmp(received_message.type, "VALUE_REQ")) {
            ESP_LOGI(APP_NAME, "Some values are being sent by other node");
            send_message("VALUE_REP", "0");
        }
        else if (!strcmp(received_message.type, "VALUE_REP")) {
            ESP_LOGI(APP_NAME, "Some values are being sent by other node");
            // TODO : something to implement here 
        }
        else {
            ESP_LOGI(APP_NAME, "Unknown type, dropping...");
        }
    }
}

void recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    struct message_str received_message;
    memcpy(&received_message, data, len);    

    esp_now_utils_handle_error(esp_sleep_enable_wifi_wakeup());
    xQueueSend(queue, &received_message, portMAX_DELAY);

    consume_message();
}

void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS)
        ESP_LOGE(APP_NAME, "Send failed");
}

void retrieve_mac() {
    esp_now_utils_handle_error(esp_read_mac(this_mac, ESP_MAC_WIFI_STA));
}

void set_broadcast_mac() {
    uint8_t addr[ESP_NOW_ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    set_mac(addr);
}

void custom_esp_now_init(){
    esp_now_utils_handle_error(esp_now_init());
    esp_now_utils_handle_error(esp_event_loop_create_default());
    esp_now_utils_handle_error(esp_now_register_recv_cb(recv_cb));
    esp_now_utils_handle_error(esp_now_register_send_cb(send_cb));
    
    set_mac(other_mac);
}

void init_esp_now() {
    queue = xQueueCreate(Q_LENGTH, STRUCT_SIZE);

    set_broadcast_mac();
    retrieve_mac();

    custom_esp_now_init();    
}

void light_sleep_custom(){
    esp_light_sleep_start();
    esp_now_deinit();
    custom_esp_now_init();
}
