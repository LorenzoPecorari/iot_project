#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <inttypes.h>

#include "esp_log.h"

#define APP_NAME_AIR_SENSOR "[AIR SENSOR] "

#define AIR_PIN 2           //  gpio pin for air sensor
#define ADC_CHANNEL ADC1_CHANNEL_0
#define ATTENUATION ADC_ATTEN_DB_11
#define WIDTH ADC_WIDTH_BIT_DEFAULT

#define NUM_SAMPLINGS 10    //  number of sampling to achieve during the execution
#define SAMPLING_TIME 6   //  period of sampling in seconds

uint32_t voltage;
uint32_t buf[NUM_SAMPLINGS];
static esp_adc_cal_characteristics_t adc1_chars;

void init_adc(){
    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11));
}

void get_values(){
    for(int i = 0; i < NUM_SAMPLINGS; i++){
        buf[i] = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC_CHANNEL), &adc1_chars);
        voltage += buf[i];
        vTaskDelay(pdMS_TO_TICKS((SAMPLING_TIME * 1000) / portTICK_PERIOD_MS));
    }

    voltage /= NUM_SAMPLINGS;
}