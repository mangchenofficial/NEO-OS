#ifndef FAT12_H
#define FAT12_H

#include "types.h"

#define FAT12_MAX_FILENAME 12
#define FAT12_FILES_PER_SECTOR 16

typedef struct {
    u8  jump[3];
    char oem[8];
    u16 bytes_per_sector;
    u8  sectors_per_cluster;
    u16 reserved_sectors;
    u8  fat_count;
    u16 root_entries;
    u16 total_sectors;
    u8  media_type;
    u16 sectors_per_fat;
    u16 sectors_per_track;
    u16 head_count;
    u32 hidden_sectors;
    u32 large_sectors;
    u8  drive_number;
    u8  reserved1;
    u8  boot_signature;
    u32 volume_id;
    char volume_label[11];
    char fs_type[8];
} __attribute__((packed)) fat12_boot_sector_t;

typedef struct {
    char filename[8];
    char ext[3];
    u8  attributes;
    u8  reserved;
    u8  create_time_tenths;
    u16 create_time;
    u16 create_date;
    u16 last_access_date;
    u16 first_cluster_high;
    u16 last_modify_time;
    u16 last_modify_date;
    u16 first_cluster_low;
    u32 file_size;
} __attribute__((packed)) fat12_dir_entry_t;

typedef struct {
    fat12_boot_sector_t boot;
    u32 fat_start_lba;
    u32 root_dir_start_lba;
    u32 data_start_lba;
} fat12_fs_t;

typedef struct {
    fat12_dir_entry_t entry;
    u32 current_cluster;
    u32 current_offset;
} fat12_file_t;

int fat12_init(fat12_fs_t* fs);
int fat12_open(fat12_fs_t* fs, const char* name, fat12_file_t* file);
int fat12_read(fat12_fs_t* fs, fat12_file_t* file, void* buffer, size_t count, size_t* read);
int fat12_list_dir(fat12_fs_t* fs, fat12_dir_entry_t* entries, size_t max_entries, size_t* count);

#endif
