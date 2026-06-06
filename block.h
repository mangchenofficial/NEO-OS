#ifndef BLOCK_H
#define BLOCK_H

#include "types.h"

#define SECTOR_SIZE 512

typedef struct {
    u8 data[SECTOR_SIZE];
} sector_t;

void block_init();
int block_read(u32 lba, u8* buffer, size_t count);
int block_write(u32 lba, const u8* buffer, size_t count);

#endif
