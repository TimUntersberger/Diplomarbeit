/*

	DHT22 sensor reading test

	Jun 2007: Ricardo Timmermann, implemetation


*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "DHT22.h"

#include "mqtt.h"
#include "wifi.h"

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

void app_main()
{
	// esp_log_level_set("*", ESP_LOG_INFO);
	ESP_ERROR_CHECK(nvs_flash_init());

	wifi_init_sta();
	while(get_is_connected() == false){
		vTaskDelay( 100 / portTICK_RATE_MS );
		printf("%d\n", get_is_connected());
	};

	mqtt_start();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	xTaskCreate( &DHT_task, "DHT_task", 2048, NULL, 5, NULL );
}