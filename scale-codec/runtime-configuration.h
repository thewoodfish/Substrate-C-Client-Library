#include <stdbool.h>

static struct Self_config {
    int config_id;
    struct Types* type_registry;
    bool intitial_state;
    int active_spec_version;
    int chain_id;

    bool only_primitives_on_init;
    int ss58_format;

} _Self;

struct Types {
    char** types;
} __Ty;

extern void init(int config_id, int ss58_format, bool only_primitives_on_init);