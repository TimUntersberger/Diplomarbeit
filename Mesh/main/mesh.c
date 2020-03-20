#include "mesh.h"

static const char *MESH_TAG = "mesh_main";
static const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
static uint8_t sender_buffer[SENDER_BUFFER_SIZE] = { 0, };
static uint8_t receiver_buffer[RECEIVE_BUFFER_SIZE] = { 0, };
static bool is_running = true;
static bool is_mesh_connected = false;
static mesh_addr_t mesh_parent_addr;
static mesh_event_root_address_t root_addr;
static int mesh_layer = -1;
static esp_netif_t *netif_sta = NULL;
static mesh_cmd_t* cmd_queue[MESH_CMD_QUEUE_SIZE] = {0};
static mesh_cmd_cb cmd_cb;
static mesh_connected_cb connected_cb;
static uint8_t mac[6];

bool mac_addr_equal(uint8_t* mac1, uint8_t* mac2){
    return !memcmp(mac1, mac2, 6);
}

bool is_mac_initialized(uint8_t* mac){
    for(int i = 0; i < sizeof(mac); i++){
        if(mac[i] != 0){
            return true;
        }
    }

    return false;
}

void mesh_set_cmd_payload(mesh_cmd_t* cmd, uint8_t* payload){
    for(int i = 0; i < MESH_CMD_PAYLOAD_SIZE; i++){
        cmd->payload[i] = payload[i];
    }
}

esp_err_t mesh_send_cmd(mesh_cmd_t* cmd){
    esp_err_t err = ESP_OK;

    int route_table_size = 0;
    mesh_addr_t route_table[CONFIG_MESH_ROUTE_TABLE_SIZE];

    mesh_data_t data;
    data.data = sender_buffer;
    data.size = sizeof(sender_buffer);
    data.proto = MESH_PROTO_BIN;
    data.tos = MESH_TOS_P2P;

    ESP_LOGI(MESH_TAG, "My mac is "MACSTR"", MAC2STR(mac));

    if(!is_mac_initialized(cmd->from)){
        memcpy(cmd->from, mac, 6);
    }

    memcpy(sender_buffer, (uint8_t *)cmd, sizeof(mesh_cmd_t));

    esp_mesh_get_routing_table((mesh_addr_t *) &route_table,
                               CONFIG_MESH_ROUTE_TABLE_SIZE * 6, &route_table_size);

    if(esp_mesh_is_root()){
        for (int i = 0; i < route_table_size; i++) {
            if(mac_addr_equal(route_table[i].addr, cmd->from) || mac_addr_equal(route_table[i].addr, mac))
                continue;

            ESP_LOGI(MESH_TAG, "Sending to "MACSTR"", MAC2STR(route_table[i].addr));
            err = esp_mesh_send(&route_table[i], &data, MESH_DATA_P2P, NULL, 0);
            if (err) {
                ESP_LOGE(MESH_TAG,
                         "[ROOT-2-UNICAST][L:%d]parent:"MACSTR" to "MACSTR", heap:%d[err:0x%x, proto:%d, tos:%d]",
                         mesh_layer, MAC2STR(mesh_parent_addr.addr),
                         MAC2STR(route_table[i].addr), esp_get_free_heap_size(),
                         err, data.proto, data.tos);
            } 
        }
    } else {
        ESP_LOGI(MESH_TAG, "Sending to "MACSTR"", MAC2STR(mesh_parent_addr.addr));

        err = esp_mesh_send(&mesh_parent_addr, &data, MESH_DATA_P2P, NULL, 0);

        if (err) {
            ESP_LOGE(MESH_TAG,
                     "[ROOT-2-UNICAST][L:%d]parent:"MACSTR" to "MACSTR", heap:%d[err:0x%x, proto:%d, tos:%d]",
                     mesh_layer, MAC2STR(mesh_parent_addr.addr),
                     MAC2STR(mesh_parent_addr.addr), esp_get_free_heap_size(),
                     err, data.proto, data.tos);
        } 
    }

    return err;
}

/**
 * Pls don't forget to malloc the command or make sure to keep the variable in scope
 * */
void mesh_queue_cmd(mesh_cmd_t* cmd){
    for(int i = MESH_CMD_QUEUE_SIZE - 1; i >= 0; i--){
        if(cmd_queue[i] != NULL){
            cmd_queue[i + 1] = cmd;
            return;
        }
    }

    cmd_queue[0] = cmd;
}

