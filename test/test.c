
#include <stdio.h>
#include <string.h>

struct Payload {
    double jsonrpc;
    char method[100];
    char** params;
    int id;
    
};

char* json_dump_payload(struct Payload* p);
char* slice(const char* str, char* result, size_t start, size_t end);

char buffer[1024];
char buf [1024];


// returns slice of string
char* slice(const char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
    return result;
}


int main(void) {

    struct Payload pl;
    char* p[] = {
        "Spain", "France", NULL
    };

    pl.jsonrpc = 2.0;
    strcpy(pl.method, "get_storage");
    pl.params = p;
    pl.id = 100;

    puts(json_dump_payload(&pl));

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
