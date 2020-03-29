#include "main.h"

static const char* TAG = "example";
static bool is_connected_to_router = false;

mesh_cmd_t mqtt_cmd = {
    .type = MESH_CMD_MQTT
};

void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;

    ESP_LOGI(TAG, "SUCCESSFULLY CONNECTED TO ROUTER");
    ESP_LOGI(TAG, "MY IP:" IPSTR, IP2STR(&event->ip_info.ip));
    is_connected_to_router = true;
    mqtt_start();
}

void on_cmd(mesh_cmd_t* cmd){
    if(esp_mesh_is_root()){
        if(cmd->type == MESH_CMD_MQTT && is_connected_to_router){
            mqtt_msg_t *mqtt_msg = (mqtt_msg_t*) cmd->payload;
            mqtt_publish_msg(mqtt_msg);
        }
        else if(cmd->type == MESH_CMD_ADD_NODE && is_connected_to_router){
            mesh_node_info_t *node_info = (mesh_node_info_t*) cmd->payload;
            mqtt_msg_t mqtt_msg = {0};

            snprintf(mqtt_msg.topic, 10, "/node/add");
            snprintf(
                mqtt_msg.payload, 
                100, 
                "{ \"parent\": \""MACSTR"\", \"mac\": \""MACSTR"\" }", 
                MAC2STR(node_info->parent),
                MAC2STR(node_info->mac)
            );

            mqtt_publish_msg(&mqtt_msg);
        }
        else if(cmd->type == MESH_CMD_REMOVE_NODE && is_connected_to_router){
            mesh_node_info_t *node_info = (mesh_node_info_t*) cmd->payload;
            mqtt_msg_t mqtt_msg = {0};

            snprintf(mqtt_msg.topic, MQTT_MESSAGE_TOPIC_SIZE, "/node/remove");
            snprintf(
                mqtt_msg.payload, 
                MQTT_MESSAGE_PAYLOAD_SIZE, 
                "{ \"mac\": \""MACSTR"\" }", 
                MAC2STR(node_info->mac)
            );

            mqtt_publish_msg(&mqtt_msg);
        }
    }
}

void wifi_init(){
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void DHT_task(void *pvParameter)
{
	setDHTgpio( 27 );
	while(1) {
	
		printf("=== Reading DHT ===\n" );
		int ret = readDHT();
		
		errorHandler(ret);

		printf( "Hum %.1f\n", getHumidity() );

		printf( "Tmp %.1f\n", getTemperature() );
		
		mqtt_msg_t msg ={0};
		snprintf(msg.topic, 20, "temperature");
   		snprintf(msg.payload, 100, "%f", getTemperature());
		mqtt_publish_msg(&msg);
		snprintf(msg.topic, 20, "humidity");
   		snprintf(msg.payload, 100, "%f", getHumidity());
        	mqtt_publish_msg(&msg);
		// currentHum = getHumidity();
		// currentTemp = getTemperature();
		// -- wait at least 2 sec before reading again ------------
		// The interval of whole process must be beyond 2 seconds !! 
		vTaskDelay( 3000 / portTICK_RATE_MS );
	}
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    mesh_create_network_interface();

    wifi_init();
    mesh_init();

    mesh_on_cmd(&on_cmd);

    mesh_start();

    xTaskCreate( &DHT_task, "DHT_task", 2048, NULL, 5, NULL );
}
