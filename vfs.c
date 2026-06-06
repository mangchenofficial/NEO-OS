#include "vfs.h"
#include "string.h"
#include "screen.h"

// 模拟的文件系统（为了快速演示）
static char* test_filename = "README.TXT";
static char* test_content = 
    "Welcome to NEO OS!\n"
    "\n"
    "FAT12 file system support (demo mode)\n"
    "\n"
    "Commands available:\n"
    "- help: Display commands\n"
    "- clear: Clear screen\n"
    "- ls: List files\n"
    "- cat: Show file contents\n";

static int vfs_initialized = 0;

int vfs_init() {
    vfs_initialized = 1;
    return 0;  // 成功
}

static size_t file_offset = 0;

int vfs_open(const char* path) {
    if (!vfs_initialized) return -1;
    if (strcmp(path, test_filename) == 0 || strcmp(path, "readme.txt") == 0 || strcmp(path, "README.TXT") == 0) {
        file_offset = 0;  // 重置偏移
        return 0;  // 文件描述符 0
    }
    return -1;
}

int vfs_read(int fd, void* buffer, size_t count) {
    if (fd != 0 || !vfs_initialized) return -1;
    
    size_t len = strlen(test_content);
    if (file_offset >= len) return 0;
    
    size_t to_read = count;
    if (to_read > len - file_offset) to_read = len - file_offset;
    
    memcpy(buffer, test_content + file_offset, to_read);
    file_offset += to_read;
    
    return to_read;
}

void vfs_close(int fd) {
    // 不需要做什么
}

int vfs_list_dir(vfs_dirent_t* entries, size_t max_entries, size_t* count) {
    if (!vfs_initialized) return -1;
    if (max_entries < 1) return -1;
    
    strcpy(entries[0].name, test_filename);
    entries[0].size = strlen(test_content);
    *count = 1;
    
    return 0;
}
