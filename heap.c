#include "heap.h"
#include "memory.h"

static uint32_t heap_start = 0;
static uint32_t heap_ptr = 0;

void heap_init() {
    heap_start = (uint32_t)alloc_page();
    heap_ptr = heap_start;
}

void* kmalloc(uint32_t size) {
    uint32_t addr = heap_ptr;
    heap_ptr += size;
    
    uint32_t current_page = addr & ~0xFFF;
    uint32_t next_page = (heap_ptr + 0xFFF) & ~0xFFF;
    
    while (next_page > current_page) {
        current_page += PAGE_SIZE;
        if (current_page < heap_start + PAGE_SIZE) continue;
        alloc_page();
    }
    
    return (void*)addr;
}

void kfree(void* addr) {
    // Bump allocator 暂时不支持释放
}