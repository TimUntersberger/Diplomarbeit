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

void set_dht22_gpio(int gpio);
void errorHandler(int response);
int readDHT();
float dht22_get_humidity();
float dht22_get_temperature();
int getSignalLevel(int usTimeOut, bool state);

#endif
