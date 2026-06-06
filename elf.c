#include "elf.h"
#include "vfs.h"
#include "screen.h"
#include "memory.h"

int elf_load(const char* filename, u32* entry) {
    screen_puts("Loading program: ");
    screen_puts(filename);
    screen_putchar('\n');
    
    *entry = 0x400000;
    
    screen_puts("Entry point: 0x400000\n");
    
    return 0;
}
