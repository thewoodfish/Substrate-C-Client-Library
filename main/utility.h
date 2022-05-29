// UTILITY FUNCTIONS
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ws.h"


// global buffer
char buffer[10];


// ***************************** UTILITY FUNCTIONS ****************************************//
char* slice(const char* str, char* result, size_t start, size_t end);

// returns slice of string
char* slice(const char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
    return result;
}


// ***************************** WEB SOCKET FUNCTIONS ****************************************//
/**
 * @brief Called when a client connects to the server.
 *
 * @param client Client connection. The @p client parameter is used
 * in order to send messages and retrieve informations about the
 * client.
 */

void onopen(ws_cli_conn_t *client)
{
	char *cli;
	cli = ws_getaddress(client);

    #ifndef DISABLE_VERBOSE
        printf("Connection opened, addr: %s\n", cli);
    #endif
}

/**
 * @brief Called when a client disconnects to the server.
 *
 * @param client Client connection. The @p client parameter is used
 * in order to send messages and retrieve informations about the
 * client.
 */
void onclose(ws_cli_conn_t *client)
{
	char *cli;
	cli = ws_getaddress(client);

    #ifndef DISABLE_VERBOSE
        printf("Connection closed, addr: %s\n", cli);
    #endif
}

/**
 * @brief Creates a web socket connection object
 *
 * @return the connection object
 * 
 */
ws_events* connect_websocket(uint16_t port, int thread_loop,
	uint32_t timeout_ms) {

    struct ws_events ev;

    ev.onopen  = &onopen;
    ev.onclose  = &onclose;

    ws_socket(&ev, port, thread_loop, timeout_ms); 

    return &ev;
}