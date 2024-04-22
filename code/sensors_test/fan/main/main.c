#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "esp_log.h"

#define APP "FAN_TEST"

/*
HW CONFIG:
    - yellow = fan speed -> 36
    - red = 12V -> 5V (not enough power)
    - black = GND -> GND
*/

#define FAN_PIN 36

TaskHandle_t taskHandler0 = NULL;

void fan_handle(){
    int cnt = 0;

    while(1){
        cnt = !cnt;
        gpio_set_level(FAN_PIN, cnt);

        ESP_LOGI(APP, "Fan: %d", cnt);
        vTaskDelay(2500 / portTICK_PERIOD_MS);
    }
} 

void app_main(void){
    esp_rom_gpio_pad_select_gpio(FAN_PIN);
    gpio_set_direction(FAN_PIN, GPIO_MODE_OUTPUT);
    
    xTaskCreate(fan_handle, "fan_test", 4096, NULL, 10, &taskHandler0);
    
}