void mesh_sender_task(void *arg)
{
    is_running = true;

    while (is_running) {
        ESP_LOGI(MESH_TAG, "layer:%d, rtableSize:%d, %s", mesh_layer,
                 esp_mesh_get_routing_table_size(),
                 (is_mesh_connected && esp_mesh_is_root()) ? "ROOT" : is_mesh_connected ? "NODE" : "DISCONNECT");
        vTaskDelay(10 * 1000 / portTICK_RATE_MS);

        for(int i = 0; i < MESH_CMD_QUEUE_SIZE; i++){
            mesh_cmd_t* cmd = cmd_queue[i];

            if(cmd == NULL)
                break;

            ESP_LOGI(MESH_TAG, "Sending command of type %d", cmd->type);

            mesh_send_cmd(cmd);

            cmd_queue[i] = NULL;
        }

        if (esp_mesh_get_routing_table_size() < 10) {
            vTaskDelay(1 * 1000 / portTICK_RATE_MS);
        }
    }

    vTaskDelete(NULL);
}

void mesh_on_cmd(mesh_cmd_cb cb){
    cmd_cb = cb;
}

void mesh_on_connected(mesh_connected_cb cb){
    connected_cb = cb;
}

void on_cmd_receive(mesh_cmd_t* cmd, mesh_addr_t from){
    if(cmd_cb != NULL)
        (*cmd_cb)(cmd);
}

esp_err_t receive_cmd(mesh_cmd_t** cmd, mesh_addr_t* from){
    esp_err_t err = ESP_OK;

    int flag = 0;

    mesh_data_t data;
    data.data = receiver_buffer;
    data.size = RECEIVE_BUFFER_SIZE;

    err = esp_mesh_recv(from, &data, portMAX_DELAY, &flag, NULL, 0);

    if (err != ESP_OK || !data.size) {
        ESP_LOGI(MESH_TAG, "err:0x%x, size:%d", err, data.size);
        return err;
    }

    *cmd = (mesh_cmd_t*) data.data;

    ESP_LOGI(MESH_TAG,
             "[#RX][L:%d] parent:"MACSTR", receive from "MACSTR", size:%d, heap:%d, flag:%d[err:0x%x, proto:%d, tos:%d]",
             mesh_layer,
             MAC2STR(mesh_parent_addr.addr), MAC2STR(from->addr),
             data.size, esp_get_free_heap_size(), flag, err, data.proto,
             data.tos);

    return err;
}

void on_node_lost(uint8_t* mac){
    mesh_cmd_t* cmd = (mesh_cmd_t*) calloc(1, sizeof(mesh_cmd_t));

    cmd->type = MESH_CMD_REMOVE_NODE;
    cmd->send_to_self = true;

    mesh_node_info_t info = {0};

    memcpy(info.mac, mac, 6);

    mesh_set_cmd_payload(cmd, (void *)&info);

    mesh_queue_cmd(cmd);
}

void on_node_connected(uint8_t* parent, uint8_t* mac){
    mesh_cmd_t* cmd = (mesh_cmd_t*) calloc(1, sizeof(mesh_cmd_t));

    cmd->type = MESH_CMD_ADD_NODE;
    cmd->send_to_self = true;

    mesh_node_info_t info = {0};

    memcpy(info.parent, parent, 6);
    memcpy(info.mac, mac, 6);

    mesh_set_cmd_payload(cmd, (void *)&info);

    mesh_queue_cmd(cmd);
}

void mesh_receiver_task(void *arg)
{
    mesh_cmd_t* cmd;
    mesh_addr_t from;
    is_running = true;

    while (is_running) {
        receive_cmd(&cmd, &from);

        if(esp_mesh_is_root() && cmd->is_broadcasted){
            mesh_send_cmd(cmd);
        }

        on_cmd_receive(cmd, from);
    }

    vTaskDelete(NULL);
}

esp_err_t mesh_start_tasks(void)
{
    static bool is_comm_p2p_started = false;
    if (!is_comm_p2p_started) {
        is_comm_p2p_started = true;
        xTaskCreate(mesh_sender_task, "MPTX", 3072, NULL, 5, NULL);
        xTaskCreate(mesh_receiver_task, "MPRX", 3072, NULL, 5, NULL);
    }
    return ESP_OK;
}

