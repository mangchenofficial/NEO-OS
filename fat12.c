#include "fat12.h"
#include "block.h"
#include "string.h"

static u16 fat12_get_fat_entry(fat12_fs_t* fs, u16 cluster) {
    u32 fat_offset = cluster + (cluster / 2);
    u32 fat_sector = fs->fat_start_lba + (fat_offset / SECTOR_SIZE);
    u32 entry_offset = fat_offset % SECTOR_SIZE;
    
    u8 buffer[SECTOR_SIZE];
    if (block_read(fat_sector, buffer, 1) != 0) {
        return 0xFFFF;
    }
    
    u16 value = *(u16*)&buffer[entry_offset];
    
    if (cluster & 1) {
        return value >> 4;
    } else {
        return value & 0x0FFF;
    }
}

static void fat12_normalize_name(const char* name, char* out) {
    int i = 0;
    int dot = -1;
    
    for (i = 0; name[i] && i < 12; i++) {
        if (name[i] == '.') {
            dot = i;
            break;
        }
    }
    
    for (i = 0; i < 8; i++) {
        if (dot == -1 && name[i]) {
            out[i] = (name[i] >= 'a' && name[i] <= 'z') ? name[i] - 32 : name[i];
        } else if (i < dot) {
            out[i] = (name[i] >= 'a' && name[i] <= 'z') ? name[i] - 32 : name[i];
        } else {
            out[i] = ' ';
        }
    }
    
    for (i = 0; i < 3; i++) {
        int ext_idx = dot == -1 ? 0 : dot + 1 + i;
        if (name[ext_idx]) {
            out[8 + i] = (name[ext_idx] >= 'a' && name[ext_idx] <= 'z') ? name[ext_idx] - 32 : name[ext_idx];
        } else {
            out[8 + i] = ' ';
        }
    }
}

int fat12_init(fat12_fs_t* fs) {
    if (block_read(0, (u8*)&fs->boot, 1) != 0) {
        return -1;
    }
    
    fs->fat_start_lba = fs->boot.reserved_sectors;
    fs->root_dir_start_lba = fs->fat_start_lba + (fs->boot.fat_count * fs->boot.sectors_per_fat);
    
    u32 root_dir_sectors = ((fs->boot.root_entries * 32) + (SECTOR_SIZE - 1)) / SECTOR_SIZE;
    fs->data_start_lba = fs->root_dir_start_lba + root_dir_sectors;
    
    return 0;
}

int fat12_open(fat12_fs_t* fs, const char* name, fat12_file_t* file) {
    char normalized[11];
    fat12_normalize_name(name, normalized);
    
    u32 root_dir_sectors = ((fs->boot.root_entries * 32) + (SECTOR_SIZE - 1)) / SECTOR_SIZE;
    
    for (u32 i = 0; i < root_dir_sectors; i++) {
        u8 sector[SECTOR_SIZE];
        if (block_read(fs->root_dir_start_lba + i, sector, 1) != 0) {
            return -1;
        }
        
        fat12_dir_entry_t* entries = (fat12_dir_entry_t*)sector;
        
        for (int j = 0; j < FAT12_FILES_PER_SECTOR; j++) {
            if (entries[j].filename[0] == 0) {
                return -1;
            }
            
            if (entries[j].filename[0] == 0xE5) {
                continue;
            }
            
            if (memcmp(entries[j].filename, normalized, 8) == 0 &&
                memcmp(entries[j].ext, normalized + 8, 3) == 0) {
                file->entry = entries[j];
                file->current_cluster = entries[j].first_cluster_low;
                file->current_offset = 0;
                return 0;
            }
        }
    }
    
    return -1;
}

int fat12_read(fat12_fs_t* fs, fat12_file_t* file, void* buffer, size_t count, size_t* read) {
    u8* out = (u8*)buffer;
    size_t total_read = 0;
    u32 bytes_left = file->entry.file_size - file->current_offset;
    
    if (count > bytes_left) {
        count = bytes_left;
    }
    
    while (total_read < count) {
        if (file->current_cluster >= 0xFF8) {
            break;
        }
        
        u32 data_lba = fs->data_start_lba + ((file->current_cluster - 2) * fs->boot.sectors_per_cluster);
        u32 offset_in_cluster = file->current_offset % (fs->boot.sectors_per_cluster * SECTOR_SIZE);
        
        u8 sector[SECTOR_SIZE];
        u32 sector_index = offset_in_cluster / SECTOR_SIZE;
        u32 sector_offset = offset_in_cluster % SECTOR_SIZE;
        
        if (block_read(data_lba + sector_index, sector, 1) != 0) {
            break;
        }
        
        size_t to_read = SECTOR_SIZE - sector_offset;
        if (to_read > count - total_read) {
            to_read = count - total_read;
        }
        
        memcpy(out + total_read, sector + sector_offset, to_read);
        
        file->current_offset += to_read;
        total_read += to_read;
        
        if (offset_in_cluster + to_read >= fs->boot.sectors_per_cluster * SECTOR_SIZE) {
            file->current_cluster = fat12_get_fat_entry(fs, file->current_cluster);
        }
    }
    
    *read = total_read;
    return 0;
}

int fat12_list_dir(fat12_fs_t* fs, fat12_dir_entry_t* entries, size_t max_entries, size_t* count) {
    u32 root_dir_sectors = ((fs->boot.root_entries * 32) + (SECTOR_SIZE - 1)) / SECTOR_SIZE;
    *count = 0;
    
    for (u32 i = 0; i < root_dir_sectors && *count < max_entries; i++) {
        u8 sector[SECTOR_SIZE];
        if (block_read(fs->root_dir_start_lba + i, sector, 1) != 0) {
            return -1;
        }
        
        fat12_dir_entry_t* sector_entries = (fat12_dir_entry_t*)sector;
        
        for (int j = 0; j < FAT12_FILES_PER_SECTOR && *count < max_entries; j++) {
            if (sector_entries[j].filename[0] == 0) {
                return 0;
            }
            
            if (sector_entries[j].filename[0] == 0xE5) {
                continue;
            }
            
            if ((sector_entries[j].attributes & 0x08) || (sector_entries[j].attributes & 0x10)) {
                continue;
            }
            
            entries[*count] = sector_entries[j];
            (*count)++;
        }
    }
    
    return 0;
}
