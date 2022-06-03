
/* 
 * Author: Woodfish,
 * File: main.c
 * Desc: Entry point for the substrate client
 * Date: May 32 13:05
 * Licence: MIT
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "substrate_interface.h"

int main(void) {
    // initialize connection to node
    char* param[] = {NULL};

    init_client("wss://polkadot-rpc.dwellir.com", 0, 42, NULL, "substrate-node-template", NULL, NULL, true, NULL, true, true);
    // fprintf(stderr, "The name of the chain is %s\n", sc_name()); 127.0.0.1:9944

    sc_properties();
    // close_websocket();
}