void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    mesh_addr_t id = {0,};
    static uint8_t last_layer = 0;

    switch (event_id) {
    case MESH_EVENT_STARTED: {
        esp_mesh_get_id(&id);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_MESH_STARTED>ID:"MACSTR"", MAC2STR(id.addr));
        is_mesh_connected = false;
        mesh_layer = esp_mesh_get_layer();
    }
    break;
    case MESH_EVENT_STOPPED: {
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOPPED>");
        is_mesh_connected = false;
        mesh_layer = esp_mesh_get_layer();
    }
    break;
    case MESH_EVENT_CHILD_CONNECTED: {
        mesh_event_child_connected_t *child_connected = (mesh_event_child_connected_t *)event_data;
        on_node_connected(mac, child_connected->mac);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, "MACSTR"",
                 child_connected->aid,
                 MAC2STR(child_connected->mac));
    }
    break;
    case MESH_EVENT_CHILD_DISCONNECTED: {
        mesh_event_child_disconnected_t *child_disconnected = (mesh_event_child_disconnected_t *)event_data;
        on_node_lost(child_disconnected->mac);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_DISCONNECTED>aid:%d, "MACSTR"",
                 child_disconnected->aid,
                 MAC2STR(child_disconnected->mac));
    }
    break;
    case MESH_EVENT_ROUTING_TABLE_ADD: {
        mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *)event_data;
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d",
                 routing_table->rt_size_change,
                 routing_table->rt_size_new);
    }
    break;
    case MESH_EVENT_ROUTING_TABLE_REMOVE: {
        mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *)event_data;
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_REMOVE>remove %d, new:%d",
                 routing_table->rt_size_change,
                 routing_table->rt_size_new);
    }
    break;
    case MESH_EVENT_NO_PARENT_FOUND: {
        mesh_event_no_parent_found_t *no_parent = (mesh_event_no_parent_found_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_NO_PARENT_FOUND>scan times:%d",
                 no_parent->scan_times);
    }
    /* TODO handler for the failure */
    break;
    case MESH_EVENT_PARENT_CONNECTED: {
        mesh_event_connected_t *connected = (mesh_event_connected_t *)event_data;
        esp_mesh_get_id(&id);
        mesh_layer = connected->self_layer;
        memcpy(&mesh_parent_addr.addr, connected->connected.bssid, 6);
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:"MACSTR"%s, ID:"MACSTR"",
                 last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
                 esp_mesh_is_root() ? "<ROOT>" :
                 (mesh_layer == 2) ? "<layer2>" : "", MAC2STR(id.addr));
        last_layer = mesh_layer;
        is_mesh_connected = true;
        if(connected_cb != NULL){
            (*connected_cb)(esp_mesh_is_root());
        }
        if (esp_mesh_is_root()) {
            esp_netif_dhcpc_start(netif_sta);
        }
        mesh_start_tasks();
    }
    break;
    case MESH_EVENT_PARENT_DISCONNECTED: {
        mesh_event_disconnected_t *disconnected = (mesh_event_disconnected_t *)event_data;
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d",
                 disconnected->reason);
        is_mesh_connected = false;
        mesh_layer = esp_mesh_get_layer();
    }
    break;
    case MESH_EVENT_LAYER_CHANGE: {
        mesh_event_layer_change_t *layer_change = (mesh_event_layer_change_t *)event_data;
        mesh_layer = layer_change->new_layer;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_LAYER_CHANGE>layer:%d-->%d%s",
                 last_layer, mesh_layer,
                 esp_mesh_is_root() ? "<ROOT>" :
                 (mesh_layer == 2) ? "<layer2>" : "");
        last_layer = mesh_layer;
    }
    break;
    case MESH_EVENT_ROOT_ADDRESS: {
        root_addr = *((mesh_event_root_address_t *)event_data);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:"MACSTR"",
                 MAC2STR(root_addr.addr));
    }
    break;
    case MESH_EVENT_VOTE_STARTED: {
        mesh_event_vote_started_t *vote_started = (mesh_event_vote_started_t *)event_data;
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_VOTE_STARTED>attempts:%d, reason:%d, rc_addr:"MACSTR"",
                 vote_started->attempts,
                 vote_started->reason,
                 MAC2STR(vote_started->rc_addr.addr));
    }
    break;
    case MESH_EVENT_VOTE_STOPPED: {
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_VOTE_STOPPED>");
        break;
    }
    case MESH_EVENT_ROOT_SWITCH_REQ: {
        mesh_event_root_switch_req_t *switch_req = (mesh_event_root_switch_req_t *)event_data;
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_SWITCH_REQ>reason:%d, rc_addr:"MACSTR"",
                 switch_req->reason,
                 MAC2STR( switch_req->rc_addr.addr));
    }
    break;
    case MESH_EVENT_ROOT_SWITCH_ACK: {
        /* new root */
        mesh_layer = esp_mesh_get_layer();
        esp_mesh_get_parent_bssid(&mesh_parent_addr);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_SWITCH_ACK>layer:%d, parent:"MACSTR"", mesh_layer, MAC2STR(mesh_parent_addr.addr));
    }
    break;
    case MESH_EVENT_TODS_STATE: {
        mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
    }
    break;
    case MESH_EVENT_ROOT_FIXED: {
        mesh_event_root_fixed_t *root_fixed = (mesh_event_root_fixed_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_FIXED>%s",
                 root_fixed->is_fixed ? "fixed" : "not fixed");
    }
    break;
    case MESH_EVENT_ROOT_ASKED_YIELD: {
        mesh_event_root_conflict_t *root_conflict = (mesh_event_root_conflict_t *)event_data;
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_ASKED_YIELD>"MACSTR", rssi:%d, capacity:%d",
                 MAC2STR(root_conflict->addr),
                 root_conflict->rssi,
                 root_conflict->capacity);
    }
    break;
    case MESH_EVENT_CHANNEL_SWITCH: {
        mesh_event_channel_switch_t *channel_switch = (mesh_event_channel_switch_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHANNEL_SWITCH>new channel:%d", channel_switch->channel);
    }
    break;
    case MESH_EVENT_SCAN_DONE: {
        mesh_event_scan_done_t *scan_done = (mesh_event_scan_done_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_SCAN_DONE>number:%d",
                 scan_done->number);
    }
    break;
    case MESH_EVENT_NETWORK_STATE: {
        mesh_event_network_state_t *network_state = (mesh_event_network_state_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_NETWORK_STATE>is_rootless:%d",
                 network_state->is_rootless);
    }
    break;
    case MESH_EVENT_STOP_RECONNECTION: {
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOP_RECONNECTION>");
    }
    break;
    case MESH_EVENT_FIND_NETWORK: {
        mesh_event_find_network_t *find_network = (mesh_event_find_network_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_FIND_NETWORK>new channel:%d, router BSSID:"MACSTR"",
                 find_network->channel, MAC2STR(find_network->router_bssid));
    }
    break;
    case MESH_EVENT_ROUTER_SWITCH: {
        mesh_event_router_switch_t *router_switch = (mesh_event_router_switch_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROUTER_SWITCH>new router:%s, channel:%d, "MACSTR"",
                 router_switch->ssid, router_switch->channel, MAC2STR(router_switch->bssid));
    }
    break;
    default:
        ESP_LOGI(MESH_TAG, "unknown id:%d", event_id);
        break;
    }
}
/**
 * Has to be after netif and flash initialization, but before initializing the wifi
 */
