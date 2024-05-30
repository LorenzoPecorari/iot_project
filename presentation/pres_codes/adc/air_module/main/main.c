#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <inttypes.h>

#include "esp_log.h"

#define APP "AIR_MODULE_TEST"
//FILE FOR AIR MODULE TEST

static esp_adc_cal_characteristics_t adc1_chars;

uint32_t voltage;

void init_adc(){

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 1100, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));
}

void get values(){
    for(int i = 0; i < 10; i++){
        voltage += esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    voltage /= 10;
    printf("%" PRIu32 " mV\n", voltage);
} 

void app_main(void)
{

    // ATTENUATION = 11 dB, REPRESENTATIONAL BITS = default value, ADC UNIT = 1 --> use pin with ADC1
   esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 1100, &adc1_chars);

   ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
   ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));

   while (1)
   {
       voltage = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
       printf("%" PRIu32 " mV\n", voltage);
       vTaskDelay(pdMS_TO_TICKS(10000/portTICK_PERIOD_MS));
   }
}