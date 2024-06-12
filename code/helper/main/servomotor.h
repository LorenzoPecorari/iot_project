//FILE FOR SERVOMOTOR TEST
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define APP "[SERVOMOTOR] "

// SERVO PINOUT SCHEMA:
// brown -> GND
// red -> +5V
// orange -> PWM

// HW CONFIG SCHEMA:                               ___
/*                 _____________________#--- B ---|   |
                  |    _________________#--- R ---|   | <- SERVOMOTOR
                __|___|______       ____#--- O ---|___|
    ESP32 ->    | G  5V     |       |                       
                |______PIN__|       |
                        |___________|

*/

#define S_PIN GPIO_NUM_6 // pin to change when all things have to be merged

// positions of servomotor, values in microseconds 
#define MIN_POS 1000 // -90 deg position, maybe not used for project
#define MID_POS 1500 // base position, 0 deg, it means "closed window"
#define MAX_POS 2000 // +90 deg position, it means "open window"

// degrees of max opening
#define MAX_ROTATION 90

// usual period of pwm signal
#define PWM_PERIOD 20000

// position of servomotor
int position = 0;

// clockwise -> direction = 1 (close window), 
// counterclockwise -> direction = 0 (open window)
int direction = 0;

// time interval that pwm signal have to be up
int period_up = 0;

// TaskHandle_t task_handler = NULL;

// initial setting, output mode
void servo_initialization(){
    gpio_set_direction(S_PIN, GPIO_MODE_OUTPUT);
}

// period to be up is computed as the minimum time for -90 degrees position + the remaining time for reaching the desired
void open_window(){

    // increase the position at each iteration while it does not reach the max value
    // gpio up for period_up us
    // gpio down for std period - period_up us

    for(; position <= MAX_ROTATION; position += 10){
        period_up = MIN_POS + ((MAX_POS - MIN_POS) * position / MAX_ROTATION);
            
        gpio_set_level(S_PIN, 1);
        esp_rom_delay_us(period_up);

        gpio_set_level(S_PIN, 0);
        esp_rom_delay_us(PWM_PERIOD - period_up);

        vTaskDelay(pdMS_TO_TICKS(20));
    }

    ESP_LOGI(APP, "Window: opened!");
}

void close_window(){

    // decrease the position at each iteration while it does not reach the "base" position
    // same as before for signal up and down

    for(; position >= 0; position -= 10){
            period_up = MIN_POS + ((MAX_POS - MIN_POS) * position / MAX_ROTATION);
            
            gpio_set_level(S_PIN, 1);
            esp_rom_delay_us(period_up);

            gpio_set_level(S_PIN, 0);
            esp_rom_delay_us(PWM_PERIOD - period_up);
            vTaskDelay(pdMS_TO_TICKS(20));
    }

    ESP_LOGI(APP, "Window: closed!");
}
    