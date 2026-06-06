#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

#define SYS_EXIT 1
#define SYS_WRITE 2
#define SYS_READ 3

void syscall_init();
u32 syscall_handler(u32 syscall_num, u32 arg1, u32 arg2, u32 arg3);

#endif
