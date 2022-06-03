
static struct Substrate {
    char* url;
    int port;
    int websocket;
    int ss58_format;
    void* type_registry;
    char* type_registry_preset;
    void* cache_region;
    void* runtime_config;
    struct Ws_option* ws_options;

    int request_id;

    // internal variables
    double version;
    char* name;
    char* properties;
    char* chain;
    
    char* token_decimal;
    char* token_symbol;

    struct Req_queue* rpc_message_queue;
    char* mock_extrinsics;

    struct Dh* default_handlers;
    int metadata_decoder;
    double runtime_version;
    double transaction_version;
    char* block_hash;
    int block_id;
    int metadata_cache;
    int type_registry_cache;
    
    bool debug; 

    struct Config* config;
    int session;

} Self ;



extern void init_client(
    const char* url, int websocket, int ss58_format, void* type_registry, const char* type_registry_preset, void* cache_region, void* runtime_config,
    bool use_remote_preset, struct Ws_option* ws_options, bool auto_discover, bool auto_reconnect
);
static void connect_websocket();
extern void close_websocket();
static char* rpc_request(char* method, char** params, void* result_handler);
extern char* sc_name();
static void free_all_rmq();
extern void sc_properties();




