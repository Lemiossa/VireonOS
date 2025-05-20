#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <types.h>

void print_int(int num);
void print_uint(uint num);
void print_hex(uint num, int width);

/* long */
void print_long(long num);
void print_ulong(ulong num);
void print_hex_long(ulong num, int width);

#endif /* PRINT_UTILS_H */
