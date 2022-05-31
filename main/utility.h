


// extern variables
extern char* buffer;
extern struct Dh _d_andler;
extern struct Config _c_def;

// Websocket connection options
struct Ws_option {
    long max_size;
    long read_limit;
    long write_limit;

};

// RPC message queue (a linked list)
struct Rmq {
    char* data;
    struct Rmq* next;
}; 

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
    double jsonrpc;
    char method[100];
    char** params;
    int id;
    
} _____PL;

extern char* slice(const char* str, char* result, size_t start, size_t end);
extern char* ip_to_url(char* url);
extern char* clear_n_copy(char* dest, char* source);
extern char* json_dump_payload(struct Payload* p);
extern char* alloc_mem(const char* str);
void zero_buffer();



