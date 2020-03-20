#ifndef MESH
#define MESH

#include <string.h>
#include <stdbool.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"

#define RECEIVE_BUFFER_SIZE         (1500)
#define SENDER_BUFFER_SIZE          (1460)
#define MESH_CMD_QUEUE_SIZE         (10)
#define MESH_CMD_PAYLOAD_SIZE       (300)

#define MESH_CMD_ADD_NODE           (8)
#define MESH_CMD_REMOVE_NODE           (9)

typedef uint8_t mesh_cmd_type_t;

typedef struct {
    mesh_cmd_type_t type;
    bool is_broadcasted;
    bool send_to_self;
    uint8_t payload[MESH_CMD_PAYLOAD_SIZE];
    uint8_t from[6];
} mesh_cmd_t;

typedef struct {
    uint8_t parent[6];
    uint8_t mac[6];
} mesh_node_info_t;

typedef void (*mesh_cmd_cb)(mesh_cmd_t*);
typedef void (*mesh_connected_cb)(bool);

const char* mesh_cmd_to_string(mesh_cmd_t* cmd);
void mesh_create_network_interface();
void mesh_queue_cmd(mesh_cmd_t* cmd);
void mesh_on_cmd(mesh_cmd_cb);
void mesh_on_connected(mesh_connected_cb);
void mesh_set_cmd_payload(mesh_cmd_t* cmd, uint8_t *payload);
void mesh_init();
void mesh_start();

#endif
