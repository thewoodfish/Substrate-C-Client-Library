// UTILITY FUNCTIONS
#include <string.h>
#include <ctype.h>
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
#include "utility.h"
#include "substrate_interface.h"


// global buffer - 4KB
char space[4096];
char* buffer = space;

// message flag { 1 to indicate a message has dropped into buffer }
int flag = 0;


// returns slice of string
char* slice(const char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
    return result;
}

/**
 * @brief Zeroes out the global buffer
 * 
 */
bool zero_buffer() {
    memset(buffer, 0x00, strlen(buffer));
    return true;
}

/**
 * @brief First clears the destination memory region and then copies into it
 *
 * @param dest The mem region to copy into
 * @param source The mem region to copy from
 * 
 */
char* clear_n_copy(char* dest, const char* source) {
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

/**
 * @brief Converts a payload sruct to JSON string
 *
 * @param p The payload struct to convert
 */

char* json_dump_payload(struct Payload* p) 
{
    int j, count = 0;
    char* dummy = (char*) malloc(1024);
    char* buf = (char*) malloc(1024);

    char* sp = buf;
    char** io = p->params;

    if (io != NULL) {
        for (j = 0; io[j]; j++) {
            sprintf(sp, "%s, ", io[j]);
            count += strlen(io[j]) + 3;
            sp += count - 1;
        }

        slice(buf, dummy, 0, count);

        sprintf(buffer, "{\"jsonrpc\":\"%s\",\"method\":\"%s\",\"params\":\"{%s}\",\"id\":\"%i\"}", p->jsonrpc, p->method, dummy, p->id);
    } else 
        sprintf(buffer, "{\"jsonrpc\":\"%s\",\"method\":\"%s\",\"params\":\"{%s}\",\"id\":\"%i\"}", p->jsonrpc, p->method, "", p->id);
    
    free(dummy);
    free(buf);

    return buffer;
}


/**
 * @brief Allocate memory 
 *
 * @param len The memory lenght to allocate
 * 
 * @returns Memory address
 */
char* alloc_mem(const char* str) {
    return (char *) malloc(strlen(str) + 1);
}


/**
 * @brief Parse JSON string into a Request Message Queue struct
 *
 * @param len The address of the JSON string
 * 
 * @returns Pointer to the RMQ
 */
void parse_json_string(struct Req_queue* rmq, char* buf)
{
    char* str = buf;
    char* s1 = rmq->jsonrpc;

    char* space = (char*) malloc(1024);
    char* s2 = space;

    char* box = (char*) malloc(10);
    char* s3 = box;

    // some sort of stack base pointer
    char* usp;

    int i, count, j;

    // extract result, id and version
    i = count = j = 0;

    if (!strstr(buf, "error")) {
        while (*str) {
            if (*str == ':') {
                // parse "jsonrpc"
                if (!i) {
                    str += 2; // skip ':"'
                    while (*str != '"') {
                        *s1 = *str;
                        str++; s1++;
                    }

                } else if (i == 1) {

                        // keep base pointer
                        usp = str;
                        while (*str) {
                            if (*str == '}') j++;
                            str++;
                        }

                        // return str back to right position
                        str = usp;

                        // if result is not a struct-like string
                        if (j == 1) {

                            str += 2; // skip ':"'
                            while (*str != '"') {
                                count++;
                                str++;
                            }

                            if (count) {
                                // return back to usp
                                str = usp;

                                str += 2;
                                while (*str != '"') {
                                    *s2 = *str;
                                    str++; s2++;
                                }
                            }
                        } else {
                            // parse struct like string
                            str += 2; // skip ':{"'

                            while (*(str) != '}') {
                                count++;
                                str++;
                            }

                            if (count) {
                                // return back to usp
                                str = usp;

                                str += 2;
                                while (*(str) != '}') {
                                    *s2 = *str;
                                    str++; s2++;
                                }
                            }
                        }
                } else {
                    str += 2; // skip ':"'
                    while (*str != '"') {
                        *s3 = *str;
                        str++; s3++;
                    }
                }

                i++;
            }
            str++;
        }

        // allocate space for result
        if (count) {
            rmq->result = (char*) malloc(count + 1);
            strcpy(rmq->result, space);
        } else {
            rmq->result = (char*) malloc(6);
            strcpy(rmq->result, "empty");
        }

        rmq->err_flag = false;
        rmq->id = atoi(box);

    } else {
        // look for error code and string
        while (*str) {
            if (*str == ':') {
                // parse "jsonrpc"
                if (!i) {
                    str += 2; // skip ':"'
                    while (*str != '"') {
                        *s1 = *str;
                        str++; s1++;
                    }
                } else if (i == 2) {
                    // parse error
                    str += 2; // skip ':-'
                    while (isdigit(*str)) {
                        *s3 = *str;
                        str++; s3++;
                    }
                } else if (i == 3) {
                    str += 2; // skip ':-'
                    while (isalpha(*str) || !ispunct(*str)) {
                        *s2 = *str;
                        str++; s2++;
                    }
                }
                i++;
            }
            str++;
        }

        rmq->result = (char*) malloc(strlen(space) + strlen(box) + 3);
        rmq->err_flag = true;

        sprintf(rmq->result, "%s: %s", box, space);

    }

    free(space);
    free(box);
}


/**
 * @brief Clear message flag
 */
void reset_flag() {
    flag = 0;
}


/**
 * @brief Append message to RMQ linked list
 * 
 * @param req A pointer to the request message
 * 
 */
void append_rpc_message(struct Req_queue* req) {
    struct Req_queue* start = Self.rpc_message_queue;
    struct Req_queue* end = Self.rpc_message_queue;
    
    if (start == NULL) 
        start = req;
    else {
        // else keep looping unti end
        while (end != NULL) 
            end = end->next;

        end->next = req;
    }
}


/**
 * @brief Delete message from RMQ linked list
 * 
 * @param req A pointer to the request message
 * 
 */
void remove_rpc_message(struct Req_queue* req) {
    struct Req_queue* start = Self.rpc_message_queue;
    struct Req_queue* prev;

    if (start == req) {
        if (start->next != NULL) 
            Self.rpc_message_queue = start->next;
        else 
            Self.rpc_message_queue = NULL;

        free(req);
        return;

    } else {        
        while (start != NULL && start != req) {
            prev = start;
            start = start->next;
        }

        if (start == NULL)
            return;

        prev->next = start->next;

        free(start);
    }
}

void parse_system_props(struct Props* p, char* buf) 
{
    // parse system properties
    char* str;
    int i;

    char* s1;
    char* s2;
    char* s3;

    i = 0;
    str = buf;

    char* buf1 = (char*) malloc(10);
    char* buf2 = (char*) malloc(10);
    char* buf3 = (char*) malloc(10);

    s1 = buf1;
    s2 = buf2;
    s3 = buf3;

    while (*str) {
        if (*str == ':') {
            if (!i) {
                str++;
                while (*str != ',') {
                    *s1 = *str;
                    s1++, str++;
                }
            } else if (i == 1) {
                str++;
                while (*str != ',') {
                    *s2 = *str;
                    s2++, str++;
                }
            } else {
                str += 2;
                while (*str != '"') {
                    *s3 = *str;
                    s3++, str++;
                }
            }
            i++;
        }
        str++;
    }

    p->ss58Format = atoi(buf1);
    p->tokenDecimals = atoi(buf2);
    strcpy(p->tokenSymbol, buf3);

    free(buf1);
    free(buf2);
    free(buf3);
}