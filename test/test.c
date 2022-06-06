
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

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
void str_replace(const char* str, const char* old, const char* new, char* buf, bool once);
void str_replace_special(char* str);
void to_lower_case(char* str);
void clear_n_copy(char* dest, const char* source);
void strip(char* str);
struct Block* parse_and_cache_block(char* buf);
void parse_block_hash(char* buf);
void parse_rpc_error(char* buf);



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
    // char* buf = (char *) malloc(255);
    // str_replace("open open -- open", "open", "close", buf, true);

    // printf("%s\n", buf);

    // char* str = "I am aBox veryBox<(jhwj392989apdoism/k>nice boy";
    // str_replace_special(str);

    // char buf[] = "ABCDEFG";
    // to_lower_case(buf);

    // printf("%s\n", buf);

    // char str[] = "mike";

    // printf("%c\n", str[strlen(str) - 1]);

    // strip(str);
    
    // printf("%s\n", str);
    char buf[] = "{\"jsonrpc\":\"2.0\",\"result\":{\"ss58Format\":0,\"tokenDecimals\":10,\"tokenSymbol\":\"DOT\"},\"id\":\"1\"}";
    // char buf[] = "{\"jsonrpc\":\"2.0\",\"result\":{\"block\":{\"header\":{\"parentHash\":\"0x800dbd6f47c8d76e5dc7d3409c3431e8d1154917bcf850a8ed5fa166f88e1066\",\"number\":\"0x4\",\"stateRoot\":\"0x71b41e8e9b92717b2a8a2ad4c97105e462970eb2f416ef4131f026bee105bc01\",\"extrinsicsRoot\":\"0x40a0a1ee1dbaa34a261fe7f2dc219fc14901ae354a5530615d719b4e771f83cb\",\"digest\":{\"logs\":[\"0x066175726120e9846f1000000000\",\"0x0561757261010148299fc995bc1100a5e60ec8400fa758e1d79fdaa58f4bd8893c2c6506a2ed635518cd46ceda6f92027a23aaf6baf13f281357a7fee854a5e9edf2d32cc08a85\"]}},\"extrinsics\":[\"0x280402000bf014bb358101\"]},\"justifications\":null},\"id\":\"2\"}";
    
    parse_json_string(buf);
}


void clear_n_copy(char* dest, const char* source) {
    memset(dest, 0x00, strlen(dest));
    strcpy(dest, source);
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

    // parse string in case of errors
    if (strstr(buf, "error"))
        parse_rpc_error(buf);
    else
        // if its a block that was queried (get_blockHash)
        parse_block_hash(buf);
//     else {
//         while (*str) {
//             if (*str == ':') {
//                 // parse "jsonrpc"
//                 if (!i) {
//                     str += 2; // skip ':"'
//                     while (*str != '"') {
//                         *s1 = *str;
//                         str++; s1++;
//                     }

//                 } else if (i == 1) {
// mv = end;

//                 // start from the back and stop at the first bracket
//                 mv--; str++;
//                 while (*mv != '}') {
//                     mv--;
//                 }

//                 // length of memory to allocate
//                 count = mv - str;

//                 while (str != (mv + 1)) {
//                     *s2 = *str;

//                     // reduce colon counts too as we proceed
//                     if (*str == ':') c--;

//                     str++; s2++;                    
//                 }
                
//                 } else {
//                     str += 2; // skip ':"'
//                     while (*str != '"') {
//                         *s3 = *str;
//                         str++; s3++;
//                     }
//                 }

//                 i++;
//             }
//             str++;
//         }

//         // allocate space for result
//         if (count) {
//             rmq->result = (char*) malloc(count + 1);
//             strcpy(rmq->result, space);
//         } else {
//             rmq->result = (char*) malloc(6);
//             strcpy(rmq->result, "empty");
//         }

//         printf("%s\n", rmq->result);
//         printf("%s\n", box);

//         rmq->error = 0;
//         rmq->id = atoi(box);

//     }
}

