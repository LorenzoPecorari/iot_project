#include "common.h"

#include "mqtt_client.h"

#define APP_NAME "[MQTT]"
#define MQTT_USERNAME //TO INSERT
#define MQTT_PWD //TO INSERT
#define URI //TO DECIDE
//INSERT CERTIFICATE

esp_mqtt_client_config_t mqtt_cfg{
    .broker.address.uri=URI,
    .broker.verifiction.certificate=/*CERTIFICATE*/,
    .credentials.username=MQTT_USERNAME,
    .credentials.authentication.password=MQTT_PWD,
};
esp_mqtt_client_handle_t client;
esp_mqtt_event_handle_t event_handler;
esp_mqtt_event_id_t event_id;

void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
    event_handler=(esp_mqtt_client_handle_t)event_data;
    client=event_handler->client;
    event_id=(esp_mqtt_event_id_t)event_id;
    ESP_LOGI(APP_NAME, "Event id received: ", event_id);
    switch(event_id){
        case MQTT_EVENT_CONNECTED:
            break;
        case MQTT_EVENT_DISCONNECT:
            break;
        case MQTT_EVENT_SUBSCRIBED:
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_DATA:
            break;
        case MQTT_EVENT_ERROR:
            break;
        case MQTT_EVENT_PUBLISHED:
            break;
        default:
            break;
    }
}

void mqtt_transmission(){

}

void mqtt_init(){
    client=esp_mqtt_client_init(&mqtt_cfg);
    if(client=NULL){
        ESP_LOGE(APP_NAME, "Can't init client");
    }
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}