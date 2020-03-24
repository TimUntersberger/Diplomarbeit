# Mesh 

Open project configuration menu (`idf.py menuconfig`) to configure the mesh network.

Following things are required: 

* channel
* router SSID
* router password
* mqtt broker url
* partition table set to custom

When the mesh network is established and if you happen to run this example on ESP-WROVER-KIT boards, the RGB light indicator will show you on which layer devices are.

The pink reprents root; the yellow reprents layer 2; the red reprents layer 3; the blue reprents layer 4; the green reprents layer 5; the white reprents layer greater than 5.

Root continuously sends an On / Off control message to all devices in its routing table. Devices including root itself receive this message and do the On / Off.


## Notes

The `app_main` method initializes the nvs_flash, netif, event_loop, wifi and mesh.
It also starts the mesh network.

THIS PROGRAM DOES NOT USE THE INTERAL LED. I REPEAT. IT DOES NOT USE THE INTERNAL LED.

When the program first starts you can see that the mesh layer goes from 0 to either 2 or 1.

0 in this case is just the initial layer (meaning it has no real meaning as far as I understood it).

1 is a parent

2 is a child

### mesh_event_handler

This event handler is the heart of the program. It has a lot of different action events. All of them are explained below.

Looks like the handler gets passed a field called `event_data`. 

It seems like the id of a mesh node is just its mac address.

Maybe we can use this for sending events or such in the network?

We probably need to define a generic protocol for the data to reuse this framework (Maybe we can just use http?).

At least we should define a simple 'protocol' for sending data.

NOTE: According to esp-idf a failure handler is still missing

#### MESH_EVENT_STARTED

* fetches the mesh id
* sets the `is_mesh_connected` variable to false
* updates the current `mesh_layer`
* logs the mac address of the id

#### MESH_EVENT_STOPPED

* sets the `is_mesh_connected` variable to false.
* updates the current `mesh_layer`

#### MESH_EVENT_CHILD_CONNECTED

Gets passed some `mesh_event_child_connected_t` data.

* logs the id and mac address of the connected child

#### MESH_EVENT_CHILD_CONNECTED

Gets passed some `mesh_event_child_disconnected_t` data.

* logs the id and mac address of the connected child

#### MESH_EVENT_ROUTING_TABLE_ADD

Gets passed some `mesh_event_routing_table_change_t` data.

* logs by how much the `routing_table` size changed and how big it is now

#### MESH_EVENT_ROUTING_TABLE_REMOVE

Gets passed some `mesh_event_routing_table_change_t` data.

* logs by how much the `routing_table` size changed and how big it is now

#### MESH_EVENT_NO_PARENT_FOUND

Gets passed some `mesh_event_no_parent_found_t` data.

* logs how often it has already scanned for a new parent

#### MESH_EVENT_PARENT_CONNECTED

Gets passed some `mesh_event_connected_t` data.

* fetches the mesh id 
* sets its `mesh_layer` to the `self_layer` of the passed `mesh_event_connected_t` data
* sets the `mesh_parent_addr`
* logs the change in mesh layers (from -> to)
* logs the mac address and id of the parent
* logs whether the current node is the parent node (If the node is not the parent node then <layer2> is printed, but only if the node is at layer 2 else NOTHING gets printed)
* updates the connected_indicator
* sets the `is_mesh_connected` variable to true.
* if the current node is the root node, then it starts the dhcpc server provided by esp_netif
* if the current node is not the root node, then it starts the peer to peer communication provided by esp_mesh

#### MESH_EVENT_PARENT_DISCONNECTED

Gets passed some `mesh_event_disconnected_t` data.

* logs why the node got disconnected
* sets the `is_mesh_connected` variable to false.
* updates the connected_indicator
* updates the `mesh_layer`

#### MESH_EVENT_LAYER_CHANGE

Gets passed some `mesh_event_disconnected_t` data.

* updates the `mesh_layer`
* logs the change in mesh layers (from -> to)
* logs whether the current node is the parent node (If the node is not the parent node then <layer2> is printed, but only if the node is at layer 2 else NOTHING gets printed)
* sets the `last_layer`
* updates the connected_indicator
