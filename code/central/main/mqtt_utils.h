#include "common.h"

#include "mqtt_client.h"
#include "certificate.h"

#define MQTT "[MQTT]"
#define MQTT_USERNAME "MoodAdmin"
#define MQTT_PWD "MoodAdminPassword00"
#define URI "mqtts://f9461e1c79cc4304895c08585e83c17f.s1.eu.hivemq.cloud:8883"
#define TOPIC "/topic/mood/air"

#define MAX_MSG_RETRASMISSION 5

esp_mqtt_client_config_t mqtt_cfg={
    .broker.address.uri=URI,
    .broker.verification.certificate=(const char*)certificate,
    .credentials.username=MQTT_USERNAME,
    .credentials.authentication.password=MQTT_PWD,
};
esp_mqtt_client_handle_t client;
esp_mqtt_event_handle_t event_handler;
esp_mqtt_event_id_t event_id;

char data_tx[128];
// int retrasmission=0;

void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
    event_handler=(esp_mqtt_event_handle_t)event_data;
    client=event_handler->client;
    event_id=(esp_mqtt_event_id_t)event_id;
    ESP_LOGI(MQTT, "Event id received: %ld", event_id);
    switch(event_id){
        case MQTT_EVENT_CONNECTED:
            break;
        case MQTT_EVENT_DISCONNECTED:
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
            ESP_LOGI(MQTT, "Data correctly trasmitted");
            esp_wifi_disconnect();
            esp_wifi_deinit();
            esp_wifi_stop();
            break;
        default:
            break;
    }
}

void mqtt_transmission(float air_avg, float temp_avg){
    sprintf(data_tx, "Air data sampled: %.2f\nTemperature detected: %.2f", air_avg, temp_avg);
    ESP_LOGI(MQTT, "Trasmitting data...");
    esp_mqtt_client_publish(client, TOPIC, data_tx, strlen(data_tx), 1, 1);
}

void mqtt_init(){
    client=esp_mqtt_client_init(&mqtt_cfg);
    if(client==NULL){
        ESP_LOGE(MQTT, "Can't init client");
    }
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}