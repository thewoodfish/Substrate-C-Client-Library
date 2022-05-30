
#include <stdio.h>
#include "jsonlib/cisson.h"


#define CISSON_IMPLEMENTATION

struct Payload {
    double jsonrpc;
    char method[100];
    char** params;
    int id;
    
};

int main(void) {

    struct Payload pl;
    char **p = {
        "Spain", "France"


    pl.jsonrpc = 2.0;
    strcpy(pl.method, "get_storage");
    pl.params = p;
    pl.id = 100;

    json_dump_payload(pl);

}

char* json_dump_payload(struct Payload* p) 
{
    char buffer[100];
    char buf[35];
    int j;
    struct tree tree = {0};

    sprintf(buffer, "{\"jsonrpc\":%d,\"method\":%s,\"params\":[],\"id\":%i}", p->jsonrpc, p->method, p->id);
    rjson(buffer, &tree);

    struct token* param = query(&tree, "/params");

    while (*p->params) {
        sprintf(buf, "%s", **p->params);
        insert_token(&tree, buf, param);
    }
    
    puts(to_string(&tree));
}
