#include "kernel/io.h"
#include "kernel/uart.h"

void print(const char* str) {
    uart_puts(str);
}

void print_hex(uint64_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[17];
    buffer[16] = '\0';

    for (int i = 15; i >= 0; i--) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
    }

    print("0x");
    print(buffer);
}

void system_shutdown(void) {
    // QEMU specific: write to system control block to trigger shutdown
    volatile uint32_t *scb = (volatile uint32_t *)0x9000000;
    *scb = 0x5555;  // Magic value to signal shutdown
}