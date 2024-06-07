#include "common.h"

#define MIC "[MICROPHONE]"

static esp_adc_cal_characteristics_t adc1;
uint8_t voltage=0;

int check_people(){
    bool people=0;
    int check=0;
    while(!people && check<15){
        voltage=gpio_get_level(MIC_CHANNEL);
        if(voltage){
            ESP_LOGI(MIC, "People detected");
            people=1;
        }else{
            ESP_LOGI(MIC, "People don't detected");
            check++;
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
    }
    return people;
}

void microphone_init(){
    esp_adc_cal_characterize(ADC, ATTENUATION, WIDTH, 0, &adc1);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(MIC_CHANNEL, ATTENUATION));
}