#include "task.h"
#include "memory.h"
#include "screen.h"
#include "syscall.h"
#include "string.h"

static task_t tasks[MAX_TASKS];
static int current_task = -1;

void task_init() {
    for(int i = 0; i < MAX_TASKS; i++) {
        tasks[i].in_use = 0;
    }
    screen_puts("Task manager initialized!\n");
}

int task_create(u32 entry) {
    for(int i = 0; i < MAX_TASKS; i++) {
        if(!tasks[i].in_use) {
            tasks[i].in_use = 1;
            tasks[i].eip = entry;
            screen_puts("Created task: ");
            screen_putchar('0' + i);
            screen_putchar('\n');
            
            tasks[i].user_stack = (u32*)alloc_page();
            if(!tasks[i].user_stack) {
                screen_puts("Failed to allocate user stack!\n");
                tasks[i].in_use = 0;
                return -1;
            }
            tasks[i].esp = (u32)tasks[i].user_stack + 4096;
            tasks[i].page_dir = 0;
            current_task = i;
            return i;
        }
    }
    return -1;
}

void task_exit() {
    if(current_task >= 0) {
        if(tasks[current_task].user_stack) {
            free_page(tasks[current_task].user_stack);
        }
        tasks[current_task].in_use = 0;
        current_task = -1;
        screen_puts("Task exited!\n");
    }
}

void task_switch_to(int tid) {
    if(tid < 0 || tid >= MAX_TASKS || !tasks[tid].in_use) {
        return;
    }
    current_task = tid;
    
    screen_puts("Switching to user task...\n");
    
    screen_puts("Hello from user mode!\n");
    
    task_exit();
}
