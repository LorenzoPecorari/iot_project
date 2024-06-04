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

//CENTRAL DEVICE MAIN FILE
void app_main(void){
    //ELEMENTS INITIALIZATION
    ESP_LOGI(APP_NAME, "Start elements initialization");
    wifi_init();
    espnow_init();
    mqtt_init();
    microphone_init();
    airdetection_init();
    ESP_LOGI(APP_NAME, "Elements initialization completed");

    //MAC SHARING TO HELPER DEVICES
    ESP_LOGI(APP_NAME, "Start mac exchange");
    esp_now_mac_tx();

    //WAITING FOR HELPER DEVICE MAC
    esp_now_mac_rx();
    ESP_LOGI(APP_NAME, "Mac exchange completed");

    while(1){
        //CHECK PERSONZ (CIT) INSIDE THE CLASSROOM
        ESP_LOGI(APP_NAME, "Checking people presence");
        check_people();

        //NOTIFY HELPERS TO START DATA SAMPLE
        start_sample_tx();

        //START AIR DETECTION AND WATING FOR HELPERS DATA
        ESP_LOGI(APP_NAME, "Starting air detection");
        xTaskCreatePinnedToCore(air_detection, "Air detection task", 4096, NULL, 10, &airTaskHandle, 0);
        ESP_LOGI(APP_NAME, "Waiting for helpers data");
        xTaskCreatePinnedToCore(helpers_data_rx, "Data waiting task", 4096, NULL, 10, &waitTaskHandle, 1);

        //WAIT FOR THE END OF BOTH TASKS
        while(eTaskGetState(airTaskHandle)==eRunning || eTaskGetState(waitTaskHandle)==eRunning){
            vTaskDelay(10000/portTICK_PERIOD_MS);
        }
        ESP_LOGI(APP_NAME, "Air task and data waiting task completed");

        //START ELABORATION
        ESP_LOGI(APP_NAME, "Starting data eleboration");
        elaboration();

        //START MQTT DATA COMMUNICATION AND HELPERS NOTIFY
        ESP_LOGI(APP_NAME, "Starting mqtt communication");
        xTaskCreatePinnedToCore(mqtt_transmission, "Mqtt task", 4096, NULL, 10, &mqttTaskHandle, 0);
        ESP_LOGI(APP_NAME, "Notifying helpers");
        xTaskCreatePinnedToCore(value_tx, "Notify task", 4096, NULL, 10, &notifyTaskHandle, 1);

        //WAIT FOR THE END OF BOTH TASKS
        while(eTaskGetState(mqttTaskHandle)==eRunning || eTaskGetState(notifyTaskHandle)==eRunning){
            vTaskDelay(10000/portTICK_PERIOD_MS);
        }
        ESP_LOGI(APP_NAME, "Mqtt communication task and helpers notify task completed");

        //LIGHT SLEEP MODE
        esp_sleep_enable_timer_wakeup(300000000);
        esp_light_sleep_start();
    }
}