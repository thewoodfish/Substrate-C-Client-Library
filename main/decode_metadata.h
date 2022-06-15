

void decode_metadata(char* decoded_meta, char* metadata);
extern char* md_to_string(char* str);
extern int md_to_int(const char* str);
static void loop(int c, char* str);
static void loop2(int c, char* str, char* fl);
static void md_decode_array(char* buf, void md_decode_elem(char* str));
int md_decode_compact();
static void inc_md_by(int i);
inline static void assign(char* tmp);
static void md_decode_extrinsics(char* buf);
static void md_decode_error(char* buf);
static void md_decode_constant(char* buf);
static void md_decode_event(char* buf);
static void md_decode_events(char* buf);
static void md_decode_call_arg(char* buf);
static void md_decode_call(char* buf);
static void md_decode_calls(char* buf);
static void md_decode_storage_hasher(char* str);
static void md_assign_byte(char* tmp);
static void md_decode_storage_type(char* buf);
static void md_decode_bool(char* buf);
static void md_decode_storage_modifier(char* str);
static void md_decode_storage_entry(char* buf);
static void md_decode_storage(char* buf);
static void md_decode_module(char* buf);
static void md_decode_option(char* buf, void md_decode_opt(char* str));
static void md_decode_string_array(char* str);
static void md_decode_byte_array(char* buf);
static void md_decode_string(char* buf);


























