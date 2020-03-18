#ifndef MQTT
#define MQTT

#include "esp_log.h"
#include "mqtt_client.h"

typedef struct {
    char topic[10];
    char payload[100];
} mqtt_msg_t;

void mqtt_publish_msg(mqtt_msg_t *msg);
void mqtt_start();

#endif
