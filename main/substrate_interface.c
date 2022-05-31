/* 
 * Author: Woodfish,
 * File: substrate_interface.h
 * Desc: A specialized library to interface with a Substrate node.
 * Further: Based on the Python substrate library by the Polkascan Group
 * Date: May 27 20:47
 * Licence: MIT
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "utility.h"
#include "network.h"
#include "substrate_interface.h"

// initialize Self struct
struct Substrate* init_client(
    const char* url, int websocket, int ss58_format, void* type_registry, const char* type_registry_preset, void* cache_region, void* runtime_config,
    bool use_remote_preset, struct Ws_option* ws_options, bool auto_discover, bool auto_reconnect
) {
    if ((strlen(url) == 0 && !websocket) || (strlen(url) > 0 && websocket)) {
        printf("%s", "Either 'url' or 'websocket' must be provided\n");
        exit(1);
    }

    // runtime config 
    Self.runtime_config = runtime_config;
    Self.cache_region = cache_region;

    Self.ss58_format = ss58_format ? ss58_format : 42; // default is 42

    Self.type_registry_preset = type_registry_preset ? alloc_mem(type_registry_preset) : NULL;
    strcpy(Self.type_registry_preset, type_registry_preset);

    Self.type_registry = type_registry;

    Self.url = alloc_mem(url);
    strcpy(Self.url, url);

    Self.request_id = 1;

    // websocket connection options
    if (ws_options) {
        Self.ws_options = malloc(sizeof(ws_options));

        Self.ws_options->max_size = !Self.ws_options->max_size ?  (long) pow(2, 32) : 0;
        Self.ws_options->read_limit = !Self.ws_options->read_limit ?  (long) pow(2, 32) : 0;
        Self.ws_options->write_limit = !Self.ws_options->write_limit ?  (long) pow(2, 32) : 0;
    }

    if (Self.url && (!strcmp(slice(Self.url, buffer, 0, 5), "wss://") || !strcmp(slice(Self.url, buffer, 0, 4), "ws://"))) {
        zero_buffer();
        connect_websocket();
    }
    else if (websocket) 
        Self.websocket = websocket;

    Self.default_handlers = malloc(sizeof(_d_andler));
    strcpy(Self.default_handlers->content_type, "application/json");
    strcpy(Self.default_handlers->cache_control, "no-cache");

    Self.config = malloc(sizeof(_c_def));
    Self.config->use_remote_preset = use_remote_preset ? use_remote_preset : true;
    Self.config->auto_discover = auto_discover ? auto_discover : true; 
    Self.config->auto_reconnect = auto_reconnect ? auto_reconnect : true;

    // Self.session = session();

    // reload_type_registry(use_remote_preset, auto_discover);

    return &Self;
} 

/**
* @brief Connect to a websocket
*/

static void connect_websocket()
{
    if (Self.url && (!strcmp(slice(Self.url, buffer, 0, 5), "wss://") || !strcmp(slice(Self.url, buffer, 0, 4), "ws://"))) {
        printf("Connecting to %s ...\n", Self.url);

        // zero out buffer
        zero_buffer();

        // buffer contain the address now.
        strstr(Self.url, "wss") ? slice(Self.url, buffer, 6, strlen(Self.url) - 6) : slice(Self.url, buffer, 6, strlen(Self.url) - 5);

        // convert the address into Numerics and copy into buffer
        
        // create connection
        Self.websocket = strstr(Self.url, "127.0") ? connect_websock(8000, 0, 1000, buffer) : connect_websock(8000, 0, 1000, ip_to_url(buffer));
        zero_buffer();
    }
}

/**8000
* @brief Close the websocket
*
* @return Returns 0 on success, -1 otherwise.
*/
// int close_websocket() 
// {
//     return close_ws();
// }

/**
* @brief Method that handles the actual RPC request to the Substrate node. The other implemented functions eventually
*        use this method to perform the request.

* @param result_handler: Callback function that processes the result received from the node
* @param method: method of the JSONRPC request
* @param params: a list containing the parameters of the JSONRPC request

* @returns a dict with the parsed result of the request.
*/

// void rpc_request(char* method, char** params, void* result_handler)
// {
//     struct Payload pl;
//     int request_id, update_nr, subscription_id;
//     char json_string[1024];
//     char json_body[4048];

//     request_id = Self.request_id;
//     Self.request_id++;

//     pl.jsonrpc = 2.0;
//     strcpy(pl.method, method);
//     pl.params = params;
//     pl.id = request_id;

//     printf("RPC request #%s: \"%s\"", request_id, method);

//     if (Self.websocket) {
//         // convert to JSON string then send
//         strcpy(json_string, json_dump_payload(&pl));
//         zero_buffer();

//         // send
//         if (websocket_send(json_string) == -1) {
//             // try to reconnect and send
//             if (Self.config.auto_reconnect && strlen(Self.url) > 0) {
//                 printf("%s", "Will reconnect and try again...");
//                 connect_websocket();

//                 rpc_request(method, params, result_handler);
//             } else {
//                 // Fatal Error
//                 printf("%s", "Error: Could not reach server");
//                 exit(1);
//             }
//         }

//         while (!json_body) {
//             // load into the 
//             json_load_append(Self.rpc_message_queue);

//             printf("%s" , Self.rpc_message_queue->data);
//         }
//     }
// }




