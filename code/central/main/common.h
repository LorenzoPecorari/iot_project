#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_adc_cal.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

#define ATTENUATION ADC_ATTEN_DB_12
#define WIDTH ADC_WIDTH_BIT_DEFAULT
#define ADC ADC_UNIT_1

#define MIC_CHANNEL ADC1_CHANNEL_0 
#define AIR_CHANNEL ADC1_CHANNEL_1 
#define TEMP_CHANNEL ADC1_CHANNEL_2
#define GREEN_LED_PIN GPIO4
#define YELLOW_LED_PIN GPIO5
#define RED_LED_PIN GPIO6
#define BLUE_LED_PIN GPIO7

//CHANNEL FOR ESP-NOW COMMUNICATION
#define ESP_NOW_CHANNEL 0