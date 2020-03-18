#ifndef MESH
#define MESH

#include <string.h>
#include <stdbool.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"

#define RECEIVE_BUFFER_SIZE          (1500)
#define SENDER_BUFFER_SIZE          (1460)
#define MESH_CMD_QUEUE_SIZE          (10)
#define MESH_CMD_PAYLOAD_SIZE          (1000)

typedef uint8_t mesh_cmd_type_t;

typedef struct {
    mesh_cmd_type_t type;
    bool is_broadcasted;
    uint8_t payload[MESH_CMD_PAYLOAD_SIZE];
    uint8_t from[6];
} mesh_cmd_t;

typedef void (*mesh_cmd_cb)(mesh_cmd_t*);

const char* mesh_cmd_to_string(mesh_cmd_t* cmd);
void mesh_create_network_interface();
void mesh_queue_cmd(mesh_cmd_t* cmd);
void mesh_on_cmd(mesh_cmd_cb);
void mesh_init();
void mesh_start();

#endif
