#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_adc_cal.h"

#define APP "MICROPHONE_TEST"

// HW CONFIG SCHEMA:
/*
                -| GND +------------+ GND |-
                 |                        |
    *ESP32S3*   -| GPIO_NUM_1 +-----+ OUT |- *MICROPHONE*
                 |                        |
                -| +5V +------------+ VCC |-
*/

// GPIO_NUM_1 -> ADC_UNIT_1, ADC1_CHANNEL_0

// attenutation and width of signal
#define ATTENUATION ADC_ATTEN_DB_11
#define WIDTH ADC_WIDTH_BIT_DEFAULT

// sampled data parameters
static esp_adc_cal_characteristics_t adc1_c;
uint32_t voltage;

// detectes the value of the output voltage from the microphone
void room_noise_sampling(){
    voltage = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_c);
    printf("%"  PRIu32 "mV\n", voltage);
}

// initializes adc unit and its channel + checks for errors
void init_microphone(){
    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &adc1_c);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ATTENUATION));
}

void app_main(void){
    init_microphone();

    // 2 sample/min => 120 sample/hour
    while(1){
        room_noise_sampling();
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}
