
#include <stdio.h>
#include <string.h>
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
    struct Req_queue* next;
} __RMQ; 

char* json_dump_payload(struct Payload* p);
char* slice(const char* str, char* result, size_t start, size_t end);
extern struct Req_queue* parse_json_string(char* buf);

char buffer[1024];
char buf [1024];


// returns slice of string
char* slice(const char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
    return result;
}


int main(void) {

    // struct Payload pl;
    // char* p[] = {
    //     "Spain", "France", NULL
    // };

    // pl.jsonrpc = 2.0;
    // strcpy(pl.method, "get_storage");
    // pl.params = p;
    // pl.id = 100;

    // puts(json_dump_payload(&pl));


    char buf[] = "{\"jsonrpc\":\"2.0\",\"result\":\"4.0.0-dev-e9c1aac\",\"id\":\"1\"}";
    parse_json_string(buf);

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

struct Req_queue* parse_json_string(char* buf) {
    // first allocate RMQ struct
    struct Req_queue* rmq = malloc(sizeof(__RMQ));
    
    char* str = buf;
    char* s1 = rmq->jsonrpc;

    char space[1024];
    char* s2 = space;

    char box[10];
    char* s3 = box;

    // some sort of stack base pointer
    char* usp;

    int i, count;

    // extract result, id and version
    i = count = 0;

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
                // parse "result"

                // keep base pointer
                usp = str;

                str += 2; // skip ':"'
                while (*str != '"') {
                    count++;
                    str++;
                }

                // return back to usp
                str = usp;

                str += 2;
                while (*str != '"') {
                    *s2 = *str;
                    str++; s2++;
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
    rmq->result = (char*) malloc(count + 1);
    strcpy(rmq->result, space);

    rmq->id = atoi(box);

    return rmq;
}
