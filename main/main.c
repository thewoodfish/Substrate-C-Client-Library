
/* 
 * Author: Woodfish,
 * File: main.c
 * Desc: Entry point for the substrate client
 * Date: May 32 13:05
 * Licence: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "substrate_interface.h"
#include "utility.h"
#include "../xxHash/xxhash.h"

int main(void) {
    // initialize connection to node
    struct Block* bl;
    struct Runtime_Version* runv;

    XXH128_hash_t one = XXH3_128bits("Sudo", 4);
    XXH128_hash_t two = XXH3_128bits("Key", 3);
    char concat[2048];

    
    // set up key

    init_client("ws://127.0.0.1:9944", 0, 42, NULL, "substrate-node-template", NULL, NULL, true, NULL, true, true);
    // fprintf(stderr, "The chain head is %s\n", sc_get_chain_head()); 

    // bl = sc_get_chain_block("", "0x75");
    // runv = sc_get_block_runtime_version(bl->parent_hash);

    // if (bl)
    //     fprintf(stderr, "The parent hash is %s\n", bl->parent_hash); 
    sprintf(concat, "0x%lx%lx%lx%lx", one.low64, one.high64, two.low64, two.high64);

    // fprintf(stderr, "%s\n", concat);
    // // if (runv)
    fprintf(stderr, "The storage value is : %s\n",
     sc_get_storage_by_key("0xbbc6b5e97a2001aad9be75af3caff9f363e911f78c638c40969a9f903026aea0", concat) );

    close_websocket();
}
