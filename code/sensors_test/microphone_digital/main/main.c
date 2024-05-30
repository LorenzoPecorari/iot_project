#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LED GPIO_NUM_1

void app_main() {

    gpio_set_direction(LED, GPIO_MODE_INPUT);

    while (1){
        printf("%d\n", gpio_get_level(LED));
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
