#include "memory.h"
#include "screen.h"

static uint32_t total_memory = 128 * 1024 * 1024;
static uint32_t free_pages = 0;
static uint32_t used_pages = 0;
static uint8_t* bitmap = 0;
static uint32_t bitmap_size = 0;
static uint32_t max_pages = 0;

static void set_bit(uint32_t index) {
    bitmap[index / 8] |= (1 << (index % 8));
}

static void clear_bit(uint32_t index) {
    bitmap[index / 8] &= ~(1 << (index % 8));
}

static uint8_t test_bit(uint32_t index) {
    return bitmap[index / 8] & (1 << (index % 8));
}

static uint32_t find_first_free() {
    for (uint32_t i = 0; i < max_pages; i++) {
        if (!test_bit(i)) {
            return i;
        }
    }
    return (uint32_t)-1;
}

void memory_init(void* map, uint32_t count) {
    max_pages = total_memory / PAGE_SIZE;
    bitmap_size = (max_pages + 7) / 8;

    // 位图放在 0x200000（2MB 处）
    bitmap = (uint8_t*)0x200000;

    for (uint32_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0;
    }

    // 标记前 256 页（1MB）为已占用
    for (uint32_t page = 0; page < 256; page++) {
        set_bit(page);
    }

    // 重新统计
    free_pages = 0;
    used_pages = 0;
    for (uint32_t i = 0; i < max_pages; i++) {
        if (test_bit(i)) {
            used_pages++;
        } else {
            free_pages++;
        }
    }
}

uint32_t memory_get_total() {
    return total_memory;
}

uint32_t memory_get_free_pages() {
    return free_pages;
}

uint32_t memory_get_used_pages() {
    return used_pages;
}

void* alloc_page() {
    uint32_t page = find_first_free();
    if (page == (uint32_t)-1) {
        return 0;
    }
    set_bit(page);
    free_pages--;
    used_pages++;
    return (void*)(page * PAGE_SIZE);
}

void free_page(void* addr) {
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    if (test_bit(page)) {
        clear_bit(page);
        free_pages++;
        used_pages--;
    }
}