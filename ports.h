#ifndef PORTS_H
#define PORTS_H

#include "types.h"

static inline u8 inb(u16 port) {
    u8 value;
    __asm__ volatile("inb %1, %0" : "=a" (value) : "dN" (port));
    return value;
}

static inline u16 inw(u16 port) {
    u16 value;
    __asm__ volatile("inw %1, %0" : "=a" (value) : "dN" (port));
    return value;
}

static inline void outb(u16 port, u8 value) {
    __asm__ volatile("outb %0, %1" : : "a" (value), "dN" (port));
}

static inline void outw(u16 port, u16 value) {
    __asm__ volatile("outw %0, %1" : : "a" (value), "dN" (port));
}

#endif
