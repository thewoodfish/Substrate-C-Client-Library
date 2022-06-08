
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

int main(void) {
    // initialize connection to node
    struct Block* bl;
    struct Runtime_Version* runv;

    init_client("ws://127.0.0.1:9944", 0, 42, NULL, "substrate-node-template", NULL, NULL, true, NULL, true, true);
    // fprintf(stderr, "The chain head is %s\n", sc_get_chain_head()); 

    bl = sc_get_chain_block("", "0x384");
    runv = sc_get_block_runtime_version(bl->parant_hash);

    // fprintf(stderr, "The runtime spec name is : %s\n", runv->spec_name);

    close_websocket();
}
