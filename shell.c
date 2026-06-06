#include "screen.h"
#include "keyboard.h"
#include "memory.h"
#include "irq.h"
#include "vfs.h"
#include "string.h"
#include "elf.h"
#include "task.h"

static void print_uint(u32 num) {
    char buf[32];
    int i = 0;
    if (num == 0) {
        screen_putchar('0');
        return;
    }
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0) {
        screen_putchar(buf[--i]);
    }
}

static void print_uint_str(u32 num, char* buf, int* pos, int max_len) {
    char tmp[32];
    int i = 0;
    if (num == 0) {
        if (*pos < max_len - 1) buf[(*pos)++] = '0';
        return;
    }
    while (num > 0) {
        tmp[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0 && *pos < max_len - 1) {
        buf[(*pos)++] = tmp[--i];
    }
}

static void strcat_uint(char* buf, int* pos, int max_len, u32 num) {
    print_uint_str(num, buf, pos, max_len);
}

static void strcat_str(char* buf, int* pos, int max_len, const char* str) {
    while (*str && *pos < max_len - 1) {
        buf[(*pos)++] = *str++;
    }
}

int strncmp(const char* s1, const char* s2, int n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
        if (s1[i] == '\0') return 0;
    }
    return 0;
}

static void cmd_help() {
    screen_puts("Available commands: help, clear, echo, about, meminfo, time, ls, cat, run\n");
}

static void cmd_clear() {
    screen_clear();
}

static void cmd_about() {
    screen_puts("NEO OS v0.1.0\n");
}

static void cmd_echo(char* args) {
    while (*args == ' ') args++;
    screen_puts(args);
    screen_putchar('\n');
}

static void cmd_meminfo() {
    u32 total = memory_get_total();
    u32 free = memory_get_free_pages();
    u32 used = memory_get_used_pages();

    screen_puts("Total memory: ");
    print_uint(total / 1024 / 1024);
    screen_puts(" MB\nFree pages: ");
    print_uint(free);
    screen_puts("\nUsed pages: ");
    print_uint(used);
    screen_putchar('\n');
}

static void cmd_time() {
    u32 ticks = irq_get_ticks();
    screen_puts("Ticks: ");
    print_uint(ticks);
    screen_puts(" (");
    print_uint(ticks / 18);
    screen_puts(" sec)\n");
}

static void cmd_ls() {
    vfs_dirent_t entries[32];
    size_t count;
    
    if (vfs_list_dir(entries, 32, &count) != 0) {
        screen_puts("Failed to list directory\n");
        return;
    }
    
    for (size_t i = 0; i < count; i++) {
        screen_puts(entries[i].name);
        screen_puts("  ");
        print_uint(entries[i].size);
        screen_putchar('\n');
    }
}

static void cmd_cat(char* args) {
    while (*args == ' ') args++;
    if (*args == '\0') {
        screen_puts("Usage: cat <filename>\n");
        return;
    }
    
    int fd = vfs_open(args);
    if (fd < 0) {
        screen_puts("File not found: ");
        screen_puts(args);
        screen_putchar('\n');
        return;
    }
    
    char buffer[256];
    int read;
    size_t total_read = 0;
    while ((read = vfs_read(fd, buffer, 255)) > 0 && total_read < 4096) {
        for (int i = 0; i < read; i++) {
            if (buffer[i] == '\n') {
                screen_putchar('\n');
            } else {
                screen_putchar(buffer[i]);
            }
        }
        total_read += read;
    }
    
    vfs_close(fd);
}

static void cmd_run(char* args) {
    while (*args == ' ') args++;
    if (*args == '\0') {
        screen_puts("Usage: run <filename>\n");
        return;
    }
    
    u32 entry;
    if (elf_load(args, &entry) != 0) {
        screen_puts("Failed to load program!\n");
        return;
    }
    
    int tid = task_create(entry);
    if (tid < 0) {
        screen_puts("Failed to create task!\n");
        return;
    }
    
    task_switch_to(tid);
    
    screen_puts("Back to shell!\n");
}

void shell_init() {
    screen_puts("NEO OS v0.1.0\n");
    screen_puts("Type 'help' for commands.\n\n");
}

