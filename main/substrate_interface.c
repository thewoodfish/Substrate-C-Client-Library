/* 
 * Author: Woodfish
 * File: substrate_interface.h
 * Desc: A specialized library to interface with a Substrate node.
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
void init_client(
    const char* url, int websocket, int ss58_format, void* type_registry, const char* type_registry_preset, void* cache_region, void* runtime_config,
    bool use_remote_preset, struct Ws_option* ws_options, bool auto_discover, bool auto_reconnect
) {
    if ((strlen(url) == 0 && !websocket) || (strlen(url) > 0 && websocket)) {
        printf("%s", "Either 'url' or 'websocket' must be provided\n");
        exit(1);
    }

    char* qbuf = (char*) malloc(10); // dummy buffer used for comparisons

    // runtime config 
    Self.runtime_config = runtime_config;
    Self.cache_region = cache_region;

    // allocate space for properties
    Self.properties = (struct Props*) malloc(sizeof(__Pr));
    Self.properties->ss58Format = ss58_format ? ss58_format : 42; // default is 42

    // allocate space for runtime config parameters
    Self.runtime_config = (struct Runtime_Config*) malloc(sizeof(__R_con));

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

    if (Self.url && ((!strcmp(slice(Self.url, buffer, 0, 6), "wss://") && zero_buffer()) || !strcmp(slice(Self.url, qbuf, 0, 5), "ws://"))) {
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

    free(qbuf);

    // Self.session = session();

    // reload_type_registry(use_remote_preset, auto_discover);

} 

/**
* @brief Connect to a websocket
*/

static void connect_websocket()
{
    char* qbuf = (char*) malloc(10); // dummy buffer used for comparisons

    if (Self.url && (!strcmp(slice(Self.url, buffer, 0, 6), "wss://") || !strcmp(slice(Self.url, qbuf, 0, 5), "ws://"))) {
        printf("Connecting to %s ...\n", Self.url);

        // zero out buffer
        zero_buffer();

        // create connection and return socket descriptor
        Self.websocket = connect_websock(Self.url);
    }

    free(qbuf);
}

/**
* @brief Close the websocket
*
* @return Returns 0 on success, -1 otherwise.
*/
void close_websocket() 
{
    close_ws();

    // clear all RPC messages
    free_all_mem();
}

/**
* @brief Method that handles the actual RPC request to the Substrate node. The other implemented functions eventually
*        use this method to perform the request.

* @param result_handler: Callback function that processes the result received from the node
* @param method: method of the JSONRPC request
* @param params: a list containing the parameters of the JSONRPC request

* @returns a struct with the parsed result of the request.
*/

static char* rpc_request(char* method, char** params, void* result_handler)
{
    struct Payload pl;
    int request_id, update_nr, subscription_id;
    char* json_string;
    char* json_body = NULL;
    struct Req_queue* req; 
    struct Req_queue* rmq;

    // first allocate RMQ struct
    req = (struct Req_queue*) malloc(sizeof(__RMQ));
    
    request_id = Self.request_id;
    Self.request_id++;

    strcpy(pl.jsonrpc, "2.0");

    pl.method = alloc_mem(method);
    strcpy(pl.method, method);

    pl.params = params;
    pl.id = request_id;

    fprintf(stderr, "RPC request #%d: \"%s\"\n", request_id, method);

    if (Self.websocket) {
        // convert to JSON string
        json_string = alloc_mem(json_dump_payload(&pl));
        strcpy(json_string, json_dump_payload(&pl));
        zero_buffer();

        // send
        if (websocket_send(json_string) == -1) {
            // try to reconnect and send
            if (Self.config->auto_reconnect && strlen(Self.url) > 0) {
                printf("%s", "Connection closed: Trying to reconnect...");
                connect_websocket();

                rpc_request(method, params, result_handler);
            } else {
                // Fatal Error
                printf("%s", "Error: Could not reach server");
                exit(1);
            }
        }
        
        while (!json_body) {
            // receive message from server into buffer
            if (websocket_recv(buffer) == -1)
                fprintf(stderr, "%s", "Error, could not read from socket!\n");

            // busy wait on queue
            while (!flag)   ;
            reset_flag();

            // extract fields & data from JSON string. Check for errors
            parse_json_string(req, buffer);
            zero_buffer();

            // check for errors
            if (req->err_flag) {
                fprintf(stderr, "Error %s\n", req->result);
                // clear result
                memset(req->result, 0x00, strlen(req->result));
            }

            // chain to linked list {to be freed later}
            append_rpc_message(req);

            /******** WEBSOCKET SUBSCRIPTIONS ***********/

            // // search for subscriptions
            // rmq = Self.rpc_message_queue;
            // while (rmq != NULL) {
            //     if (rmq->id && rmq->id == request_id) {

            //         if (result_handler != NULL) {
            //             // Set subscription ID and only listen to messages containing this ID
            //             subscription_id = rmq->id;
            //             fprintf(stderr, "Websocket subscription [%d] created", subscription_id);
            //         } else {
            //             json_body = alloc_mem(rmq->result);
            //             strcpy(json_body, rmq->result);
            //         }

            //         // remove from queue
            //         remove_rpc_message(rmq);
            //     }
            // }
            /******** WEBSOCKET SUBSCRIPTIONS ***********/

            json_body = req->result;

        }

        return json_body;
    }
}

