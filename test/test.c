
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../main/substrate_interface.h"

#define LOG_MEMORY_LENGTH 512
#define CACHE_LENGTH 10


struct Payload {
    double jsonrpc;
    char method[100];
    char** params;
    int id;
    
};

struct Runtime_Version {
    char* spec_name;
    char* impl_name;
    int authoring_version;
    int spec_version;
    int impl_version;
    int transaction_version;
    int state_version;
} __RunVerse;

struct Block {
    int block_number;
    char* parant_hash;
    char* state_root;
    char* extrinsic_root;
    char* extrinsics;
    char* justifications;
    struct Block_log* blok_log;
    struct Block* next;
} __Blovk;

struct Block_log {
    int block_no;
    int logs_count;
    char** logs;
} __Logz;

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
void decode_runtime_string(const char* buf);


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
    // char buf[] = "{\"jsonrpc\":\"2.0\",\"result\":{\"ss58Format\":0,\"tokenDecimals\":10,\"tokenSymbol\":\"DOT\"},\"id\":\"1\"}";
    // char buf[] = "{\"jsonrpc\":\"2.0\",\"result\":{\"block\":{\"header\":{\"parentHash\":\"0x800dbd6f47c8d76e5dc7d3409c3431e8d1154917bcf850a8ed5fa166f88e1066\",\"number\":\"0x4\",\"stateRoot\":\"0x71b41e8e9b92717b2a8a2ad4c97105e462970eb2f416ef4131f026bee105bc01\",\"extrinsicsRoot\":\"0x40a0a1ee1dbaa34a261fe7f2dc219fc14901ae354a5530615d719b4e771f83cb\",\"digest\":{\"logs\":[\"0x066175726120e9846f1000000000\",\"0x0561757261010148299fc995bc1100a5e60ec8400fa758e1d79fdaa58f4bd8893c2c6506a2ed635518cd46ceda6f92027a23aaf6baf13f281357a7fee854a5e9edf2d32cc08a85\"]}},\"extrinsics\":[\"0x280402000bf014bb358101\"]},\"justifications\":null},\"id\":\"2\"}";
    
    // char buf[] = "\"block\":{\"header\":{\"parentHash\":\"0x5650462b8284066efa29d46ed8c6e8f265da49d94de2c9a5ad663ebf8e87d3fe\",\"number\":\"0xa\",\"stateRoot\":\"0x1abea41538cd04daf6d439616a3c9c1a478142733cfd3fc3dc439c1efd62cca1\",\"extrinsicsRoot\":\"0xb22479f86cd53dc80c225e2352ab2700b4a99950391678db75427bd62c0c838f\",\"digest\":{\"logs\":[\"0x066175726120d8a76f1000000000\",\"0x056175726101013684d67f71a162adc9b756b0f0098a22eea961d0542f075b5e57ad48f69a767e17c2aedc768b393b2a208137764e48eb52cc8497c3e0aabd750946af6e96b284\"]}},\"extrinsics\":[\"0x280402000b80d6ed388101\"]},\"justifications\":null";
    char buf[] = "\"specName\":\"node-template\",\"implName\":\"node-template\",\"authoringVersion\":1,\"specVersion\":100,\"implVersion\":1,\"apis\":[[\"0xdf6acb689907609b\",4],[\"0x37e397fc7c91f5e4\",1],[\"0x40fe3ad401f8959a\",6],[\"0xd2bc9897eed08f15\",3],[\"0xf78b278be53f454c\",2],[\"0xdd718d5cc53262d4\",1],[\"0xab3c0572291feb8b\",1],[\"0xed99c5acb25eedf5\",3],[\"0xbc9d89904f5b923f\",1],[\"0x37c8bb1350a9a2a8\",1]],\"transactionVersion\":1,\"stateVersion\":1";

    // parse_and_cache_block(buf);
    decode_runtime_string(buf);

    
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
    char* str;
    char* parent_hash;
    char* number;
    char* state_root;
    char* extrinsics_root;
    char* extrinsics;
    char* justifications;
    struct Block* blovk;
    struct Block_log* bl_logs;

    // allocate memory for block and logs
    blovk = (struct Block*) malloc(sizeof(__Blovk));
    bl_logs = (struct Block_log*) malloc(sizeof(__Logz));

    parent_hash = (char*) malloc(96);
    number = (char*) malloc(10);
    state_root = (char*) malloc(96);
    extrinsics_root = (char*) malloc(96);
    extrinsics = (char*) malloc(96);
    justifications = (char*) malloc(96);
    
    char* s1 = parent_hash;
    char* s2 = number;
    char* s3 = state_root;
    char* s4 = extrinsics_root;
    char* s5;
    char* s6 = extrinsics;
    char* s7 = justifications;

    // logs shouldn't be more than 10, hopefully
    char* logs[10];
    char* new_logs[10];

    int n, i, j, lc;

    // allocate memory for logs
    for (j = 0; j < 10; j++) 
        logs[j] = (char*) malloc(LOG_MEMORY_LENGTH);
        

    str = buf;
    n = i = lc = 0;
    s5 = logs[i];


    while (*str) {
        if (*str == ':') {
            if (n == 2) {
                // get parent hash
                str += 2;
                while (*str != '"') {
                    *s1 = *str;
                    str++; s1++;
                }
            } else if (n == 3) {
                // get blovk number (string)
                str += 2;
                while (*str != '"') {
                    *s2 = *str;
                    str++; s2++;
                }
            } else if (n == 4) {
                // get state root
                str += 2;
                while (*str != '"') {
                    *s3 = *str;
                    str++; s3++;
                }
            } else if (n == 5) {
                // get extrinsics root
                while (!isalnum(*str)) 
                    str++;

                while (*str != '"') {
                    *s4 = *str;
                    str++; s4++;
                }
            } else if (n == 7) {
                // get each log
                str += 3;

                while (*str != '}') {
                    while (*str != '"') {
                        *s5 = *str;
                        str++; s5++;
                    }

                    str += 2;
                    i++; str++;
                    s5 = logs[i];
                }
            } else if (n == 8) {
                // get extrinsics
                str += 3;
                while (*str != '"') {
                    *s6 = *str;
                    str++; s6++;
                }
            } else if (n == 9) {
                str++;
                while (*str) {
                    *s7 = *str;
                    str++; s7++;
                }
            }
    
            n++;
        }

        str++;
    }

    for (j = 0; j < 10; j++) {
        if (isdigit(logs[j][0])) {
            new_logs[j] = (char*) malloc(LOG_MEMORY_LENGTH);
            strcpy(new_logs[j], logs[j]);

            // increment logs count
            lc++;
        }
        // free logs, because the memory they occupy somehow makes no sense
        free(logs[j]);
    }

    // set up logs
    bl_logs->block_no = (int) strtol(number, NULL, 0);
    bl_logs->logs_count = lc;
    bl_logs->logs = new_logs;

    for (j = 0; j < bl_logs->logs_count; j++)
        printf("%s\n", bl_logs->logs[j]);

    // assign pointers
    blovk->block_number = (int) strtol(number, NULL, 0);
    blovk->parant_hash = parent_hash;
    blovk->state_root = state_root;
    blovk->extrinsic_root = extrinsics_root;
    blovk->extrinsics = extrinsics;
    blovk->justifications = justifications;
    blovk->blok_log = bl_logs;

    // append_block(blovk);

    return blovk;
}


