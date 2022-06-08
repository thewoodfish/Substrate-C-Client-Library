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
char* buffer;

// for chain methods
char* chain_method;

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
    char* special_params[] = { "chain_getHead", "chain_getBlockHash","chain_getBlock", "chain_getHeader", "state_getMetadata", "chain_getRuntimeVersion", NULL };

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
    // check if its as error response
    if (strstr(buf, "error"))
        parse_rpc_error(rmq, buf);
    else
        parse_block_hash(rmq, buf);
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

        free(req->result);
        free(req->jsonrpc);
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

        free(start->result);
        free(start->jsonrpc);
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

static void parse_rpc_error(struct Req_queue* rmq,char* buf)
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
                while (*str != '"') {
                    *s2 = *str;
                    str++; s2++;
                }
            }
            i++;
        }
        str++;
    }

    rmq->result = (char*) malloc(strlen(space) + strlen(box) + 3);
    rmq->err_flag = 1;

    sprintf(rmq->result, "%d: %s", atoi(box), space);

    free(space);
    free(box);
}

static void parse_block_hash(struct Req_queue* rmq, char* buf) 
{
    char* mv;
    char* str = buf;
    char* end = &buf[strlen(buf) - 1];
    int bytes;
    char* s1 = rmq->jsonrpc;
    mv = end;

    // metadata requires a lot of space
    if (!strcmp(chain_method, "state_getMetadata")) {
        bytes = 70410;
    }
    else    
        bytes = 3072;
    
    char* space = (char*) malloc(bytes);
    char* s2 = space;

    char* box = (char*) malloc(10);
    char* s3 = box;

    int i, count, j, c, n, b;

    // extract result, id and version
    i = count = j = n = c = b = 0;

    while (*str) {
        // count the number of colons in string
        if (*str == ':') 
            c++;

        // count the number of brackets 
        // some "results" are not bracketed
        if (*str == '{')
            b++;

        str++;
    }

    // restore pointer
    str = buf;

    while (*str) {
        if (*str == ':') {
            // parse "jsonrpc"clea
            if (!i) {
                str += 2; // skip ':"'
                while (*str != '"') {
                    *s1 = *str;
                    str++; s1++;
                }
            } else if (i == 1) {
                // skip first char

                if (*(str + 1) != 'n')
                    str += 2;
                else 
                    str++;

                // reset pointer
                mv = end;

                // start from the back and stop at the first bracket
                mv--; 
                if (b > 1) {
                    while (*mv != '}')
                        mv--;
                } else {
                    // response does not contain brackets
                    // define a new stop
                    while (*mv != ',')
                        mv--;

                    // if result not null
                    if (*(str) != 'n')
                        mv--; // one more time
                }
                // length of memory to allocate
                count = mv - str + 2;

                while (str != mv) {
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

    rmq->err_flag = 0;
    rmq->id = atoi(box);

    free(space);
    free(box);

}

struct Block* parse_and_cache_block(char* buf, const char* method) 
{
    // this function parses block strings for methods like
    // -chain_getHeader, -chain_getBlock etc.

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
    bool get_block;

    // logs shouldn't be more than 10, hopefully
    char* logs[10];
    char* new_logs[10];

    int n, i, j, lc;

    // allocate memory for logs
    for (j = 0; j < 10; j++) 
        logs[j] = (char*) malloc(LOG_MEMORY_LENGTH);
        
    // difference between the funtion tells on some variables
    if (!strcmp(method, "getBlock")) {
        n = 0;
        get_block = true;
    } else {
        n = 2;
        get_block = false;
    }

    str = buf;
    i = lc = 0;
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
                // skip log if youre not getting block
                if (get_block) {
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
            new_logs[j] = alloc_mem(logs[j]);
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

    // assign pointers
    blovk->block_number = (int) strtol(number, NULL, 0);
    blovk->parant_hash = parent_hash;
    blovk->state_root = state_root;
    blovk->extrinsic_root = extrinsics_root;
    blovk->extrinsics = extrinsics;
    blovk->justifications = justifications;
    blovk->blok_log = bl_logs;

    if (get_block)
        append_block(blovk);

    return blovk;
}

static void append_block(struct Block* new) {
    struct Block* end = Self.block_cache;
    int i;

    i = 0;
    
    if (end == NULL) 
        end = new;
    else {
        // else keep looping unti end
        while (end != NULL) 
            end = end->next;

        end->next = new;
    }

    // check for block cache limit
    // if limit is reached, delete from the back
    
    // count the blocks
    end = Self.block_cache;
    while (end) {
        i++;
        end = end->next;
    }

    if (i > CACHE_LENGTH) 
        remove_block(Self.block_cache);
}

/**
 * @brief Delete message from the block cache
 * 
 * @param req A pointer to the block structure
 * 
 */
static void remove_block(struct Block* blovk) {
    struct Block* start;
    struct Block* prev;
    int j;

    start = Self.block_cache;

    if (start == blovk) {
        if (start->next != NULL) 
            Self.block_cache = start->next;
        else 
            Self.block_cache = NULL;

        // free all the logs first
        for (j = 0; j < blovk->blok_log->logs_count; j++)
            free(blovk->blok_log->logs[j]);

        free(blovk->extrinsic_root);
        free(blovk->extrinsics);
        free(blovk->justifications);
        free(blovk->parant_hash);
        free(blovk->state_root);
        free(blovk);

        return;

    } else {        
        while (start != NULL && start != blovk) {
            prev = start;
            start = start->next;
        }

        if (start == NULL)
            return;

        prev->next = start->next;

        // free block logs
        for (j = 0; j < start->blok_log->logs_count; j++)
            free(start->blok_log->logs[j]);

        free(blovk->extrinsic_root);
        free(blovk->extrinsics);
        free(blovk->justifications);
        free(blovk->parant_hash);
        free(blovk->state_root);
        free(start);
    }
}

struct Runtime_Version* decode_runtime_string(const char* buf)
{
    char* str;
    char* spec_name;
    char* impl_name;
    char* auth_version;
    char* spec_version;
    char* impl_version;
    char* trans_version;
    char* state_version;

    spec_name = (char*) malloc(96);
    impl_name = (char*) malloc(96);
    auth_version = (char*) malloc(10);
    spec_version = (char*) malloc(10);
    impl_version = (char*) malloc(10);
    trans_version = (char*) malloc(10);
    state_version = (char*) malloc(10);

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
    strcpy(Self.run_version->spec_name, spec_name);
    strcpy(Self.run_version->impl_name, impl_name);
    Self.run_version->authoring_version = atoi(auth_version);
    Self.run_version->spec_version = atoi(spec_version);
    Self.run_version->impl_version = atoi(impl_version);
    Self.run_version->transaction_version = atoi(trans_version);
    Self.run_version->state_version = atoi(state_version);

    // free the memory allocated
    free(spec_name);
    free(impl_name);
    free(auth_version);
    free(spec_version);
    free(impl_version);
    free(trans_version);
    free(state_version);

    return Self.run_version;
}