#include <stdint.h>
#include <stdlib.h>

int base64_encode(unsigned char *source, size_t sourcelen, char *target, size_t targetlen);
void _base64_encode_triple(unsigned char triple[3], char result[4]);
int _base64_decode_triple(char quadruple[4], unsigned char *result);
size_t base64_decode(char *source, unsigned char *target, size_t targetlen);



