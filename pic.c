#include "pic.h"
#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

void pic_init(void) {
    // 先完全禁用所有IRQ，避免初始化期间有中断
    outb(PIC1_DATA, 0xFF);
    io_wait();
    outb(PIC2_DATA, 0xFF);
    io_wait();

    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    outb(PIC1_DATA, 0xFF);  // 保持禁用所有，稍后按需启用
    io_wait();
    outb(PIC2_DATA, 0xFF);
    io_wait();
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }
    outb(PIC1_COMMAND, 0x20);
}
