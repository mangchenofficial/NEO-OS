#ifndef VFS_H
#define VFS_H

#include "types.h"
#include "fat12.h"

#define VFS_MAX_NAME 32
#define VFS_MAX_FILES 16

typedef struct {
    char name[VFS_MAX_NAME];
    u32 size;
} vfs_dirent_t;

typedef struct {
    int in_use;
    fat12_file_t fat_file;
} vfs_file_t;

int vfs_init();
int vfs_open(const char* path);
int vfs_read(int fd, void* buffer, size_t count);
void vfs_close(int fd);
int vfs_list_dir(vfs_dirent_t* entries, size_t max_entries, size_t* count);

#endif
