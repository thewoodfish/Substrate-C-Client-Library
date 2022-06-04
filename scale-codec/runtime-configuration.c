/*
 * Author: Woodfish
 * Desc: Runtime Config for a substrate node
 * Date: Jun 3 19:02
 * 
 * (c) Algorealm
 */

#include <stdbool.h>
#include <string.h>
#include "runtime-configuration.h"
#include "../main/utility.h"


void init(int config_id, int ss58_format, bool only_primitives_on_init) 
{
    _Self.config_id = config_id;
    _Self.type_registry = (struct Types*) malloc(sizeof(__Ty));
    _Self.intitial_state = false;

    _Self.only_primitives_on_init = only_primitives_on_init;
    _Self.ss58_format = ss58_format;
}

// @classmethod
//     def convert_type_string(cls, name):

//         name = re.sub(r'T::', "", name)
//         name = re.sub(r'^T::', "", name, flags=re.IGNORECASE)
//         name = re.sub(r'<T>', "", name, flags=re.IGNORECASE)
//         name = re.sub(r'<T as Trait>::', "", name, flags=re.IGNORECASE)
//         name = re.sub(r'<T as Trait<I>>::', "", name, flags=re.IGNORECASE)
//         name = re.sub(r'<T as Config>::', "", name, flags=re.IGNORECASE)
//         name = re.sub(r'<T as Config<I>>::', "", name, flags=re.IGNORECASE)
//         name = re.sub(r'\n', "", name)
//         name = re.sub(r'^(grandpa|session|slashing|limits|beefy_primitives|xcm::opaque)::', "", name)
//         name = re.sub(r'VecDeque<', "Vec<", name, flags=re.IGNORECASE)
//         name = re.sub(r'^Box<(.+)>$', r'\1', name, flags=re.IGNORECASE)

//         if name == '()':
//             return "Null"
//         if name.lower() in ['vec<u8>', '&[u8]', "& 'static[u8]"]:
//             return "Bytes"
//         if name.lower() == '<lookup as staticlookup>::source':
//             return 'LookupSource'
//         if name.lower() == '<balance as hascompact>::type':
//             return 'Compact<Balance>'
//         if name.lower() == '<blocknumber as hascompact>::type':
//             return 'Compact<BlockNumber>'
//         if name.lower() == '<moment as hascompact>::type':
//             return 'Compact<Moment>'
//         if name.lower() == '<inherentofflinereport as inherentofflinereport>::inherent':
//             return 'InherentOfflineReport'

//         return name

char* convert_type_string(char* name) 
{
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
    str_replace_special(name, false);  // special case

    if (!strcmp(name, "()")) 
        return "Null";
    if (str)

    
}