#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_io.h"

#define APP_NAME "[LIGHT_SLEEP TEST]"

void app_main(void) {
    const float wakeup_time_sec = 2.5;

    for(int i = 0; i < 10; i++){
        vTaskDelay(2500 / portTICK_PERIOD_MS);
    }

    for(int i = 0; i < 10; i++){
        esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);
        esp_light_sleep_start();
    }

    ESP_LOGI(APP_NAME, "End of execution");
    vTaskDelete(NULL);
}
