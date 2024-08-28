#include <stdint.h>
#include "kernel/pmm.h"
#include "kernel/io.h"

// UART registers
#define UART0_BASE 0x09000000
#define UART0_DR   (UART0_BASE + 0x00)
#define UART0_FR   (UART0_BASE + 0x18)

// Function to write a character to UART
void uart_putc(unsigned char c) {
    // Wait for UART to become ready to transmit
    while (*((volatile uint32_t *)(UART0_FR)) & (1 << 5));
    *((volatile uint32_t *)(UART0_DR)) = c;
}

// Function to print a string
void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        uart_putc((unsigned char)str[i]);
    }
}

void print_hex(uint64_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 60; i >= 0; i -= 4) {
        uart_putc(hex_chars[(num >> i) & 0xF]);
    }
}

// Kernel main function
void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3) {
    (void)dtb_ptr32; // Mark as unused
    (void)x1;
    (void)x2;
    (void)x3;

    // For now, let's assume we have 128MB of RAM
    uint64_t mem_size = 128 * 1024 * 1024;

    print("Initializing Physical Memory Manager...\n");
    pmm_init(mem_size);

    print("Total memory: ");
    print_hex(mem_size);
    print(" bytes\n");

    print("Free memory: ");
    print_hex(pmm_get_free_memory());
    print(" bytes\n");

    print("Allocating a page...\n");
    void* page = pmm_alloc_page();
    if (page) {
        print("Allocated page at address: ");
        print_hex((uint64_t)page);
        print("\n");
    } else {
        print("Failed to allocate page!\n");
    }

    print("Free memory after allocation: ");
    print_hex(pmm_get_free_memory());
    print(" bytes\n");

    print("Freeing the allocated page...\n");
    pmm_free_page(page);

    print("Free memory after freeing: ");
    print_hex(pmm_get_free_memory());
    print(" bytes\n");

    print("Physical Memory Manager test complete.\n");

    while(1) {
        // Infinite loop to keep the kernel running
    }
}
