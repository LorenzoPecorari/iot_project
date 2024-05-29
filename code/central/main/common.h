#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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
#define WIDTH ADC_WIDTH_BIT_DEFUALT8
#define ADC ADC_UNIT_1

#define MIC_CHANNEL //TO DECIDE
#define AIR_CHANNEL //TO DECIDE

#define PEOPLE_PRESENCE //TO DECIDE

#define AIR_PIN //TO DECODE
#define MIC_PIN //TO DECIDE
#define RED_LED_PIN //TO DECIDE
#define YELLOW_LED_PIN //TO DECIDE
#define GREEN_LED_PIN //TO DECIDE

//CHANNEL FOR ESP-NOW COMMUNICATION
#define ESP_NOW_CHANNEL 0