
#ifndef __MESH_MAIN_H__
#define __MESH_MAIN_H__

#include <string.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "mesh.h"
#include "http.h"
#include "mqtt.h"

#define  MESH_CMD_MQTT    (0x1)
#define  MESH_CMD_HTTP    (0x2)

#endif
