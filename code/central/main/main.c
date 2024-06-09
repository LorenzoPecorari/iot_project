#include "esp-now_utils.h"
#include "wifi_utils.h"
#include "microphone_utils.h"
#include "air-detection_utils.h"
#include "mqtt_utils.h"

#define APP_NAME "[CENTRAL]"

TaskHandle_t airTaskHandle=NULL;
TaskHandle_t waitTaskHandle=NULL;
TaskHandle_t mqttTaskHandle=NULL;
TaskHandle_t notifyTaskHandle=NULL;

message_t* packet;
float helper_average_avg;
float helper_temperature_avg;

void set_peer_custom(){
    esp_now_peer_info_t* peer=(esp_now_peer_info_t*)malloc(sizeof(esp_now_peer_info_t));
    if(peer==NULL){
        ESP_LOGE(ESPNOW, "Memory allocation failed");
        return;
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    
    memcpy(peer->peer_addr, helper_mac, ESP_NOW_ETH_ALEN);
    peer->channel=ESP_NOW_CHANNEL;
    peer->encrypt=false;

    esp_now_utils_handle_error(esp_now_mod_peer(peer));
    
    free(peer);
}

void custom_esp_now_init(){
    espnow_init();
    set_mac(helper_mac);
    // set_peer_custom();
}

void light_sleep_custom(){
    esp_light_sleep_start();
    esp_now_deinit();
    custom_esp_now_init();
}

//CENTRAL DEVICE MAIN FILE
void app_main(void){
    //ELEMENTS INITIALIZATION
    ESP_LOGI(APP_NAME, "Start elements initialization");
    wifi_init();
    espnow_init();
    set_broadcast_trasmission();
    memset(&central_mac, 0, sizeof(uint8_t)*ESP_NOW_ETH_ALEN);
    retrieve_mac();
    // mqtt_init();
    microphone_init();
    airdetection_init();
    ESP_LOGI(APP_NAME, "Elements initialization completed");
    packet=(message_t*)malloc(MSG_STRUCT_SIZE);
    // packet->payload="";
    // vTaskDelay(10000/portTICK_PERIOD_MS);

    //MAC SHARING TO HELPER DEVICES
    ESP_LOGI(APP_NAME, "Start mac exchange");
    // memset(payload, 0, MESSAGE_SIZE);
    sprintf(payload, "%02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
    packet_build(packet, CENTRAL_MAC, payload);
    esp_now_tx((void*)&packet_send);

    //WAITING FOR HELPER DEVICE MAC
    esp_now_rx(NULL, NULL);
    ESP_LOGI(APP_NAME, "Mac exchange completed");

    while(1){
        //CHECK PERSONZ (CIT) INSIDE THE CLASSROOM
        ESP_LOGI(APP_NAME, "Checking people presence");
        
        // set channel for esp now communication
        esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
        set_peer_custom();
        
        if(check_people()){
            //START SAMPLE
            ESP_LOGI(APP_NAME, "People detected");
            memset(payload, 0, MESSAGE_SIZE);
            sprintf(payload, "%d", 1);
            packet_build(packet, CENTRAL_WAKE, payload);
            esp_now_tx((void*) &packet_send); // invio dati su presenza persone

            //START AIR DETECTION AND WATING FOR HELPERS DATA
            ESP_LOGI(APP_NAME, "Waiting for helpers data");
            esp_now_rx(&helper_average_avg, &helper_temperature_avg);
            ESP_LOGI(APP_NAME, "Starting air detection");
            air_detection();

            //WAIT FOR THE END OF BOTH TASKS
            ESP_LOGI(APP_NAME, "Air task and data waiting task completed");

            //START ELABORATION
            ESP_LOGI(APP_NAME, "Starting data eleboration");
            elaboration(helper_average_avg, helper_temperature_avg);
            memset(payload, 0, MESSAGE_SIZE);
            sprintf(payload, "%.2f$%.2f", general_avg_air, general_avg_temp);
            packet_build(packet, CENTRAL_VALUE, payload);

            //START MQTT DATA COMMUNICATION AND HELPERS NOTIFY
            ESP_LOGI(APP_NAME, "Notifying helpers");
            esp_now_tx((void*) &packet_send);
            // ESP_LOGI(APP_NAME, "Starting mqtt communication");
            esp_wifi_set_channel(MQTT_CHANNEL, WIFI_SECOND_CHAN_NONE);
            esp_wifi_connect();
            vTaskDelay(10000/portTICK_PERIOD_MS);
            mqtt_init();
            mqtt_transmission(general_avg_air, general_avg_temp);
            esp_mqtt_client_stop(client);

            ESP_LOGI(APP_NAME, "Mqtt communication task and helpers notify task completed");
            wifi_deinit_custom();
            wifi_init();
            esp_now_deinit();
            custom_esp_now_init();

            //LIGHT SLEEP MODE
            // esp_sleep_enable_timer_wakeup(2 * 1000 *1000);
            // light_sleep_custom();
            // // vTaskDelay(2000/portTICK_PERIOD_MS);
            // ESP_LOGI(APP_NAME, "Restarting loop...");
            // wifi_reinit();
            // esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
        }else{
            //SLEEP
            ESP_LOGI(APP_NAME, "Nobody detected");
            memset(payload, 0, MESSAGE_SIZE);
            sprintf(payload, "%d", 1);
            packet_build(packet, CENTRAL_WAKE, payload);
            esp_now_tx((void*) &packet_send);
            // esp_sleep_enable_timer_wakeup(90000000);
            // esp_light_sleep_start();
        }        
    }
}