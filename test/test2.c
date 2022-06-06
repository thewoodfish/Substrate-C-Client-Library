#include <stdio.h>
#include "../cScale/src/scale.h"
#include <stdbool.h>
#include <stdlib.h>


int main(void) {
    scale_compact_int compact = SCALE_COMPACT_INT_INIT;
    encode_u128_string_to_compact_int_scale(&compact, "0x066175726120966c6f1000000000");

    char *compact_hex = decode_compact_to_hex(&compact);
    uint16_t value = strtoull(compact_hex, NULL, 16);
    printf("%s = %u\n", compact_hex, value);

    free(compact_hex);

    return 0;
}