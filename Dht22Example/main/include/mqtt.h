#ifndef MQTT
#define MQTT

#include "esp_log.h"
#include "mqtt_client.h"

#define MQTT_MESSAGE_TOPIC_SIZE (20)
#define MQTT_MESSAGE_PAYLOAD_SIZE (100)

typedef struct {
    char topic[MQTT_MESSAGE_TOPIC_SIZE];
    char payload[MQTT_MESSAGE_PAYLOAD_SIZE];
} mqtt_msg_t;

void mqtt_publish_msg(mqtt_msg_t *msg);
void mqtt_start();

#endif
