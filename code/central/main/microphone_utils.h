#include "common.h"

#define MIC "[MICROPHONE]"

static esp_adc_cal_characteristics_t adc1;
uint8_t voltage;

int check_people(){
    bool people=0;
    int check=0;
    while(!people && check<6){
        voltage=esp_adc_cal_raw_to_voltage(adc1_get_raw(MIC_CHANNEL), &adc1);
        if(voltage>PEOPLE_PRESENCE){
            ESP_LOGI(MIC, "People detected");
            people=1;
        }else{
            ESP_LOGI(MIC, "People don't detected");
            check++;
            vTaskDelay(10000/portTICK_PERIOD_MS);
        }
    }
    return people=1;
}

void microphone_init(){
    esp_adc_cal_characterize(ADC, ATTENUATION, WIDTH, 0, &adc1);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(MIC_CHANNEL, ATTENUATION));
}