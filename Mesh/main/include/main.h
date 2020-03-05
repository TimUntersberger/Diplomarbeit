
#ifndef __MESH_MAIN_H__
#define __MESH_MAIN_H__

#include "esp_err.h"

#define  MESH_CMD_MQTT    (0x3)

typedef uint8_t mesh_cmd_type_t;

typedef struct {
    mesh_cmd_type_t type;
} mesh_cmd_t;

#endif
