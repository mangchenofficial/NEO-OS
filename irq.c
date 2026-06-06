#include "irq.h"
#include "screen.h"
#include "keyboard.h"
#include "pic.h"
#include "syscall.h"
#include "types.h"
#include "mouse.h"

typedef struct {
    uint32_t ds, es, fs, gs;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;

static volatile uint32_t tick = 0;

static const char *exception_messages[] = {
    "除零错误",
    "调试",
    "不可屏蔽中断",
    "断点",
    "溢出检测",
    "越界",
    "无效操作码",
    "无协处理器",
    "双重故障",
    "协处理器段越界",
    "坏的TSS",
    "段不存在",
    "栈故障",
    "一般保护错误",
    "页错误",
    "未知中断",
    "协处理器故障",
    "对齐检查",
    "机器检查",
    "SIMD浮点异常"
};

void isr_handler(registers_t regs) {
    if (regs.int_no == 128) {
        u32 result = syscall_handler(regs.eax, regs.ebx, regs.ecx, regs.edx);
        regs.eax = result;
    } else {
        screen_puts("异常: ");
        screen_puts(exception_messages[regs.int_no < 20 ? regs.int_no : 19]);
        screen_puts("\n");
        for (;;);
    }
}

void irq_handler(registers_t regs) {
    if (regs.int_no == 0x20) {
        tick++;
    } else if (regs.int_no == 0x21) {
        keyboard_handle_interrupt();
    } else if (regs.int_no == 0x2C) {
        mouse_handle_interrupt();
    }

    pic_send_eoi(regs.int_no - 0x20);
}

uint32_t irq_get_ticks(void) {
    return tick;
}
