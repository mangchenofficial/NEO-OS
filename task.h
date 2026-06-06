#ifndef TASK_H
#define TASK_H

#include "types.h"

#define MAX_TASKS 8

typedef struct {
    u32* page_dir;
    u32 eip;
    u32 esp;
    u32* user_stack;
    int in_use;
} task_t;

void task_init();
int task_create(u32 entry);
void task_exit();
void task_switch_to(int tid);

#endif
