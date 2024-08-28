#ifndef PMM_H
#define PMM_H

#include <stdint.h>

void pmm_init(uint64_t mem_size);
void* pmm_alloc_page();
void pmm_free_page(void* page_address);
uint64_t pmm_get_free_memory();

#endif // PMM_H
