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

#define MIC_PIN 
#define AIR_PIN

// esp-now communication channel
#define CHANNEL 0

extern uint32_t air_voltage;