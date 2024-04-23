#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/*
           |- 5V --- CABLE --- POSITIVE (RED CABLE) --- *FAN* --- NEGATIVE (BLACK CABLE) ----- COLLECTOR -|
           |                                                                                              |
*ESP32S3* -|- GPIO2 --- CABLE --- RESISTENCE 10 KOHM --- CABLE ------------------------------------ BASE -|- *TRANSISTOR* 
           |                                                                                              |
           |- GND --- CABLE -------------------------------------------------------------------- EMITTER -|
           |   |-----------------------------------------------------|
           |                                                         |
           |- GPIO4 --- CABLE --- POSITIVE --- *LED* --- NEGATIVE ---|
*/

#define PIN_TRANSISTOR GPIO_NUM_2 // Imposta il pin collegato alla base del transistor
#define LED_PIN GPIO_NUM_4 // Imposta il pin di controllo dell'ESP32-S3

void init() {
    // Configura i pin
    gpio_set_direction(PIN_TRANSISTOR, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

void controlloVentola(void *pvParameter) {
    while (1) {
        // Accendi la ventola
        printf("Spenta\n");
        gpio_set_level(PIN_TRANSISTOR, 1);
        gpio_set_level(LED_PIN, 0); // Accendi il pin di controllo
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Attendi per 5 secondi
        
        // Spegni la ventola
        printf("Accesa\n");
        gpio_set_level(PIN_TRANSISTOR, 0);
        gpio_set_level(LED_PIN, 1); // Spegni il pin di controllo
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Attendi per 5 secondi
    }
}

void app_main() {
    // Inizializza i pin
    init();

    // Crea il task per il controllo della ventola
    xTaskCreate(controlloVentola, "Controllo Ventola", 4096, NULL, 5, NULL);
}
