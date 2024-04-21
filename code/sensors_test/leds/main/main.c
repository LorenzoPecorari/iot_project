#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define APP "LEDS_TEST"
//FILE FOR LEDS TEST


/*
        |---GPIO1---CABLE---220OHM---RED PIN---CABLE---GROUND-----|
        |                                                         |
ESP32 --|---GPIO2---CABLE---220OHM---YELLOW PIN---CABLE---GROUND--|-- GROUND(ESP32)
        |                                                         |
        |---GPIO3---CABLE---220OHM---GREEN PIN---CABLE---GROUND---|
*/



#define RED_PIN 1
#define YELLOW_PIN 2
#define GREEN_PIN 3

int active_pin= GREEN_PIN;

void change_pin(int pin){
    gpio_set_level(active_pin, 0);
    active_pin=pin;
    gpio_set_level(active_pin, 1);
}

void app_main(void){
    gpio_set_direction(RED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(YELLOW_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GREEN_PIN, GPIO_MODE_OUTPUT);

    gpio_set_level(GREEN_PIN, 1);
    int value=0;
    while(1){
        ESP_LOGI(APP, "Changing active pin");
        if(value%2){
            change_pin(RED_PIN);
            ESP_LOGI(APP, "Active pin: RED PIN");
        }else if(value%3){
            change_pin(YELLOW_PIN);
            ESP_LOGI(APP, "Active pin: YELLOW PIN");
        }else{
            change_pin(GREEN_PIN);
            ESP_LOGI(APP, "Active pin: GREEN PIN");
        }
        value++;
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }

}
