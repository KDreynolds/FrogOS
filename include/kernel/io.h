#ifndef IO_H
#define IO_H

#include <stdint.h>

void print(const char* str);
void print_hex(uint64_t num);
void system_shutdown(void);

#endif // IO_H