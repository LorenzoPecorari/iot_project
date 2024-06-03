#include "common.h"

#define AIR "[AIR]"
#define SAMPLE 20
#define DELAY_INPUT 500/portTICK_PERIOD_MS

static esp_adc_cal_characteristics_t adc1;
uint8_t voltages[SAMPLE];
int average=0;

void elaboration(){
    //TO DECIDE
}

void air_detection(){
    for(int i=0; i<SAMPLE; i++){
        voltages[i]=esp_adc_cal_raw_to_voltage(adc1_get_raw(AIR_CHANNEL), &adc1);
        average=voltages[i];
        vTaskDelay(pdMS_TO_TICKS(DELAY_INPUT));
    }
    ESP_LOGI(AIR, "Air values detected");
    average/=SAMPLE;
    ESP_LOGI(AIR, "Air values average %d", average);
}

void airdetection_init(){
    esp_adc_cal_characterize(ADC, ATTENUATION, WIDTH, 0, &adc1);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(AIR_CHANNEL, ATTENUATION));
}