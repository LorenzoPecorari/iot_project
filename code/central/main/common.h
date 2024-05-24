#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define MIC_PIN //TO DECIDE
#define AIR_PIN //TO DECODE
#define RED_LED_PIN //TO DECIDE
#define YELLOW_LED_PIN //TO DECIDE
#define GREEN_LED_PIN //TO DECIDE

//CHANNEL FOR ESP-NOW COMMUNICATION
#define CHANNEL 0