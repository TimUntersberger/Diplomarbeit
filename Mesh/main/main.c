#include "main.h"

static const char* TAG = "example";
static bool is_connected_to_router = false;

mesh_cmd_t mqtt_cmd = {
    .type = MESH_CMD_MQTT
};

typedef struct {
    char topic[10];
    char payload[100];
} mqtt_msg_t;

mesh_cmd_t http_cmd = {
    .type = MESH_CMD_HTTP
};

void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;

    ESP_LOGI(TAG, "SUCCESSFULLY CONNECTED TO ROUTER");
    ESP_LOGI(TAG, "MY IP:" IPSTR, IP2STR(&event->ip_info.ip));
    is_connected_to_router = true;
}

const char* type_to_name(uint8_t type){
    if(type == MESH_CMD_MQTT)
        return "MQTT";
    else if(type == MESH_CMD_HTTP)
        return "HTTP";
    return "UNKNOWN";
}

void on_cmd(mesh_cmd_t* cmd){
    ESP_LOGI(TAG, "Received a %s command from "MACSTR"", type_to_name(cmd->type), MAC2STR(cmd->from));

    if(esp_mesh_is_root()){
        if(cmd->type == MESH_CMD_MQTT && is_connected_to_router){
            mqtt_msg_t *mqtt_msg = (mqtt_msg_t*) cmd->payload;
            ESP_LOGI(TAG, "Payload: %s", mqtt_msg->payload);

            ESP_LOGI(TAG, "size of mqtt msg payload: %d", strlen(mqtt_msg->payload));
        }
        else if(cmd->type == MESH_CMD_HTTP && is_connected_to_router){
            http_request(HTTP_METHOD_GET, (const char*) cmd->payload);
            ESP_LOGI(TAG, "RESPONSE: %s", http_get_response_body());
        }
    }
    else {}
}

void wifi_init(){
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_ERROR);
//    esp_log_level_set("HTTP_CLIENT", ESP_LOG_INFO); 
 //   esp_log_level_set("mesh_main", ESP_LOG_INFO); 
    esp_log_level_set("example", ESP_LOG_INFO);
    ESP_ERROR_CHECK(nvs_flash_init());
    /*TODO: Research why netif is initialized before wifi, might be why we get the error */
    /*TODO: rewrite receive to always send to all nodes in iptable, since every node could have a subnode*/
    /*TODO: Try to create a subnetwork by moving some nodes out of range of the root node*/

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    mesh_create_network_interface();

    wifi_init();
    mesh_init();

    mesh_on_cmd(&on_cmd);
    mqtt_cmd.is_broadcasted = true;

    mqtt_msg_t mqtt_msg = {0};

    snprintf(mqtt_msg.topic, 10, "/");
    snprintf(mqtt_msg.payload, 100, "HELLO WORLD!");

    mesh_set_cmd_payload(&http_cmd, (void *)"http://ota.baaka.io/api/firmware/test/updatedAt");
    mesh_set_cmd_payload(&mqtt_cmd, (void *)&mqtt_msg);

    ESP_LOGI(TAG, "len of payload: %d", strlen(mqtt_msg.payload));

    mesh_start();

    mesh_queue_cmd(&mqtt_cmd);
}
