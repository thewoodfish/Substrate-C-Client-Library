#include "../wsServer/ws.h"
#include <stdint.h>

extern int connect_websock(uint16_t port, int thread_loop,
	uint32_t timeout_ms, char* address);

extern void onopen(ws_cli_conn_t *client);
extern void onclose(ws_cli_conn_t *client);
extern int websocket_send(char* str);
extern int close_ws();



