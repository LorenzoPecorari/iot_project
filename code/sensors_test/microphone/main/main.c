// #include <stdio.h>
// 




// HW CONFIG SCHEMA:
/*
                -| GND +------------+ GND |-
                 |                        |
    *ESP32S3*   -| GPIO_NUM_1 +-----+ OUT |- *MICROPHONE*
                 |                        |
                -| +5V +------------+ VCC |-
*/

// GPIO_NUM_1 -> ADC_UNIT_1, ADC1_CHANNEL_0

/*

+================= ATTENUATIONS =================+
|================================================|
| ADC_ATTEN_DB_0       -->     100 mV ~ 950 mV   |
| ADC_ATTEN_DB_2_5     -->     100 mV ~ 1250 mV  |
| ADC_ATTEN_DB_6       -->     150 mV ~ 1750 mV  |
| ADC_ATTEN_DB_11 (12) -->     150 mV ~ 2450 mV  |
+================================================+

+============================ WIDTHS =========================+
|=============================================================|
| ADC_WIDTH_9Bit    (0)         -->     9 bits                |
| ADC_WIDTH_10Bit   (1)         -->     10 bits               |
| ADC_WIDTH_11Bit   (2)         -->     11 bits               |
| ADC_WIDTH_12Bit   (3)         -->     12 bits               |
| ADC_WIDTH_BIT_DEFAULT         -->     max bits available    |
+=============================================================+

!!  use only 0, 1, 2 or 3 and not ADC_WIDTH_*bit otherwise it will not compile !!

*/

// attenutation and width of signal
// #define ATTENUATION ADC_ATTEN_DB_12
// #define WIDTH ADC_WIDTH_BIT_DEFAULT

// // sampled data parameters
// static esp_adc_cal_characteristics_t adc1_c;
// uint32_t voltage;

// // detectes the value of the output voltage from the microphone
// void room_noise_sampling(){
//     voltage = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_c);
//     printf("%"  PRIu32 "\n", voltage);
// }

// // initializes adc unit and its channel + checks for errors
// void init_microphone(){
//     ESP_ERROR_CHECK(adc1_config_width(WIDTH));
//     ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ATTENUATION));
//     esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 1100, &adc1_c);

// }

// void app_main(void){
//     init_microphone();

//     // 1 sample/min => 120 sample/hour
//     while(1){
//         room_noise_sampling();
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define MIC_PIN GPIO_NUM_4

void init_gpio() {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;       
    io_conf.pin_bit_mask = (1ULL << MIC_PIN);    
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void mic_task(void *pvParameter) {
    while(1) {
        int mic_value = gpio_get_level(MIC_PIN);
        if (mic_value == 1) {
            printf("Rumore rilevato!\n");
        } else {
            printf("Silenzio.\n");
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main() {

    init_gpio();

    xTaskCreate(&mic_task, "mic_task", 2048, NULL, 5, NULL);
}