void parse_rpc_error(char* buf)
{
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
    // parse rpc error
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

void str_replace(const char* str_x, const char* old_x, const char* new, char* rbuf, bool once)
{
    // https://codereview.stackexchange.com/questions/236212/find-and-replace-a-string-in-c-c-without-using-standard-library

    enum SIZE
    {
        ARRAY_MAX = 50
    };

    char original[ARRAY_MAX];
    char find[ARRAY_MAX];
    char replace[ARRAY_MAX];

    strcpy(original, str_x);
    strcpy(find, old_x);
    strcpy(replace, new);

    char* current = original;
    while (*current != '\0')
    {
        if (*current == find[0])
        {
            char* match_iter = current;
            char* find_iter = find;
            int match = 0;
            while (*match_iter != '\0' && *find_iter != '\0')
            {
                if (*match_iter == *find_iter)
                    match = 1;
                else {
                    match = 0;
                    break;
                }

                match_iter++;
                find_iter++;
            }

            if (match)
            {
                // printf("the whole word matched\n");
                find_iter = find;
                char* replace_iter = replace;
                while(*find_iter != '\0' &&
                      *current != '\0' &&
                      *replace_iter != '\0')
                {
                    *current = *replace_iter;
                    ++find_iter;
                    ++replace_iter;
                    ++current;
                }
                if (*find_iter != '\0' &&
                    *replace_iter == '\0')
                {
                    // printf("match is longer than replace\n");

                    char* move_left = current;
                    while(*find_iter != '\0' &&
                          *move_left != '\0')
                    {
                        ++find_iter;
                        ++move_left;
                    }

                    char* temp_current = current;
                    while(*move_left != '\0' &&
                          *temp_current != '\0')
                    {
                        *temp_current = *move_left;
                        ++temp_current;
                        ++move_left;
                    }

                    *temp_current = '\0';
                }
                else if (*find_iter == '\0' &&
                         *replace_iter != '\0')
                {
                    // printf("replace is longer than match\n");
                    char* move_right = current;
                    char temp[ARRAY_MAX];
                    char* temp_iter = temp;
                    while(*replace_iter != '\0')
                    {
                        *temp_iter = *current;
                        *current = *replace_iter;
                        ++current;
                        ++temp_iter;
                        ++replace_iter;
                    }

                    char* current_to_end = current;
                    while(*current_to_end != '\0')
                    {
                        *temp_iter = *current_to_end;
                        ++temp_iter;
                        ++current_to_end;
                    }

                    *temp_iter = '\0';

                    temp_iter = temp;
                    char* temp_current = current;
                    while(*temp_iter != '\0')
                    {
                        *temp_current = *temp_iter;
                        ++temp_current;
                        ++temp_iter;
                    }
                    *temp_current = '\0';
                }
                // else if (*find_iter == '\0' &&
                //          *replace_iter == '\0')
                // {
                //     // printf("replace and match are same length\n");
                // }

            }

            if (once) break;
        }

        ++current;
    }

    // printf("The sentence after replacement: %s\n", original);
    strcpy(rbuf, original);
}

void str_replace_special(char* str)
{
    char* s1;
    char* s2;
    int i;
    char* buf;
    char *fl;
    char *space;
    char* s3;
    char* sp;
    bool j;

    char start[] = "Box<(";
    s1 = str;
    s2 = start;

    i = 0;
    buf = (char*) malloc(255);
    space = (char*) malloc(255);

    sp = (char*) malloc(255);
    s3 = sp;

    fl = space;
    j = false;

    while (*s1) {
        if (*s1 == *s2) {
            // clear memory
            memset(sp, 0, 255);
            while (*s1 == *s2) {
                *s3 = *s1;
                s1++; s2++; i++; s3++; 
            }


            if (i == strlen(start)) {
                i = 0;
                // let s1 continue
                while (*s1 != '>') 
                    s1++;

                // bring together
                j = true;
                memset(sp, 0, 255);
                sprintf (sp, "%s%s", space, (s1 + 1));
                break;
            } else {
                // update fl
                s3 = sp;

                while(*s3) {
                    *fl = *s3;
                    fl++; s3++;
                }

                i = 0;
                s2 = start;
            }

        }

        *fl = *s1;
        s1++; fl++;
    }

    if (j)
        printf("%s\n", sp);
    else 
        printf("%s\n", str);

    free(buf);
    free(space);
    free(sp);
}

// convert string to lowercase
void to_lower_case(char* str) {
    char *s;
    s = str;

    while (*s) {
        *s = tolower(*s);
        s++;
    }
}

void strip(char* str) {
    char* s;
    char* b;
    char* buf;
    char* s1;

    s = str;
    b = &str[strlen(str) - 1];

    buf = (char*) malloc(strlen(str) + 1);
    s1 = buf;

    // skip leading whitespaces
    while (isspace(*s)) s++;

    // skip trailing whitespaces
    while (isspace(*b)) b--;

    while (*s && s != (b + 1)) {
        *s1 = *s;
        s++; s1++;
    }

    clear_n_copy(str, buf);
    free(buf);
}

void parse_block_hash(char* buf) 
{
    // first allocate RMQ struct
    struct Req_queue* rmq = malloc(sizeof(__RMQ));
    char* mv;
    
    char* str = buf;
    char* end = &buf[strlen(buf) - 1];
    char* s1 = rmq->jsonrpc;
    mv = end;

    char* space = (char*) malloc(1024);
    char* s2 = space;

    char* box = (char*) malloc(10);
    char* s3 = box;

    int i, count, j, c, n;

    // extract result, id and version
    i = count = j = n = c = 0;

    // count the number of colons in string
    while (*str) {
        if (*str == ':') 
            c++;
        str++;
    }

    // restore pointer
    str = buf;

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
                mv = end;

                // start from the back and stop at the first bracket
                mv--; str++;
                while (*mv != '}') {
                    mv--;
                }

                // length of memory to allocate
                count = mv - str;

                while (str != (mv + 1)) {
                    *s2 = *str;

                    // reduce colon counts too as we proceed
                    if (*str == ':') c--;

                    str++; s2++;                    
                }
                
            } else if (i == (c - 1)) {
                // last colon, extract id

                mv = end;
                // move mv backwards at least 10 times backwards, that would have covered the id
                n = 0;
                while (n < 11) {
                    mv--; n++;
                }

                // now mv can loop forward to extract the digits of the id
                while (*mv) {
                    while (isdigit(*mv))  {
                        *s3 = *mv;
                        mv++; s3++;
                    }
                    mv++;
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

    printf("%s\n\n", rmq->result);
    printf("%s\n", box);

    rmq->error = 0;
    rmq->id = atoi(box);

}

struct Block* parse_and_cache_block(char* buf) 
{
    // "block":{"header":{"parentHash":"0x800dbd6f47c8d76e5dc7d3409c3431e8d1154917bcf850a8ed5fa166f88e1066","number":"0x4",
    // "stateRoot":"0x71b41e8e9b92717b2a8a2ad4c97105e462970eb2f416ef4131f026bee105bc01","extrinsicsRoot"
    // :"0x40a0a1ee1dbaa34a261fe7f2dc219fc14901ae354a5530615d719b4e771f83cb","digest":{"logs":["0x066175726120e9846f1000000000",
    // "0x0561757261010148299fc995bc1100a5e60ec8400fa758e1d79fdaa58f4bd8893c2c6506a2ed635518cd46ceda6f92027a23aaf6baf13f281357a7fee854a5e9edf2d32cc08a85"]

    // {"jsonrpc":"2.0","result":{"block":{"header":{"parentHash":"0x800dbd6f47c8d76e5dc7d3409c3431e8d1154917bcf850a8ed5fa166f88e1066","number":"0x4",
    // "stateRoot":"0x71b41e8e9b92717b2a8a2ad4c97105e462970eb2f416ef4131f026bee105bc01","extrinsicsRoot":
    // "0x40a0a1ee1dbaa34a261fe7f2dc219fc14901ae354a5530615d719b4e771f83cb","digest":{"logs":["0x066175726120e9846f1000000000",
    // "0x0561757261010148299fc995bc1100a5e60ec8400fa758e1d79fdaa58f4bd8893c2c6506a2ed635518cd46ceda6f92027a23aaf6baf13f281357a7fee854a5e9edf2d32cc08a85"]}},
    // "extrinsics":["0x280402000bf014bb358101"]},"justifications":null},"id":"2"}

    char* str;
    char* parent_hash;
    char* number;
    char* state_root;
    char* extrinsics_root;

    // logs shouldn't be more than 10, hopefully
    char* logs[10];
    int n;

    str = buf;
    n = 0;

    while (*str) {
        if (*str == ':') {
            if (!n) {
                
            }

            n++;
        }
    }

}