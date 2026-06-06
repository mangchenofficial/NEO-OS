#include "syscall.h"
#include "screen.h"
#include "task.h"

u32 syscall_handler(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3) {
    switch(syscall_num) {
        case SYS_EXIT:
            screen_puts("User program exited!\n");
            task_exit();
            return 0;
        case SYS_WRITE: {
            const char* buf = (const char*)arg1;
            for(u32 i = 0; i < arg2; i++) {
                screen_putchar(buf[i]);
            }
            return arg2;
        }
        default:
            screen_puts("Unknown syscall!\n");
            return -1;
    }
}

void syscall_init() {
    screen_puts("Syscall initialized!\n");
}
