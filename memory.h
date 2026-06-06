#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define PAGE_SIZE 4096

void memory_init(void* map, uint32_t count);
uint32_t memory_get_total();
uint32_t memory_get_free_pages();
uint32_t memory_get_used_pages();
void* alloc_page();
void free_page(void* addr);

#endif