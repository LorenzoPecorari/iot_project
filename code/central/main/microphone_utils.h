#include "common.h"

#define MICROPHONE "[MICROPHONE]"
#define ATTENUATION ADC_ATTEN_DB_12
#define WIDTH ADC_WIDTH_BIT_DEFUALT

static esp_adc_cal_characteristics_t adc1;
uint8_t voltage;

void check_people(){
    bool people=0;
    int check=0;
    while(!people){
        while(!people && check<6){
            voltage=esp_adc_cal_raw_to_voltage(adc1_get_raw(MIC_CHANNEL), &adc1);
            if(voltage>PEOPLE_PRESENCE){
                ESP_LOGI(MICROPHONE, "People detected")
                people=1;
            }else{
                ESP_LOGI(MICROPHONE, "People don't detected");
                check++;
                vTaskDelay(10000/portTICK_PERIOD_MS);
            }
        }
        check=0;
        vTaskDelay(540000/portTICK_PERIOD_MS); //LIGHT SLEEP MODE?
    }
}

void microphone_init(){
    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &adc1);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(MIC_CHANNEL, ATTENUATION))
}