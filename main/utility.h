// UTILITY FUNCTIONS
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


// global buffer
char buffer[1024];


// Websocket connection options
struct Ws_option {
    long max_size;
    long read_limit;
    long write_limit;

};

struct Dh {
    char content_type[17];
    char cache_control[10];
};

struct Config {
    bool use_remote_preset = true;
    bool auto_discover = true;
    bool auto_reconnect = true;
};

struct Payload {
    double jsonrpc;
    char method[100];
    char** params;
    int id;
    
} _____PL;




// ***************************** UTILITY FUNCTIONS ****************************************//
char* slice(const char* str, char* result, size_t start, size_t end);
char* ip_to_url(char* url);
char* clear_n_copy(char* dest, char* source);
char* json_dump_payload(struct Payload* p);



// returns slice of string
char* slice(const char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
    return result;
}

/**
 * @brief Zeroes out the global buffer
 * 
 */
void zero_buffer() {
    memset(buffer, 0x00, strlen(buffer));
}

/**
 * @brief First clears the destination memory region and then copies into it
 *
 * @param dest The mem region to copy into
 * @param source The mem region to copy from
 * 
 */
char* clear_n_copy(char* dest, char* source) {
    memset(dest, 0x00, strlen(dest));
    strcpy(dest, source);
}

/**
 * @brief Converts a URL into its IP address
 *
 * @param url The url to convert to IP
 */
char* ip_to_url(char* url) {
    char* IPbuf;
    struct hostent* host_entry;

    if ((host_entry = gethostbyname(url)) == NULL) {
        perror("Could not resolve address");
        exit(1);
    }

    IPbuf = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    return IPbuf;
}

char* json_dump_payload(struct Payload* p) 
{
    int j, count = 0;
    char dummy[1024];
    char buf[1024];

    char *sp = buf;
    char** io = p->params;

    for (j = 0; io[j]; j++) {
        sprintf(sp, "%s, ", io[j]);
        count += strlen(io[j]) + 3;
        sp += count - 1;
    }

    sprintf(buffer, "{\"jsonrpc\":\"%f\",\"method\":\"%s\",\"params\":\"%s\",\"id\":\"%i\"}", p->jsonrpc, p->method, slice(buf, dummy, 0, count), p->id);
    
    return buffer;
}

