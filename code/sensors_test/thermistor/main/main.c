#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <inttypes.h>
#include <math.h>

#include "esp_log.h"

#define APP "[THERMISTOR TEST] "

/*
    NOTES:
    
    - Thermistor is a sensor that changes its resistance with temperature changes
    - its resistance can be found using a tension divisor
    - it can be used the beta function fo evaluate the temperature sensed byt sensor starting from the voltage sampled
    - beta coefficient is found experimentally, usually is around 3900 and 4000; tells about sensivity of thermistor
    - at 25 °C (nominal temperature), resistance of thermistor is approximately 10k Ohm

    - resistance of thermistor wrt sampled voltage: R_thermistor = R_ext * (Vcc / V_sampled - 1)    <-- tension divisor!!! 

    - Beta function for finding temperature:
        1 / T = 1 / t_s + (ln(R_thermistor / external resistance) / beta_coefficient) =>
            => T = 1 / (1 / t_s + (ln(R_thermistor / external resistance) / beta_coefficient))

 */

#define CHANNEL ADC1_CHANNEL_2

#define V_REF 1100
#define R_FIXED 100000              // outer resistance, 100k Ohm
#define B_COEFFICIENT 3950          // Beta coefficient thermistor
#define NOMINAL_RESISTANCE 10000    // Nominal resistance of thermistor, 10k Ohm
#define NOMINAL_TEMPERATURE 25

static esp_adc_cal_characteristics_t adc1_chars;

void app_main(void)
{
    float resistance;
    float temperature;
    uint32_t voltage;

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, V_REF, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(CHANNEL, ADC_ATTEN_DB_11));

    while (1)
    {
        voltage = adc1_get_raw(CHANNEL);
        voltage = esp_adc_cal_raw_to_voltage(voltage, &adc1_chars);

        // resistance of thermistor
        resistance = (float)R_FIXED * ((3300.0 / voltage) - 1);
        printf("R_thermistor: %.2f ohm\n", resistance);

        // temperature evaluated in Celsius
        temperature = 1.0 / (1.0 / (NOMINAL_TEMPERATURE + 273.15) + log(resistance / NOMINAL_RESISTANCE) / B_COEFFICIENT) - 273.15;

        printf("T: %.2f°C\n", temperature);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
