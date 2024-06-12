#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "esp_log.h"

#define APP_NAME_AIR "[AIR SENSOR] "

#define AIR_CHANNEL ADC1_CHANNEL_0
#define ATTENUATION ADC_ATTEN_DB_11
#define WIDTH ADC_WIDTH_BIT_DEFAULT

#define TEMP_CHANNEL ADC1_CHANNEL_2

#define V_REF 1100
#define R_FIXED 100000              // outer resistance, 100k Ohm
#define B_COEFFICIENT 3950          // Beta coefficient thermistor
#define NOMINAL_RESISTANCE 10000    // Nominal resistance of thermistor, 10k Ohm
#define NOMINAL_TEMPERATURE 25

#define NUM_SAMPLINGS 10    //  number of sampling to achieve during the execution
#define SAMPLING_TIME 6   //  period of sampling in seconds

uint32_t air_voltage;
uint32_t temp_voltage;

float resistance_therm;
float temperature;

uint32_t buf[NUM_SAMPLINGS];

static esp_adc_cal_characteristics_t air_adc1_chars;
static esp_adc_cal_characteristics_t temp_adc1_chars;

void init_adc(){
    // initialization for air module
    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &air_adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(AIR_CHANNEL, ATTENUATION));

    // initialization for thermistor
    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, V_REF, &temp_adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(TEMP_CHANNEL, ATTENUATION));
}

void get_values(){
    for(int i = 0; i < NUM_SAMPLINGS; i++){
        buf[i] = esp_adc_cal_raw_to_voltage(adc1_get_raw(AIR_CHANNEL), &air_adc1_chars);
        air_voltage += buf[i];
        vTaskDelay((SAMPLING_TIME * 1000) / portTICK_PERIOD_MS);
    }

    air_voltage /= NUM_SAMPLINGS;

    for(int i = 0; i < NUM_SAMPLINGS; i++){
        temp_voltage = esp_adc_cal_raw_to_voltage(adc1_get_raw(TEMP_CHANNEL), &temp_adc1_chars);

        resistance_therm = (float) R_FIXED * ((3300.0 / temp_voltage) - 1);
        temperature += 1.0 / (1.0 / (NOMINAL_TEMPERATURE + 273.15) + log(resistance_therm / NOMINAL_RESISTANCE) / B_COEFFICIENT) - 273.15;

        vTaskDelay((SAMPLING_TIME * 1000) / portTICK_PERIOD_MS);
    }

    temperature /= NUM_SAMPLINGS;

    ESP_LOGI(APP_NAME_AIR, "Air: %" PRIu32 "mV - Temp: %.2f°C", air_voltage, temperature);
}