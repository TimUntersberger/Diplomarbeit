/*

        DHT22 temperature sensor driver

*/

#ifndef DHT22_H_
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

#include <stdbool.h>

// == function prototypes =======================================

void dht22_set_gpio(int gpio);
void dht22_handle_error(int response);
int dht22_read();
float dht22_get_humidity();
float dht22_get_temperature();
int dht22_get_signal_level(int usTimeOut, bool state);

#endif