void decode_runtime_string(const char* buf)
{
    char* str;
    char* spec_name;
    char* impl_name;
    char* auth_version;
    char* spec_version;
    char* impl_version;
    char* trans_version;
    char* state_version;
    struct Runtime_Version* runv;

    spec_name = (char*) malloc(96);
    impl_name = (char*) malloc(96);
    auth_version = (char*) malloc(10);
    spec_version = (char*) malloc(10);
    impl_version = (char*) malloc(10);
    trans_version = (char*) malloc(10);
    state_version = (char*) malloc(10);

    runv = Self.run_version;
    
    char* s1 = spec_name;
    char* s2 = impl_name;
    char* s3 = auth_version;
    char* s4 = spec_version;
    char* s5 = impl_version;
    char* s6 = trans_version;
    char* s7 = state_version;

    int n, i, j, lc;

    str = (char*) buf;
    n = i = lc = 0;

    while (*str) {
        if (*str == ':') {
            if (!n) {
                str += 2;
                while (*str != '"') {
                    *s1 = *str;
                    str++; s1++;
                }
            } else if (n == 1) {
                str += 2;
                while (*str != '"') {
                    *s2 = *str;
                    str++; s2++;
                }
            } else if (n == 2) {
                str++;
                while (*str != '"') {
                    *s3 = *str;
                    str++; s3++;
                }
            } else if (n == 3) {
                str++;
                while (*str != '"') {
                    *s4 = *str;
                    str++; s4++;
                }
            } else if (n == 4) {                
                str++;
                while (*str != '"') {
                    *s5 = *str;
                    str++; s5++;
                }
            } else if (n == 6) {
                str++;
                while (*str != '"') {
                    *s6 = *str;
                    str++; s6++;
                }
            } else if (n == 7) {
                str++;
                while (*str) {
                    *s7 = *str;
                    str++; s7++;
                }
            }
    
            n++;
        }

        str++;
    }

    // assign values
    strcpy(runv->spec_name, spec_name);
    strcpy(runv->impl_name, impl_name);
    runv->authoring_version = atoi(auth_version);
    runv->spec_version = atoi(spec_version);
    runv->impl_version = atoi(impl_version);
    runv->transaction_version = atoi(trans_version);
    runv->state_version = atoi(state_version);

    // free the memory allocated
    free(spec_name);
    free(impl_name);
    free(auth_version);
    free(spec_version);
    free(impl_version);
    free(trans_version);
    free(state_version);

    return runv;
}