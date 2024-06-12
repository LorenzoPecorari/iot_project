#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// libraries for random number generation, REMOVE when merging all the codes!!
#include "esp_system.h" 
#include "esp_random.h"

/*
            |- 5V --- CABLE --- POSITIVE (RED CABLE) --- *FAN* --- NEGATIVE (BLACK CABLE) ----- COLLECTOR -|
            |                                                                                              |
*ESP32S3*  -|- GPIO19--- CABLE --- RESISTENCE 10 KOHM --- CABLE ------------------------------------ BASE -|- *TRANSISTOR* 
            |                                                                                              |
            |- GND --- CABLE -------------------------------------------------------------------- EMITTER -|
            |   |-------------------------------------------------------|
            |                                                           |
            |- GPIO20--- CABLE --- POSITIVE --- *R LED* --- NEGATIVE ---|
            |- GPIO21--- CABLE --- POSITIVE --- *B LED* --- NEGATIVE ---|

*/

#define PIN_TRANSISTOR GPIO_NUM_1 // Imposta il pin collegato alla base del transistor
// #define LED_PIN GPIO_NUM_4 // Imposta il pin di controllo dell'ESP32-S3
#define HOT_LED_PIN GPIO_NUM_20
#define COLD_LED_PIN GPIO_NUM_21

int temp_flag = 0; 
/* flag for defining if use the fan for decreasing on incresing temperature
    temp_flag = 1 --> room hot, decrease temperature, cold led on
    temp_flag = 0 --> room good, both leds off
    temp = -1 --> room cold, increase temperature, hot led on
*/

void fan_init() {
    // Configura i pin
    gpio_set_direction(PIN_TRANSISTOR, GPIO_MODE_OUTPUT);
    // gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(HOT_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(COLD_LED_PIN, GPIO_MODE_OUTPUT);
}


// it turns on the fan
void switch_fan_on(){
    gpio_set_level(PIN_TRANSISTOR, 1);
    printf("Fan: on\n");
}

// it turns off the fan
void switch_fan_off(){
    gpio_set_level(PIN_TRANSISTOR, 0);
    printf("Fan: off\n");
}

// it turns off all the leds
void switch_leds_off(){
    gpio_set_level(HOT_LED_PIN, 0);
    gpio_set_level(COLD_LED_PIN, 0);
}

// it turns on led for cooling, turns off led for heating, turns on fan
void cool_room(){
    ESP_LOGI(APP_NAME_AIR, "High temerature, fan needed for COOLING the room\n");
    gpio_set_level(HOT_LED_PIN, 0);
    gpio_set_level(COLD_LED_PIN, 1);
    switch_fan_on();
}

// it turns on led for heating, turns off led for cooling, turns on fan
void heat_room(){
    ESP_LOGI(APP_NAME_AIR, "Low temperature, fan needed for HEATING the room\n");
    gpio_set_level(COLD_LED_PIN, 0);
    gpio_set_level(HOT_LED_PIN, 1);
    switch_fan_on();
}

void ok_room(){
    gpio_set_level(COLD_LED_PIN, 0);
    gpio_set_level(HOT_LED_PIN, 0);
    switch_fan_off();
}

void controlloVentola(void *pvParameter) {
    while (1) {

        temp_flag = (esp_random() % 3) - 1;

        // based on temp_flag, it will decide to use the fan or not
        if(!temp_flag){
            printf("Good temperature, fan not needed\n");
            switch_fan_off();
            switch_leds_off();
        }
        else{
            if(temp_flag == -1)
                cool_room();
            else
                heat_room();
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS);

        /*
        // Accendi la ventola
        printf("Spenta\n");
        gpio_set_level(PIN_TRANSISTOR, 0);
        gpio_set_level(LED_PIN, 0); // Accendi il pin di controllo
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Attendi per 5 secondi
        
        // Spegni la ventola
        printf("Accesa\n");
        gpio_set_level(PIN_TRANSISTOR, 1);
        gpio_set_level(LED_PIN, 1); // Spegni il pin di controllo
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Attendi per 5 secondi
        */

    }
}

