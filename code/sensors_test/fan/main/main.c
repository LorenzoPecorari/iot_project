#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"

// Definizione dei pin
#define PIN_TRANSISTOR GPIO_NUM_2
#define PIN_POTENZIOMETRO ADC_CHANNEL_6 // Pin ADC 6 corrispondente al GPIO 34 su ESP32-S3

void init() {
    // Configurazione del pin del transistor
    // gpio_config_t transistorConfig;
    // transistorConfig.pin_bit_mask = (1ULL << PIN_TRANSISTOR);
    // transistorConfig.mode = GPIO_MODE_OUTPUT;
    // gpio_config(&transistorConfig);

    // // Configurazione del pin del potenziometro come ingresso analogico
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_config_channel_atten(PIN_POTENZIOMETRO, ADC_ATTEN_DB_0);
    gpio_set_direction(PIN_TRANSISTOR, GPIO_MODE_OUTPUT);
}

void controlloTrasistor(void *pvParameter) {
    while (1) {
        // Accendi il transistor
        gpio_set_level(PIN_TRANSISTOR, 1);
        printf("Pin %d\n", gpio_get_level(PIN_TRANSISTOR));
        // Leggi il potenziale in uscita
        // uint32_t potenzialeUscita = adc1_get_raw(PIN_POTENZIOMETRO);
        vTaskDelay(5000/portTICK_PERIOD_MS);
        // // Stampa il potenziale in uscita sulla porta seriale
        // printf("Potenziale in uscita: %lu\n", potenzialeUscita);

        // Spegni il transistor
        gpio_set_level(PIN_TRANSISTOR, 0);
        printf("Pin %d\n", gpio_get_level(PIN_TRANSISTOR));
        // Attendi un po' prima di ripetere il loop
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}

void app_main() {
    // Inizializzazione delle periferiche
    init();

    // Creazione del task per il controllo del transistor
    xTaskCreate(controlloTrasistor, "Controllo Transistor", 4096, NULL, 5, NULL);
}
