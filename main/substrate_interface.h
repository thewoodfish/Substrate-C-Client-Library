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

/* Parameters */

// initialization structure

/*
    Fields
    ----------
    url: the URL to the substrate node, either in format https://127.0.0.1:9933 or wss://127.0.0.1:9944
    ss58_format: The address type which account IDs will be SS58-encoded to Substrate addresses. Defaults to 42, for Kusama the address type is 2
    type_registry: A struct containing the custom type registry in format: {'types': {'customType': 'u32'},..}
    type_registry_preset: The name of the predefined type registry shipped with the SCALE-codec, e.g. kusama
    cache_region: a Dogpile cache region as a central store for the metadata cache
    use_remote_preset: When True preset is downloaded from Github master, otherwise use files from local installed scalecodec package
    ws_options: struct of options to pass to the websocket-client create_connection function
*/

static struct Init_vars {
    char url[25];
    int websocket;
    int ss58_format = 42;
    void* type_registry;
    char* type_registry_preset;
    void* cache_region;
    void* runtime_config;
    Ws_option ws_options;

    int request_id;

    // internal variables
    char* version;
    char* name;
    char* properties;
    char* chain;
    
    char* token_decimal;
    char* token_symbol;
    // char* ss58_format;

    char** rpc_message_queue;
    char* mock_extrinsics;

    Dh default_handlers;
    int metadata_decoder;
    double runtime_version;
    double transaction_version;
    char block_hash[65];
    int block_id;
    int metadata_cache;
    int type_registry_cache;
    
    bool debug = false; 

    Config config;
    int session;

} Self ;


// extern variables
extern char buffer[1024];

Init_vars* init(
    char* url, int websocket, int ss58_format, void* type_registry, char* type_registry_preset, void* cache_region, void* runtime_config,
    bool use_remote_preset, Ws_option ws_options, bool auto_discover, bool auto_reconnect
);
void connect_websocket();
void rpc_request(char* method, char** params, void* result_handler);



// initialize Self struct
Init_vars* init(
    char* url, int websocket, int ss58_format, void* type_registry, char* type_registry_preset, void* cache_region, void* runtime_config,
    bool use_remote_preset, Ws_option ws_options, bool auto_discover, bool auto_reconnect
) {
    if ((strlen(url) == 0 && websocket == NULL) || (strlen(url) > 0 && websocket)) {
        printf("%s", "Either 'url' or 'websocket' must be provided");
        exit(1);
    }

    // runtime config 
    Self.runtime_config = runtime_config;
    Self.cache_region = cache_region;

    ss58_format ? Self.ss58_format = ss58_format : NULL;

    Self.type_registry_preset = type_registry_preset;
    Self.type_registry = type_registry;

    strcpy(Self.url, url);
    Self.request_id = 1;

    // websocket connection options
    Self.ws_options = ws_options;

    !Self.ws_options.max_size ? Self.ws_options.max_size = (long) pow(2, 32) : NULL;
    !Self.ws_options.read_limit ? Self.ws_options.read_limit = (long) pow(2, 32) : NULL;
    !Self.ws_options.write_limit ? Self.ws_options.write_limit = (long) pow(2, 32) : NULL;

    if (Self.url && (!strcmp(slice(Self.url, buffer, 0, 6), "wss://") || !strcmp(slice(Self.url, buffer, 0, 5), "ws://"))) {
        // get IP address from url slice
        zero_buffer();

        connect_websocket();
    }
    else if (websocket) 
        Self.websocket = websocket;

    strcpy(Self.default_handlers.content_type, "application/json");
    strcpy(Self.default_handlers.cache_control, "no-cache");

    Self.config.use_remote_preset = use_remote_preset;
    Self.config.auto_discover = auto_discover;
    Self.config.auto_reconnect = auto_reconnect;

    Self.session = session();

    reload_type_registry(use_remote_preset, auto_discover);

    return &Self;
} 

/**
* @brief Connect to a websocket
*/

static void connect_websocket()
{
    if (Self.url && (!strcmp(slice(Self.url, buffer, 0, 6), "wss://") || !strcmp(slice(Self.url, buffer, 0, 5), "ws://"))) {
        printf("Connecting to %s ...", Self.url);

        // zero out buffer
        zero_buffer();

        // buffer contain the address now.
        strstr(Self.url, "wss") ? slice(Self.url, buffer, 6, strlen(Self.url) - 6) : slice(Self.url, buffer, 6, strlen(Self.url) - 5);

        // convert the address into Numerics and copy into buffer
        
        // create connection
        Self.websocket = connect_websock(8000, 1, 1000, ip_to_url(buffer));
        zero_buffer();
    }
}

/**
* @brief Close the websocket
*
* @return Returns 0 on success, -1 otherwise.
*/
int close_websocket() 
{
    return close_ws();
}

/**
* @brief Method that handles the actual RPC request to the Substrate node. The other implemented functions eventually
*        use this method to perform the request.

* @param result_handler: Callback function that processes the result received from the node
* @param method: method of the JSONRPC request
* @param params: a list containing the parameters of the JSONRPC request

* @returns a dict with the parsed result of the request.
*/

void rpc_request(char* method, char** params, void* result_handler)
{
    Payload pl;
    int request_id, update_nr, subscription_id;
    char json_string[1024];
    char json_body[4048];

    request_id = Self.request_id;
    Self.request_id++;

    pl.jsonrpc = 2.0;
    strcpy(pl.method, method);
    pl.params = params;
    pl.id = request_id;

    printf("RPC request #%s: \"%s\"", request_id, method);

    if (Self.websocket) {
        // convert to JSON string then send
        strcpy(json_string, json_dump_payload(&pl));
        zero_buffer();

        // send
        if (websocket_send(json_string) == -1) {
            // try to reconnect and send
            if (Self.config.auto_reconnect && strlen(Self.url) > 0) {
                printf("%s", "Will reconnect and try again...");
                connect_websocket();

                rpc_request(method, params, result_handler);
            } else {
                // Fatal Error
                printf("%s", "Error: Could not reach server");
                exit(1);
            }
        }



    }

}


