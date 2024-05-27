#include "common.h"

#define SAMPLE 1024

static esp_adc_cal_characteristics_t adc1;
uint8_t voltages[SAMPLE];
float average=0.0;

void elaboration(){
    //TO DECIDE
}

void air_detection(){
    for(int i=0; i<SAMPLE; i++){
        voltages[i]=esp_adc_cal_raw_to_voltage(adc1_get_raw(AIR_CHANNEL), &adc1);
        avg=voltages[i];
    }
    avg=/SAMPLE;
}

void airdetection_init(){
    esp_adc_cal_characterize(ADC, ATTENUATION, WIDTH, 0, &adc1);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(AIR_CHANNEL, ATTENUATION));
}