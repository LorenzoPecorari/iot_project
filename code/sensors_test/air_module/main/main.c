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

void app_main(void)
{
   uint32_t voltage;

    // ATTENUATION = 11 dB, REPRESENTATIONAL BITS = default value, ADC UNIT = 1 --> use pin with ADC1
   esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);

   ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
   ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));

   while (1)
   {
       voltage = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
       printf("ADC1_CHANNEL_0: %" PRIu32 " mV\n", voltage);
       vTaskDelay(pdMS_TO_TICKS(60000/portTICK_PERIOD_MS));
   }
}