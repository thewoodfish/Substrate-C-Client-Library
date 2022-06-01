
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
#include "../websocket/wsclient.h"
#include "network.h"
#include "utility.h"

// global ws client
wsclient cl_main;


int onclose(wsclient *c) {
	fprintf(stderr, "Connection closed: %d\n", c->sockfd);
	return 0;
}

int onerror(wsclient *c, wsclient_error *err) {
	fprintf(stderr, "Error: (%d): %s\n", err->code, err->str);
	if(err->extra_code) {
		errno = err->extra_code;
		perror("recv");
	}
	return 0;
}

int onmessage(wsclient *c, wsclient_message *msg) {
	// fprintf(stderr, "onmessage: (%llu): %s\n", msg->payload_len, msg->payload);

	// copy into buffer
	clear_n_copy(buffer, msg->payload);
	flag = 1; // set flag

	return 0;
}

int onopen(wsclient *c) {
	fprintf(stderr, "Connection opened: %d\n", c->sockfd);
	return 0;
}

int connect_websock(const char* str) {
    wsclient* client = libwsclient_new(str);

    if (!client) {
		fprintf(stderr, "Unable to initialize new WS client.\n");
		exit(1);
	}

    //set callback functions for this client
	libwsclient_onopen(client, &onopen);
	libwsclient_onmessage(client, &onmessage);
	libwsclient_onerror(client, &onerror);
	libwsclient_onclose(client, &onclose);

    libwsclient_helper_socket(client, "test.sock");

	// starts run thread.
	libwsclient_run(client);

	cl_main = *client;

    return client->sockfd;
}

int close_ws() {
	// close connection
	libwsclient_finish(&cl_main);
	libwsclient_close(&cl_main);
}

// send string to server
int websocket_send(const char* str) {
	return libwsclient_send(&cl_main, str);
}

// recive from server
int websocket_recv(char* str) {
	return _libwsclient_read(&cl_main, str, strlen(str));
}
