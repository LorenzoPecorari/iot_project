#include "esp-now_utils.h"
#include "wifi_utils.h"

#define APP_NAME "[CENTRAL]"
//CENTRAL DEVICE MAIN FILE

void app_main(void){
    //WIFI INITIALIZATION FOR ESP-NOW COMMUNICATION
    wifi_init();
    esp_now_init();

    //MAC SHARING TO HELPER DEVICES
    esp_now_mac_tx();

    //WAITING FOR HELPER DEVICE MAC
    esp_now_mac_rx();
}
