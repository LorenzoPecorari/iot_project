#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define PIN_TRANSISTOR GPIO_NUM_2 // Imposta il pin collegato alla base del transistor
#define VENTOLA_PIN GPIO_NUM_4 // Imposta il pin di controllo dell'ESP32-S3

void init() {
    // Configura i pin
    gpio_set_direction(PIN_TRANSISTOR, GPIO_MODE_OUTPUT);
    gpio_set_direction(VENTOLA_PIN, GPIO_MODE_OUTPUT);
}

void controlloVentola(void *pvParameter) {
    while (1) {
        // Accendi la ventola
        gpio_set_level(PIN_TRANSISTOR, 1);
        gpio_set_level(VENTOLA_PIN, 1); // Accendi il pin di controllo
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Attendi per 5 secondi
        
        // Spegni la ventola
        gpio_set_level(PIN_TRANSISTOR, 0);
        gpio_set_level(VENTOLA_PIN, 0); // Spegni il pin di controllo
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Attendi per 5 secondi
    }
}

void app_main() {
    // Inizializza i pin
    init();

    // Crea il task per il controllo della ventola
    xTaskCreate(controlloVentola, "Controllo Ventola", 4096, NULL, 5, NULL);
}
