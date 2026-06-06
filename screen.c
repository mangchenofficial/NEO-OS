#include "screen.h"

static volatile unsigned short* const VGA = (volatile unsigned short*)0xB8000;
static const int WIDTH = 80;
static const int HEIGHT = 25;
static int cursor_x = 0;
static int cursor_y = 0;

static inline unsigned char inportb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outportb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void screen_init() {
    cursor_x = 0;
    cursor_y = 0;
    screen_clear();
}

void screen_clear() {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        VGA[i] = 0x0720;
    }
    cursor_x = 0;
    cursor_y = 0;
    screen_update_cursor(0, 0);
}

void screen_update_cursor(int x, int y) {
    cursor_x = x;
    cursor_y = y;
    unsigned short pos = y * WIDTH + x;
    outportb(0x3D4, 0x0E);
    outportb(0x3D5, (pos >> 8) & 0xFF);
    outportb(0x3D4, 0x0F);
    outportb(0x3D5, pos & 0xFF);
}

void screen_scroll() {
    for (int y = 1; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            VGA[(y-1) * WIDTH + x] = VGA[y * WIDTH + x];
        }
    }
    for (int x = 0; x < WIDTH; x++) {
        VGA[(HEIGHT-1) * WIDTH + x] = 0x0720;
    }
}

void screen_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= HEIGHT) {
            screen_scroll();
            cursor_y = HEIGHT - 1;
        }
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA[cursor_y * WIDTH + cursor_x] = 0x0720;
        }
    } else {
        VGA[cursor_y * WIDTH + cursor_x] = (0x07 << 8) | c;
        cursor_x++;
        if (cursor_x >= WIDTH) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= HEIGHT) {
                screen_scroll();
                cursor_y = HEIGHT - 1;
            }
        }
    }
    screen_update_cursor(cursor_x, cursor_y);
}

void screen_puts(const char* str) {
    while (*str) {
        screen_putchar(*str++);
    }
}