/**
* @brief Free all RPC message queue
*/
static void free_all_mem() {
    while (Self.rpc_message_queue != NULL) {
        remove_rpc_message(Self.rpc_message_queue);
        Self.rpc_message_queue = Self.rpc_message_queue->next;
    }
}

char* sc_name() {
    char* buf;
    char** param = NULL;

    // check for errors
    if (buf = rpc_request("system_name", param, NULL)) {
        if (Self.name == NULL) {
            Self.name = alloc_mem(buf);
            strcpy(Self.name, buf);
        }
    }

    return Self.name;
}

struct Props* sc_properties() {
    char* buf;
    char** param = NULL;

    if (!strlen(Self.properties->tokenSymbol)) {
        buf = rpc_request("system_properties", param, NULL);
        if (strcmp(buf, "empty")) { // if buf is not "empty"
            parse_system_props(Self.properties, buf);
        }
    }

    return Self.properties;
}

char* sc_chain() {
    char* buf;
    char** param = NULL;

    // check for errors
    if (buf = rpc_request("system_chain", param, NULL)) {
        if (Self.chain == NULL) {
            Self.chain = alloc_mem(buf);
            strcpy(Self.chain, buf);
        }
    }

    return Self.chain;
}

char* sc_version() {
    char* buf;
    char** param = NULL;

    // check for errors
    if (buf = rpc_request("system_version", param, NULL)) {
        if (Self.version == NULL) {
            Self.version = alloc_mem(buf);
            strcpy(Self.version, buf);
        }
    }

    return Self.version;
}

int sc_token_decimals() {
    if (!Self.token_decimals) 
        if (Self.properties->tokenDecimals) 
            Self.token_decimals = Self.properties->tokenDecimals;
    
    return Self.token_decimals;
}

int set_token_decimal(int val) {
    Self.token_decimals = val;

    return Self.token_decimals;
}

char* sc_token_symbol() {
    if (!strlen(Self.token_symbol)) {
        if (strlen(Self.properties->tokenSymbol)) 
            strcpy(Self.token_symbol, Self.properties->tokenSymbol);
        else 
            strcpy(Self.token_symbol, "UNIT");
    }

    return Self.token_symbol;
}

char* set_token_symbol(const char* token) {
    clear_n_copy(Self.token_symbol, token);

    return Self.token_symbol;
}

int sc_ss58_format() {
    if (!Self.ss58_format) 
        if (Self.properties->ss58Format) 
            Self.ss58_format = Self.properties->ss58Format;
    else
        Self.ss58_format = 42;
    
    return Self.ss58_format;
}

int set_ss58_format(int val) {
    Self.ss58_format = val;
    Self.runtime_config->ss58_format = val;

    return Self.ss58_format;
}