#include <stdint.h>
#include "../websocket/wsclient.h"


extern int connect_websock(const char* address);

extern int onclose(wsclient *c);

extern int onerror(wsclient *c, wsclient_error *err);

// extern int websocket_send(char* str);
extern int close_ws();

int onmessage(wsclient *c, wsclient_message *msg);


