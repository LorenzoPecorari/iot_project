#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#define APP "air"

TaskHandle_t taskHandler0 = NULL;

void Hello_world(void* arg){
    while(1){
        ESP_LOGI(APP, "Hello from this new project!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
} 

void app_main(void){
    xTaskCreate(Hello_world, "hello_world", 4096, NULL, 10, &taskHandler0);
    
}
