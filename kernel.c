#include "screen.h"
#include "keyboard.h"
#include "shell.h"
#include "memory.h"
#include "heap.h"
#include "idt.h"
#include "pic.h"
#include "vfs.h"
#include "ports.h"
#include "task.h"
#include "syscall.h"
#include "video.h"
#include "mouse.h"
#include "desktop.h"

#define sti() __asm__ volatile("sti")
#define cli() __asm__ volatile("cli")

void kernel_main() {
    cli();

    screen_init();
    screen_puts("内核初始化开始...\n");

    memory_init(0, 0);
    screen_puts("内存初始化完成...\n");

    heap_init();
    screen_puts("堆初始化完成...\n");

    keyboard_init();
    screen_puts("键盘初始化完成...\n");

    pic_init();
    screen_puts("PIC初始化完成...\n");

    idt_init();
    screen_puts("IDT初始化完成...\n");

    // 0xFB = 11111011: 屏蔽 PIT (0), 允许键盘 (1), 允许级联 (2)
    outb(0x21, 0xFB);
    outb(0xA1, 0xEF);
    sti();
    screen_puts("中断已启用!\n");

    if (vfs_init() == 0) {
        screen_puts("虚拟文件系统初始化完成!\n");
    } else {
        screen_puts("虚拟文件系统初始化失败!\n");
    }
    
    task_init();
    syscall_init();
    video_init(0, 0, 0, 0);
    screen_puts("视频初始化完成!\n");
    mouse_init();
    screen_puts("鼠标初始化完成!\n");
    
    desktop_init();
    screen_puts("桌面初始化完成!\n");
    
    desktop_draw();
    
    for (;;) {
        desktop_update();
    }
}