void shell_run() {
    while (1) {
        screen_puts("> ");
        char* line = keyboard_readline();

        while (*line == ' ') line++;
        if (*line == '\0') continue;

        if (strncmp(line, "help", 4) == 0) {
            cmd_help();
        } else if (strncmp(line, "clear", 5) == 0) {
            cmd_clear();
        } else if (strncmp(line, "about", 5) == 0) {
            cmd_about();
        } else if (strncmp(line, "echo", 4) == 0) {
            cmd_echo(line + 4);
        } else if (strncmp(line, "meminfo", 7) == 0) {
            cmd_meminfo();
        } else if (strncmp(line, "time", 4) == 0) {
            cmd_time();
        } else if (strncmp(line, "ls", 2) == 0) {
            cmd_ls();
        } else if (strncmp(line, "cat", 3) == 0) {
            cmd_cat(line + 3);
        } else if (strncmp(line, "run", 3) == 0) {
            cmd_run(line + 3);
        } else {
            screen_puts("Unknown: ");
            screen_puts(line);
            screen_putchar('\n');
        }
    }
}

void shell_execute(char* cmd, char* output, int max_len) {
    output[0] = '\0';
    int pos = 0;

    while (*cmd == ' ') cmd++;
    if (*cmd == '\0') return;

    if (strncmp(cmd, "help", 4) == 0) {
        strcat_str(output, &pos, max_len, "help, clear, echo, about, meminfo, time, ls, cat, run");
    } else if (strncmp(cmd, "clear", 5) == 0) {
    } else if (strncmp(cmd, "about", 5) == 0) {
        strcat_str(output, &pos, max_len, "NEO OS v0.1.0");
    } else if (strncmp(cmd, "echo", 4) == 0) {
        strcat_str(output, &pos, max_len, cmd + 4);
    } else if (strncmp(cmd, "meminfo", 7) == 0) {
        u32 total = memory_get_total();
        u32 free = memory_get_free_pages();
        u32 used = memory_get_used_pages();
        strcat_str(output, &pos, max_len, "Total: ");
        strcat_uint(output, &pos, max_len, total / 1024 / 1024);
        strcat_str(output, &pos, max_len, " MB, Free: ");
        strcat_uint(output, &pos, max_len, free);
        strcat_str(output, &pos, max_len, " pages, Used: ");
        strcat_uint(output, &pos, max_len, used);
        strcat_str(output, &pos, max_len, " pages");
    } else if (strncmp(cmd, "time", 4) == 0) {
        u32 ticks = irq_get_ticks();
        strcat_str(output, &pos, max_len, "Ticks: ");
        strcat_uint(output, &pos, max_len, ticks);
        strcat_str(output, &pos, max_len, " (");
        strcat_uint(output, &pos, max_len, ticks / 18);
        strcat_str(output, &pos, max_len, " sec)");
    } else if (strncmp(cmd, "ls", 2) == 0) {
        vfs_dirent_t entries[32];
        size_t count;
        if (vfs_list_dir(entries, 32, &count) == 0) {
            for (size_t i = 0; i < count; i++) {
                strcat_str(output, &pos, max_len, entries[i].name);
                strcat_str(output, &pos, max_len, " ");
                strcat_uint(output, &pos, max_len, entries[i].size);
                if (i < count - 1) strcat_str(output, &pos, max_len, ", ");
            }
        } else {
            strcat_str(output, &pos, max_len, "Failed to list directory");
        }
    } else if (strncmp(cmd, "cat", 3) == 0) {
        char* args = cmd + 3;
        while (*args == ' ') args++;
        if (*args == '\0') {
            strcat_str(output, &pos, max_len, "Usage: cat <filename>");
        } else {
            int fd = vfs_open(args);
            if (fd < 0) {
                strcat_str(output, &pos, max_len, "File not found: ");
                strcat_str(output, &pos, max_len, args);
            } else {
                char buffer[256];
                int read;
                size_t total_read = 0;
                while ((read = vfs_read(fd, buffer, 255)) > 0 && total_read < 1024 && pos < max_len - 1) {
                    for (int i = 0; i < read && pos < max_len - 1; i++) {
                        if (buffer[i] != '\n') {
                            output[pos++] = buffer[i];
                        }
                    }
                    total_read += read;
                }
                vfs_close(fd);
            }
        }
    } else if (strncmp(cmd, "run", 3) == 0) {
        strcat_str(output, &pos, max_len, "Run command not available in GUI terminal");
    } else {
        strcat_str(output, &pos, max_len, "Unknown command: ");
        strcat_str(output, &pos, max_len, cmd);
    }

    output[pos] = '\0';
}
