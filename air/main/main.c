#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "esp_log.h"

#define APP "air"

/*
HW CONFIG:
    - yellow = fan speed -> 36
    - red = 12V -> 5V (not enough power)
    - black = GND -> GND
*/


#define FAN_PIN 36

TaskHandle_t taskHandler0 = NULL;

void Hello_world(void* arg){
    int cnt = 0;

    while(1){
        ESP_LOGI(APP, "Hello from this new project!");
        cnt = !cnt;
        gpio_set_level(FAN_PIN, cnt);

        ESP_LOGI(APP, "Fan: %d!", cnt);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
} 

void app_main(void){
    esp_rom_gpio_pad_select_gpio(FAN_PIN);
    gpio_set_direction(FAN_PIN, GPIO_MODE_OUTPUT);
    
    xTaskCreate(Hello_world, "hello_world", 4096, NULL, 10, &taskHandler0);
    
}
