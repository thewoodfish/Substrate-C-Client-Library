/*
 * Author: Woodfish
 * Desc: Runtime Config for a substrate node
 * Date: Jun 3 19:02
 * 
 * (c) Algorealm
 */

#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "runtime-configuration.h"
#include "../main/utility.h"


void init(int config_id, int ss58_format, bool only_primitives_on_init) 
{
    _Self.config_id = config_id;
    _Self.type_registry = (struct Type_Reg*) malloc(sizeof(__Ty));
    _Self.initial_state = false;

    _Self.only_primitives_on_init = only_primitives_on_init;
    _Self.ss58_format = ss58_format;
}

void convert_type_string(char* name) 
{        
    char **types = { "vec<u8>", "&[u8]", "& 'static[u8]" };

    str_replace(name, "T::", "", name, false);
    str_replace(name, "T::", "", name, true);
    str_replace(name, "<T>", "", name, false);
    str_replace(name, "<T as Trait>::", "", name, false);
    str_replace(name, "<T as Trait<I>>::", "", name, false);
    str_replace(name, "<T as Config>::", "", name, false);
    str_replace(name, "<T as Config<I>>::", "", name, false);
    str_replace(name, "\n", "", name, false);

    // -------------------------------
    str_replace(name, "grandpa::", "", name, true);
    str_replace(name, "session::", "", name, true);
    str_replace(name, "slashing::", "", name, true);
    str_replace(name, "limits::", "", name, true);
    str_replace(name, "beefy_primites::", "", name, true);
    str_replace(name, "xcm::opaque::", "", name, true);
    // -------------------------------

    str_replace(name, "VecDeque<", "Vec<", name, false);
    str_replace_special(name);  // special case

    // name to lowercase
    to_lower_case(name);

    if (!strcmp(name, "()")) 
        clear_n_copy(name, "Null");
    else if (in_array(types, name))
        clear_n_copy(name, "Bytes");
    else if (!strcmp(name, "<lookup as staticlookup>::source"))
        clear_n_copy(name, "LookupSource");
    else if (!strcmp(name, "<balance as hascompact>::type"))
        clear_n_copy(name, "Compact<Balance>");
    else if (!strcmp(name, "<blocknumber as hascompact>::type"))
        clear_n_copy(name, "Compact<BlockNumber>");
    else if (!strcmp(name, "<moment as hascompact>::type"))
        clear_n_copy(name, "Compact<Moment>");
    else if (!strcmp(name, "<inherentofflinereport as inherentofflinereport>::inherent"))
        clear_n_copy(name, "InherentOfflineReport");

}

void get_decoder_class(char* buf, char* type_str, const char* spec_version_id)
{
    char* decoder_class;
    char* ts_lower; // type_str to lower case

    decoder_class = alloc_mem(buf);
    ts_lower = alloc_mem(type_str);

    to_lower_case(ts_lower);

    strip(type_str);
    if (!strcmp(type_str, ""))
        return;

    convert_type_string(type_str);

    strcpy(decoder_class, get_type(_Self.type_registry, ts_lower));

    if (decoder_class != NULL) {
        if (type_str[strlen(type_str) - 1] == '>') {
            
        }
    }
    
}