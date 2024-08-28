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

    uart_init();  // Initialize UART

    print("Kernel started.\n");

    // For now, let's assume we have 128MB of RAM
    uint64_t mem_size = 128 * 1024 * 1024;

    print("Initializing Physical Memory Manager...\n");
    pmm_init(mem_size);

    print("PMM initialization complete.\n");
    print("Total memory: ");
    print_hex(mem_size);
    print(" bytes\n");

    print("Preparing to calculate free memory...\n");
    // Add a small delay here
    for (volatile int i = 0; i < 1000000; i++) {
        __asm__("nop");
    }

    print("Calculating free memory...\n");
    uint64_t free_mem = pmm_get_free_memory();

    print("Free memory calculation complete.\n");
    print("Free memory: ");
    print_hex(free_mem);
    print(" bytes\n");

    print("Physical Memory Manager test complete.\n");

    while(1) {
        // Infinite loop to keep the kernel running
    }
}