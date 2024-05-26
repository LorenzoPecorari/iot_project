#include "common.h"

#include "esp_now.h"
#include "esp_task.h"
#include "esp_mac.h"

#define APP_NAME_ESP_NOW "[ESP-NOW] "

#define Q_LENGTH 10
#define SIZE sizeof(uint8_t)
#define DELAY (2500 / portTICK_PERIOD_MS)

QueueHandle_t queue;

uint8_t central_mac[ESP_NOW_ETH_ALEN] = {0}; // initially broadcast, later specific address
uint8_t helper_mac[ESP_NOW_ETH_ALEN] = {0};

// "handles" the return value of a function
void esp_now_utils_handle_error(esp_err_t err){
    if(err != ESP_OK)
        ESP_LOGE(APP_NAME_ESP_NOW, "Error %d", err);
}

// callback function for messges receiving
void receive_cb(const uint8_t *central_mac, const uint8_t *data, int len){
    xQueueSend(queue, data, len);
    ESP_LOGI(APP_NAME_ESP_NOW, "Received : %d", data[0]);
}

// callback function for messages sending
void send_cb(const uint8_t *central_mac, esp_now_send_status_t status){
    if (status != ESP_NOW_SEND_SUCCESS)
        ESP_LOGE(APP_NAME_ESP_NOW, "Send failed");
}

// retrives the mac address of the device (wifi module)
void retrieve_mac(){
    esp_now_utils_handle_error(esp_read_mac(helper_mac, ESP_MAC_WIFI_STA));
    ESP_LOGI(APP_NAME_ESP_NOW, "MAC: %02x:%02x:%02x:%02x:%02x:%02x", helper_mac[0], helper_mac[1], helper_mac[2], helper_mac[3], helper_mac[4], helper_mac[5]);
}

// sets the peer with certain mac passed as argoument of the function
void set_peer(esp_now_peer_info_t* peer, uint8_t* mac_address){
    if(peer && mac_address){
        memcpy(peer->peer_addr, central_mac, ESP_NOW_ETH_ALEN);
        peer->channel = CHANNEL;
        peer->encrypt = false;
        
        esp_now_utils_handle_error(esp_now_add_peer(peer));
    }
}

// sets the mac of the peer through set_peer()
void set_mac(uint8_t* mac_address){
    if(!mac_address)
        ESP_LOGE(APP_NAME_ESP_NOW, "Invalid mac pointer");
    else{
        memcpy(central_mac, mac_address, ESP_NOW_ETH_ALEN);
        
        esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    
        if (peer == NULL) {
            ESP_LOGE(APP_NAME_ESP_NOW, "Memory allocation failed");
            return;
        }

        memset(peer, 0, sizeof(esp_now_peer_info_t));
        set_peer(peer, mac_address);
        free(peer);
    }

}

// sets the peer in order to sends and receive messages in broadcast
void set_broadcast_mac(){
    uint8_t addr[ESP_NOW_ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,}; // broadcast mac
    set_mac(addr);
}

// transmits its own broadcast
void esp_now_mac_tx(){
    ESP_LOGI(APP_NAME_ESP_NOW, "Sending mac to... %02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
    int cnt = 0;

    while(cnt < ESP_NOW_ETH_ALEN){
        esp_now_send(central_mac, &helper_mac[cnt], SIZE);
        ESP_LOGI(APP_NAME_ESP_NOW, "Sent: %d", helper_mac[cnt]);
        vTaskDelay(DELAY);
        cnt++;
    }
}

// receives the mac address of the other peer
void esp_now_mac_rx(){
    memset(central_mac, 0, SIZE * ESP_NOW_ETH_ALEN);
    int cnt = 0;

    queue = xQueueCreate(Q_LENGTH, SIZE);

    // receives the mac address from the other peer
    while(cnt < ESP_NOW_ETH_ALEN){
        xQueueReceive(queue, &central_mac[cnt], portMAX_DELAY);
        cnt++;
    }

    ESP_LOGI(APP_NAME_ESP_NOW, "Mac address received= %02x:%02x:%02x:%02x:%02x:%02x\n", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);

    set_mac(central_mac);
}

//  creates the queue, initializes esp-now, registers the callback functions, sets the peer to which send data
void init_esp_now(){
    queue = xQueueCreate(Q_LENGTH, SIZE);

    if (queue == NULL) {
        ESP_LOGE(APP_NAME_ESP_NOW, "Failed to create queue");
        return;
    }

    esp_now_utils_handle_error(esp_now_init());
    esp_now_utils_handle_error(esp_now_register_recv_cb(receive_cb));
    esp_now_utils_handle_error(esp_now_register_send_cb(send_cb));

    set_broadcast_mac();

    memset(helper_mac, 0, SIZE * ESP_NOW_ETH_ALEN);
    retrieve_mac(helper_mac);
}

// sends sampled data to central device
void send_data(float temp_val, float air_val){
    float data[2] = {temp_val, air_val};ù

    esp_now_utils_handle_error(esp_now_send(central_mac, (uint8_t*) &data, sizeof(float) * 2));
    ESP_LOGI(APP_NAME_ESP_NOW, "Sent data: (%f, %f)", data[0], data[1]);
}
