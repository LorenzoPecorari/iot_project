#include "esp-now_utils_helper.h"
#include "wifi_utils.h"

#define APP_NAME "[HELPER] "

void app_main(void){
    wifi_init();
    init_esp_now();

    esp_now_mac_tx();

    send_data(8.16, 32.64);
    //esp_now_mac_rx();   

    while(1){
        // light sleep until central does not send wakeup message
        
        // sample data
        
        // send data
        // send_data(8.16, 32.64);
    }

}
