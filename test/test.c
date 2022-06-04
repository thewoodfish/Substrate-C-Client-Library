
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
    str_replace("open open -- open", "open", "close", buf, true);

    printf("%s\n", buf);
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

    char start[] = "Box<(";
    s1 = str;
    s2 = start;

    i = 0;
    buf = (char*) malloc(255);
    space = (char*) malloc(255);

    fl = space;

    while (*s1) {
        if (*s1 == *s2) {
            *s1++; *s2++; i++;
        }

        if (i == strlen(start)) {
            i = 0;
            // let s1 continue
            while (*s1 != '>') 
                s1++;
        }

        *fl = *s1;
        s1++; fl++;
    }
}



