#include <stdbool.h>

static struct Self_config {
    int config_id;
    struct Type_Reg* type_registry; 
    bool initial_state;
    char* active_spec_version_id;
    int chain_id;

    bool only_primitives_on_init;
    int ss58_format;

} _Self;

struct Type_Reg {
    struct Types* types; // implement this as a linked list
} __T_reg;

struct Types {
    char* name;
    char* val;
    struct Types* next;
} __Ty;

extern void init(int config_id, int ss58_format, bool only_primitives_on_init);
extern void convert_type_string(char* name);
