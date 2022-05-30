
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include "../wsServer/ws.h"


ws_cli_conn_t* sock_client;

int connect_websock(uint16_t port, int thread_loop,
	uint32_t timeout_ms, char* address);



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

    // set global variable for subsequent use
    sock_client = client;

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
 * @return the connection descriptor
 * 
 */
int connect_websock(uint16_t port, int thread_loop,
	uint32_t timeout_ms, char* address) 
{
    struct ws_events ev;

    ev.onopen  = &onopen;
    ev.onclose  = &onclose;

    return ws_socket(&ev, port, thread_loop, timeout_ms, address); 
}


/**
 * @brief Sends a string to server
 *
 * @return the number of bytes written (-1 if error)
 * 
 */
int websocket_send(char* str) {
    return ws_sendframe_txt(sock_client, str);
}

/**
* @brief Close the websocket
*
* @return Returns 0 on success, -1 otherwise.
*/
int close_ws() {
    return ws_close_client(sock_client); 
}