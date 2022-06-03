
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

    init_client("ws://127.0.0.1:9944", 0, 42, NULL, "substrate-node-template", NULL, NULL, true, NULL, true, true);
    set_ss58_format(45);
    fprintf(stderr, "The Samaritan coin symbol is %d\n", sc_ss58_format()); 

    set_token_decimal(10);

    // sc_properties();
    close_websocket();
}
