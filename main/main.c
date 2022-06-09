
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
    fprintf(stderr, "The storage value is : %s\n", sc_get_storage_by_key("0x5c0d1176a568c1f92944340dbfed9e9c530ebca703c85910e7164cb7d1c9e47b") );

    close_websocket();
}
