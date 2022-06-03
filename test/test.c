
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

struct Payload {
    double jsonrpc;
    char method[100];
    char** params;
    int id;
    
};

// RPC message queue (a linked list)
struct Req_queue {
    char jsonrpc[5];
    char* result;
    int id;
    int error;
    struct Req_queue* next;
} __RMQ; 


// chain properties
struct Props {
    int ss58Format;
    int tokenDecimals;
    char tokenSymbol[6];
} __Pr;

char* json_dump_payload(struct Payload* p);
char* slice(const char* str, char* result, size_t start, size_t end);
void parse_json_string(char* buf);
void parse_system_props(struct Props* p, char* buf);
void str_replace(const char* str, const char* old, const char* new, char* buf);

char buffer[1024];
char buf [1024];


// returns slice of string
char* slice(const char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
    return result;
}


int main(void) {

    // struct Payload pl;
    struct Props* p;

    // char* p[] = {
    //     "Spain", "France", NULL
    // };

    // pl.jsonrpc = 2.0;
    // strcpy(pl.method, "get_storage");
    // pl.params = p;
    // pl.id = 100;

    // puts(json_dump_payload(&pl));

    // char buf[] = "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":null}";
    // char buf[] = "{\"jsonrpc\":\"2.0\",\"result\":{\"ss58Format\":0,\"tokenDecimals\":10,\"tokenSymbol\":\"DOT\"},\"id\":\"1\"}";
    // // char buf[] = "{\"jsonrpc\":\"2.0\",\"result\":\"4.0.0-dev-e9c1aac\",\"id\":\"1\"}";
    // parse_json_string(buf);

    // // printf("%s\n", slice("wss://westend-rpc.polkadot.io", buffer, 0, 6));

    // char space[] = "ss58Format\":0,\"tokenDecimals\":10,\"tokenSymbol\":\"DOT";
    // p = (struct Props*) malloc(sizeof(__Pr));

    // parse_system_props(p, space);

    // printf("The ss58 format is %d\n", p->ss58Format);
    // printf("The token decimal is %d\n", p->tokenDecimals);
    // printf("The token symbol is %s\n", p->tokenSymbol);
    char* buf = (char *) malloc(255);
    str_replace("Samaritan operating system", "operating", "flower", buf);


}

char* json_dump_payload(struct Payload* p) 
{
    int j, count = 0;
    char dummy[1024];

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

void parse_json_string(char* buf) {
    // first allocate RMQ struct
    struct Req_queue* rmq = malloc(sizeof(__RMQ));
    
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

        printf("%s\n", rmq->result);
        printf("%s\n", box);

        rmq->error = 0;
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
        rmq->error = 1;

        sprintf(rmq->result, "%s: %s", box, space);

    }

    free(rmq);
    free(space);
    free(box);
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

void str_replace(const char* str_x, const char* old_x, const char* new, char* rbuf) {
    int c, n; 
    char* s1;
    char* str;
    char *s2;
    char* old;
    char* esp; // pointer of some sort 
    char* rb;
    char* s3;
    char* buf;
    int j;

    buf = (char*) malloc(strlen(rbuf) + 1);
    strcpy(buf, rbuf);

    rb = buf;

    str = (char*) malloc(strlen(str_x) + 1);
    strcpy(str, str_x);

    s1 = str;

    old = (char*) malloc(strlen(old_x) + 1);
    strcpy(old, old_x);

    s2 = old;

    c = j = 0;

    while (*s1) {
        if (*s1 == *s2) {
            esp = s1;
            while (*s1 == *s2) {
                s1++; s2++; c++;
            }

            // compare and replace if need be
            if (c = strlen(str)) {
                // copy into buffer
                s3 = str;

                printf("%d", j);

                sprintf(rbuf, "%s%s%s", buf, new, s1);
                // break;
            }
        }
        *rb = *s1;
        s1++; rb++;
    }

    printf("Output: %s\n", rbuf);

    free(str);
    free(old);
}