void mesh_create_network_interface(){
    /*  create network interfaces for mesh (only station instance saved for further manipulation, soft AP instance ignored */
    ESP_ERROR_CHECK(esp_netif_create_default_wifi_mesh_netifs(&netif_sta, NULL));
}

void mesh_init(){
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_MESH_MAX_LAYER));
    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(10));
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
    /* mesh ID */
    memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);
    /* router */
    cfg.channel = CONFIG_MESH_CHANNEL;
    cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
    memcpy((uint8_t *) &cfg.router.ssid, CONFIG_MESH_ROUTER_SSID, cfg.router.ssid_len);
    memcpy((uint8_t *) &cfg.router.password, CONFIG_MESH_ROUTER_PASSWD,
           strlen(CONFIG_MESH_ROUTER_PASSWD));
    /* mesh softAP */
    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(CONFIG_MESH_AP_AUTHMODE));
    cfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;
    memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
           strlen(CONFIG_MESH_AP_PASSWD));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
}

void mesh_start(){
    /* mesh start */
    ESP_ERROR_CHECK(esp_mesh_start());
    esp_efuse_mac_get_default(mac);
    ESP_LOGI(MESH_TAG, "mesh starts successfully, heap:%d, %s\n",  esp_get_free_heap_size(),
         esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed");
}
