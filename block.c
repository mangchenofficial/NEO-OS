#include "block.h"
#include "ports.h"

#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECTORCOUNT 0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE       0x1F6
#define ATA_STATUS      0x1F7
#define ATA_COMMAND     0x1F7

static void ata_wait() {
    while (inb(ATA_STATUS) & 0x80);
}

static int ata_read_sector(u32 lba, u8* buffer) {
    ata_wait();
    
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTORCOUNT, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, 0x20);
    
    ata_wait();
    
    if (!(inb(ATA_STATUS) & 0x08)) {
        return -1;
    }
    
    for (int i = 0; i < 256; i++) {
        u16 data = inw(ATA_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }
    
    return 0;
}

void block_init() {
}

int block_read(u32 lba, u8* buffer, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (ata_read_sector(lba + i, buffer + i * SECTOR_SIZE) != 0) {
            return -1;
        }
    }
    return 0;
}

int block_write(u32 lba, const u8* buffer, size_t count) {
    return -1;
}
