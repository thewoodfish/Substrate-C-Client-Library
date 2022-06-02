
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
    struct Substrate* substrate;
    char* param[] = {NULL};

    substrate = init_client("ws://127.0.0.1:9944", 0, 42, NULL, "substrate-node-template", NULL, NULL, true, NULL, true, true);
    fprintf(stderr, "%s\n", rpc_request("system_version", param, NULL));
}
