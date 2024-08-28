#include "kernel/io.h"
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768 // Supports up to 4GB of RAM

static uint32_t memory_bitmap[BITMAP_SIZE];
static uint64_t total_memory;

void pmm_init(uint64_t mem_size) {
    print("PMM: Initializing...\n");
    total_memory = mem_size;
    
    print("PMM: Marking all pages as used...\n");
    // Initially mark all pages as used
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        memory_bitmap[i] = 0xFFFFFFFF;
        if (i % 1000 == 0) {
            print(".");  // Print a dot every 1000 iterations to show progress
        }
    }
    print("\n");

    print("PMM: Calculating number of pages...\n");
    // Calculate number of pages and mark them as free
    size_t num_pages = mem_size / PAGE_SIZE;
    print("PMM: Number of pages: ");
    print_hex(num_pages);
    print("\n");

    print("PMM: Marking pages as free...\n");
    for (size_t i = 0; i < num_pages; i++) {
        memory_bitmap[i / 32] &= ~(1 << (i % 32));
        if (i % 1000 == 0) {
            print(".");  // Print a dot every 1000 iterations to show progress
        }
    }
    print("\n");

    print("PMM: Initialization complete.\n");
}

void* pmm_alloc_page() {
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        if (memory_bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                if (!(memory_bitmap[i] & (1 << j))) {
                    memory_bitmap[i] |= (1 << j);
                    return (void*)(((i * 32) + j) * PAGE_SIZE);
                }
            }
        }
    }
    return NULL; // Out of memory
}

void pmm_free_page(void* page_address) {
    uintptr_t addr = (uintptr_t)page_address;
    size_t page_index = addr / PAGE_SIZE;
    memory_bitmap[page_index / 32] &= ~(1 << (page_index % 32));
}

uint64_t pmm_get_free_memory() {
    uint64_t free_pages = 0;
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        for (int j = 0; j < 32; j++) {
            if (!(memory_bitmap[i] & (1 << j))) {
                free_pages++;
            }
        }
    }
    return free_pages * PAGE_SIZE;
}