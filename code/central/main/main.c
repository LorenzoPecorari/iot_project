#include "esp-now_utils.h"
#include "wifi_utils.h"
#include "microphone_utils.h"

#define APP_NAME "[CENTRAL]"
//CENTRAL DEVICE MAIN FILE

void app_main(void){
    //ELEMENTS INITIALIZATION
    wifi_init();
    espnow_init();
    microphone_init();

    //MAC SHARING TO HELPER DEVICES
    esp_now_mac_tx();

    //WAITING FOR HELPER DEVICE MAC
    esp_now_mac_rx();

    while(1){
        //CHECK PERSONZ (CIT) INSIDE THE CLASSROOM
        check_people();

        
    }
}
