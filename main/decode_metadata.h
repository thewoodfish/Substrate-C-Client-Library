

extern void decode_metadata(char* meta);
extern char* md_to_string(char* str);
extern int md_to_int(const char* str);
static void loop(int c, char* str);
static void loop2(int c, char* str, char* fl);
static void md_decode_array(char* buf, void md_decode_elem(char* str));
static int md_decode_compact();
static void inc_md_by(int i);






