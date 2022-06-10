
struct Substrate {
    char* url;
    int port;
    int websocket;
    void* type_registry;
    char* type_registry_preset;
    void* cache_region;
    struct Runtime_Config* runtime_config;
    struct Ws_option* ws_options;

    int request_id;

    // internal variables
    char* version;
    char* name;
    struct Props* properties;
    char* chain;

    struct Req_queue* rpc_message_queue;
    char* mock_extrinsics;

    int token_decimals;
    char token_symbol[6];
    int ss58_format;

    struct Dh* default_handlers;
    int metadata_decoder;
    struct Metadata_Decoder* m_decoder;
    int block_id;
    int metadata_cache;
    int type_registry_cache;
    
    bool debug; 
    char* block_hash;

    struct Config* config;
    int session;

    int runtime_version;
    int transaction_version;

    struct Block* block_cache;

    struct Runtime_Version* run_version;

} Self ;




extern void init_client(
    const char* url, int websocket, int ss58_format, void* type_registry, const char* type_registry_preset, void* cache_region, void* runtime_config,
    bool use_remote_preset, struct Ws_option* ws_options, bool auto_discover, bool auto_reconnect
);
static void connect_websocket();
extern void close_websocket();
static char* rpc_request(char* method, char** params, void* result_handler);
extern char* sc_name();
static void free_all_mem();
extern struct Props* sc_properties();
extern char* sc_chain();
extern char* sc_version();
extern int sc_token_decimals();
extern int set_token_decimal(int val);
extern char* sc_token_symbol();
extern char* set_token_symbol(const char* token);
extern int sc_ss58_format();
extern int set_ss58_format(int val);
extern char* sc_get_chain_head();
extern char* sc_get_chain_finalised_head();
extern char* sc_get_block_hash(const char* block_id);
extern struct Block* sc_get_chain_block(const char* block_hash, const char* block_id);
static void add_param(char** param, char* buf);
extern int sc_get_block_number(const char* block_hash);
extern char* sc_get_metadata(const char* block_hash);
static bool is_error(const char* buf);
static void possibly_exit_rudely(void);
extern char* sc_get_storage_by_key(const char* key);
extern void sc_get_block_runtime_version(struct Runtime_Version* runv, const char* block_hash);
extern char* generate_storage_hash(const char* storage_module, const char* storage_function, char** list, char** hashers);
extern void init_runtime(const char* block_hash, const char* block_id);













