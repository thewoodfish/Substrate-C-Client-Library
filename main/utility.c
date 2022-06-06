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
void clear_n_copy(char* dest, const char* source) {
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

static bool in_array(char** array, char* str) {
    int i;

    for (i = 0; array[i]; i++)
        if (!strcmp(array[i], str)) 
            return true;

    return false;
}

/**
 * @brief Converts a payload sruct to JSON string
 *
 * @param p The payload struct to convert
 */

char* json_dump_payload(struct Payload* p) 
{
    int j, count = 0;
    char* special_params[] = { "chain_getHead", "chain_getBlockHash","chain_getBlock", NULL };

    char* dummy = (char*) malloc(1024);
    char* buf = (char*) malloc(1024);

    char* sp = buf;
    char** io = p->params;

    if (io != NULL) {
        for (j = 0; io[j]; j++) {
            if (io[j + 1])
                sprintf(sp, "%s, ", io[j]);
            else
                sprintf(sp, "%s", io[j]);

            count += strlen(io[j]) + 3;
            sp += count - 1;
        }

        slice(buf, dummy, 0, count);

        if (in_array(special_params, p->method))  
            sprintf(buffer, "{\"jsonrpc\":\"%s\",\"method\":\"%s\",\"params\":[\"%s\"],\"id\":\"%i\"}", p->jsonrpc, p->method, dummy, p->id);
        else 
            sprintf(buffer, "{\"jsonrpc\":\"%s\",\"method\":\"%s\",\"params\":\"{%s}\",\"id\":\"%i\"}", p->jsonrpc, p->method, dummy, p->id);

    } else {

        if (in_array(special_params, p->method))  
            sprintf(buffer, "{\"jsonrpc\":\"%s\",\"method\":\"%s\",\"params\":[%s],\"id\":\"%i\"}", p->jsonrpc, p->method, "", p->id);
        else
            sprintf(buffer, "{\"jsonrpc\":\"%s\",\"method\":\"%s\",\"params\":\"{%s}\",\"id\":\"%i\"}", p->jsonrpc, p->method, "", p->id);
    }
    
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
            while (*match_iter!='\0' && *find_iter!='\0')
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
                while (*find_iter != '\0' &&
                      *current != '\0' &&
                      *replace_iter != '\0' )
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

            // only replace first occurrence
            if (once) break;
        }

        ++current;
    }

    // printf("The sentence after replacement: %s\n", original);
    // clear buffer
    clear_n_copy(rbuf, original);
}

void str_replace_special(char* str)
{
    // try replicating 
    // re.sub(r'^Box<(.+)>$', '', name, flags=re.IGNORECASE)

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
        clear_n_copy(str, sp);

    free(buf);
    free(space);
    free(sp);
}


void to_lower_case(char* str) {
    // convert string to lowercase
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

    buf = alloc_mem(str);
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

// char* get_type(struct Type_Reg* self, char* str) 
// {
//     struct Types *ty, *curr;

//     if (self->types) {
//         ty = self->types;
//         while (ty) {
//             if (!strcmp(ty->name, str))
//                 return ty->val;

//             ty = ty->next;
//         }
//     }

//     return NULL;
// }

struct Block* parse_and_cache_block(char* buf) 
{
    struct Block* bl;
    bl = (struct Block*) malloc(sizeof(__Blovk));

    return bl;
}
