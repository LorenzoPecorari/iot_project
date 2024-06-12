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


#define YELLOW_LED_PIN 4
#define GREEN_LED_PIN 5

#define RED_LED_PIN 6
#define BLUE_LED_PIN 7

void app_main(void){
    gpio_set_direction(RED_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLUE_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(YELLOW_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    while(1){
        for(int i=0; i<10; i++){
            if(i==0){
                printf("Nessun led acceso\n");
                gpio_set_level(GREEN_LED_PIN, 0);
                gpio_set_level(BLUE_LED_PIN, 0);
                gpio_set_level(RED_LED_PIN, 0);
                gpio_set_level(YELLOW_LED_PIN, 0);
            }else if(i==1){
                printf("Led verde acceso\n");
                gpio_set_level(GREEN_LED_PIN, 1);
                gpio_set_level(BLUE_LED_PIN, 0);
                gpio_set_level(RED_LED_PIN, 0);
                gpio_set_level(YELLOW_LED_PIN, 0);
            }else if(i==2){
                printf("Led giallo acceso\n");
                gpio_set_level(GREEN_LED_PIN, 0);
                gpio_set_level(BLUE_LED_PIN, 0);
                gpio_set_level(RED_LED_PIN, 0);
                gpio_set_level(YELLOW_LED_PIN, 1);
            }else if(i==3){
                printf("Led blu acceso\n");
                gpio_set_level(GREEN_LED_PIN, 0);
                gpio_set_level(BLUE_LED_PIN, 1);
                gpio_set_level(RED_LED_PIN, 0);
                gpio_set_level(YELLOW_LED_PIN, 0);
            }else if(i==4){
                printf("Led rosso acceso\n");
                gpio_set_level(GREEN_LED_PIN, 0);
                gpio_set_level(BLUE_LED_PIN, 0);
                gpio_set_level(RED_LED_PIN, 2);
                gpio_set_level(YELLOW_LED_PIN, 0);
            }else if(i==5){
                printf("Led verde e blu acceso\n");
                gpio_set_level(GREEN_LED_PIN, 1);
                gpio_set_level(BLUE_LED_PIN, 1);
                gpio_set_level(RED_LED_PIN, 0);
                gpio_set_level(YELLOW_LED_PIN, 0);
            }else if(i==6){
                printf("Led verde e rosso acceso\n");
                gpio_set_level(GREEN_LED_PIN, 1);
                gpio_set_level(BLUE_LED_PIN, 0);
                gpio_set_level(RED_LED_PIN, 1);
                gpio_set_level(YELLOW_LED_PIN, 0);
            }else if(i==7){
                printf("Led giallo e blu acceso\n");
                gpio_set_level(GREEN_LED_PIN, 0);
                gpio_set_level(BLUE_LED_PIN, 1);
                gpio_set_level(RED_LED_PIN, 0);
                gpio_set_level(YELLOW_LED_PIN, 1);
            }else if(i==8){
                printf("Led giallo e rosso acceso\n");
                gpio_set_level(GREEN_LED_PIN, 0);
                gpio_set_level(BLUE_LED_PIN, 0);
                gpio_set_level(RED_LED_PIN, 1);
                gpio_set_level(YELLOW_LED_PIN, 1);
            }else if(i==9){
                printf("Tutti i led acceso\n");
                gpio_set_level(GREEN_LED_PIN, 1);
                gpio_set_level(BLUE_LED_PIN, 1);
                gpio_set_level(RED_LED_PIN, 1);
                gpio_set_level(YELLOW_LED_PIN, 1);
            }
            vTaskDelay(2000/portTICK_PERIOD_MS);
        }
    }

}
