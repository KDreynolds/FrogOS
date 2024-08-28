#include <stdint.h>
#include "kernel/pmm.h"
#include "kernel/io.h"
#include "kernel/uart.h"


// Function to print a string
void print(const char* str) {
    uart_puts(str);
}

void print_hex(uint64_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 60; i >= 0; i -= 4) {
        uart_putc(hex_chars[(num >> i) & 0xF]);
    }
}

void delay(int count) {
    for (volatile int i = 0; i < count; i++) {
        for (volatile int j = 0; j < 10000; j++) {
            __asm__("nop");
        }
    }
}

// Kernel main function
void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3) {
    (void)dtb_ptr32; // Mark as unused
    (void)x1;
    (void)x2;
    (void)x3;


    uart_init();
    print("Kernel started.\n");
    delay(100);

    // For now, let's assume we have 128MB of RAM
    uint64_t mem_size = 128 * 1024 * 1024;

    print("Initializing Physical Memory Manager...\n");
    delay(100);
    pmm_init(mem_size);
    delay(100);

    print("Total memory: ");
    print_hex(mem_size);
    print(" bytes\n");
    delay(100);

    print("Calculating free memory...\n");
    delay(100);
    uint64_t free_mem = pmm_get_free_memory();
    delay(100);
    if (free_mem == 0) {
        print("Error calculating free memory.\n");
    } else {
        print("Free memory: ");
        print_hex(free_mem);
        print(" bytes\n");
    }
    delay(100);

    print("Physical Memory Manager test complete.\n");
    delay(100);

    while(1) {
        // Infinite loop to keep the kernel running
    }
}
