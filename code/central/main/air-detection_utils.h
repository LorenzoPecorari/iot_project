#include "common.h"

#define AIR "[AIR-DETECTION]"
#define SAMPLE 20
#define DELAY 500/portTICK_PERIOD_MS

#define V_REF 1100
#define R_FIXED 100000              // outer resistance, 100k Ohm
#define B_COEFFICIENT 3950          // Beta coefficient thermistor
#define NOMINAL_RESISTANCE 10000    // Nominal resistance of thermistor, 10k Ohm
#define NOMINAL_TEMPERATURE 25

static esp_adc_cal_characteristics_t adc1_airq;
static esp_adc_cal_characteristics_t adc1_temp;
uint32_t air_quality[SAMPLE];
uint32_t temperatures[SAMPLE];
float average_air=0.0;
float general_avg_air=0.0;

float average_temp=0.0;
float general_avg_temp=0.0;
float resistance;
float temperature;
uint32_t voltage_air;

void elaboration(float helper_average){
    //COMPUTING GENERAL AIR QUALITY VALUES AVERAGE
    general_avg_air=average_air+helper_average;
    general_avg_air/=2;
    ESP_LOGI(AIR, "General average of data sampled on this window: %f", general_avg_air);

    //COMPUTING GENERAL TEMPERATURE VALUES AVERAGE

    average_air=0.0;
    general_avg_air=0.0;
    average_temp=0.0;
    general_avg_temp=0.0;
}

void temp_detection(){
    for(int i=0; i<SAMPLE; i++){
        voltage_air=esp_adc_cal_raw_to_voltage(adc1_get_raw(TEMP_CHANNEL), &adc1_temp);
        resistance = (float)R_FIXED * ((3300.0 / voltage) - 1);

        // temperature evaluated in Celsius
        temperature = 1.0 / (1.0 / (NOMINAL_TEMPERATURE + 273.15) + log(resistance / NOMINAL_RESISTANCE) / B_COEFFICIENT) - 273.15;
        temperatures[i]=temperature;
        average_temp+=temperature;

        vTaskDelay(DELAY);
    }
    average_temp/=SAMPLE;
    ESP_LOGI(AIR, "Temperature values detected");
}

void air_quality_detection(){
    for(int i=0; i<SAMPLE; i++){
        air_quality[i]=esp_adc_cal_raw_to_voltage(adc1_get_raw(AIR_CHANNEL), &adc1_airq);
        average_air=air_quality[i];
        vTaskDelay(DELAY);
    }
    average_air/=SAMPLE;
    ESP_LOGI(AIR, "Air quality values detected");
}

void air_detection(){
    air_quality_detection();
    temp_detection();
}

void airdetection_init(){
    esp_adc_cal_characterize(ADC, ATTENUATION, WIDTH, 0, &adc1_airq);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(AIR_CHANNEL, ATTENUATION));
    ESP_LOGI(AIR, "Air quality detection parameters initialized");

    esp_adc_cal_characterize(ADC, ATTENUATION, WIDTH, V_REF, &adc1_temp);

    ESP_ERROR_CHECK(adc1_config_width(WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(TEMP_CHANNEL, ATTENUATION));
    ESP_LOGI(AIR, "Temperature detection parameters initialized");
}