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
float helper_average;

//CENTRAL DEVICE MAIN FILE
void app_main(void){
    //ELEMENTS INITIALIZATION
    ESP_LOGI(APP_NAME, "Start elements initialization");
    wifi_init();
    espnow_init();
    // mqtt_init();
    microphone_init();
    airdetection_init();
    ESP_LOGI(APP_NAME, "Elements initialization completed");
    packet=(message_t*)malloc(MSG_STRUCT_SIZE);
    // packet->payload="";
    vTaskDelay(10000/portTICK_PERIOD_MS);

    //MAC SHARING TO HELPER DEVICES
    ESP_LOGI(APP_NAME, "Start mac exchange");
    // memset(payload, 0, MESSAGE_SIZE);
    sprintf(payload, "%02x:%02x:%02x:%02x:%02x:%02x", central_mac[0], central_mac[1], central_mac[2], central_mac[3], central_mac[4], central_mac[5]);
    packet_build(packet, CENTRAL_MAC, payload);
    esp_now_tx((void*)&packet_send);

    //WAITING FOR HELPER DEVICE MAC
    esp_now_rx(NULL);
    ESP_LOGI(APP_NAME, "Mac exchange completed");

    while(1){
        //CHECK PERSONZ (CIT) INSIDE THE CLASSROOM
        ESP_LOGI(APP_NAME, "Checking people presence");
        if(check_people()){
            //START SAMPLE
            ESP_LOGI(APP_NAME, "People detected");
            memset(payload, 0, MESSAGE_SIZE);
            sprintf(payload, "%d", 1);
            packet_build(packet, CENTRAL_WAKE, payload);
            esp_now_tx((void*) packet);

            //START AIR DETECTION AND WATING FOR HELPERS DATA
            ESP_LOGI(APP_NAME, "Starting air detection");
            xTaskCreatePinnedToCore(air_detection, "Air detection task", 4096, NULL, 10, &airTaskHandle, 0);
            ESP_LOGI(APP_NAME, "Waiting for helpers data");
            xTaskCreatePinnedToCore(esp_now_rx, "Data waiting task", 4096, (void*) &helper_average, 10, &waitTaskHandle, 1);

            //WAIT FOR THE END OF BOTH TASKS
            while(eTaskGetState(airTaskHandle)==eRunning || eTaskGetState(waitTaskHandle)==eRunning){
                vTaskDelay(10000/portTICK_PERIOD_MS);
            }
            ESP_LOGI(APP_NAME, "Air task and data waiting task completed");

            //START ELABORATION
            ESP_LOGI(APP_NAME, "Starting data eleboration");
            elaboration(helper_average);
            memset(payload, 0, MESSAGE_SIZE);
            sprintf(payload, "%f", general_avg);
            packet_build(packet, CENTRAL_VALUE, payload);

            //START MQTT DATA COMMUNICATION AND HELPERS NOTIFY
            ESP_LOGI(APP_NAME, "Starting mqtt communication");
            xTaskCreatePinnedToCore(mqtt_transmission, "Mqtt task", 4096, (void*)&general_avg, 10, &mqttTaskHandle, 0);
            ESP_LOGI(APP_NAME, "Notifying helpers");
            xTaskCreatePinnedToCore(esp_now_tx, "Notify task", 4096, (void*) packet, 10, &notifyTaskHandle, 1);

            //WAIT FOR THE END OF BOTH TASKS
            while(eTaskGetState(mqttTaskHandle)==eRunning || eTaskGetState(notifyTaskHandle)==eRunning){
                vTaskDelay(10000/portTICK_PERIOD_MS);
            }
            ESP_LOGI(APP_NAME, "Mqtt communication task and helpers notify task completed");

            //LIGHT SLEEP MODE
            esp_sleep_enable_timer_wakeup(300000000);
            esp_light_sleep_start();
        }else{
            //SLEEP
            ESP_LOGI(APP_NAME, "Nobody detected");
            memset(payload, 0, MESSAGE_SIZE);
            sprintf(payload, "%d", 1);
            packet_build(packet, CENTRAL_WAKE, payload);
            esp_now_tx((void*) packet);
            esp_sleep_enable_timer_wakeup(90000000);
            esp_light_sleep_start();
        }        
    }
}