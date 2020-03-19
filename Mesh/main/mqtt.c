#include "mqtt.h"

static const char *TAG = "mqtt_client";
static esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_PUBLISHED:
            break;
        case MQTT_EVENT_DATA:
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            break;
    }
    return ESP_OK;
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    mqtt_event_handler_cb(event_data);
}

void mqtt_publish_msg(mqtt_msg_t *msg){
    if(client == NULL){
        ESP_LOGW(TAG, "Aborting due to undefined mqtt_client");
        return;
    }
    esp_mqtt_client_publish(client, msg->topic, msg->payload, 0, 0, 0);
}

void mqtt_on_msg(mqtt_msg_cb cb){
    msg_cb = cb;
}

void mqtt_start(){
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_MQTT_BROKER_URL,
    };
    ESP_LOGI(TAG, "Mqtt client starting");
    client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_LOGI(TAG, "Mqtt client started %d", (int) client);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
