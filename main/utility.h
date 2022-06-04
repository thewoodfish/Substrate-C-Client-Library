


// extern variables
extern char* buffer;
extern int flag;
extern struct Dh _d_andler;
extern struct Config _c_def;

// Websocket connection options
struct Ws_option {
    long max_size;
    long read_limit;
    long write_limit;
};

// chain properties
struct Props {
    int ss58Format;
    int tokenDecimals;
    char tokenSymbol[6];
} __Pr;

// runtime configuration
struct Runtime_Config {
    int ss58_format;
} __R_con;

// metadate decoder
struct Metadata_Decoder {
    bool portable_registry;
} __Met_data;

// RPC message queue (a linked list)
struct Req_queue {
    char jsonrpc[5];
    char* result;
    int id;
    bool err_flag;
    struct Req_queue* next;
} __RMQ; 

struct Dh {
    char content_type[17];
    char cache_control[10];
} _d_andler;

struct Config {
    bool use_remote_preset;
    bool auto_discover;
    bool auto_reconnect;
} _c_def;

struct Payload {
    char jsonrpc[10];
    char* method;
    char** params;
    int id;
    
} _____PL;

extern char* slice(const char* str, char* result, size_t start, size_t end);
extern char* ip_to_url(char* url);
extern char* clear_n_copy(char* dest, const char* source);
extern char* json_dump_payload(struct Payload* p);
extern char* alloc_mem(const char* str);
extern bool zero_buffer();
extern void parse_json_string(struct Req_queue* rmq, char* buf);
extern void reset_flag();
extern void append_rpc_message(struct Req_queue* req);
extern void remove_rpc_message(struct Req_queue* req);
extern void parse_system_props(struct Props* p, char* buf);
static bool in_array(char** array, char* str);
extern void str_replace(const char* str_x, const char* old_x, const char* new, char* rbuf, bool once);





