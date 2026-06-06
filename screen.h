#ifndef SCREEN_H
#define SCREEN_H

void screen_init();
void screen_clear();
void screen_putchar(char c);
void screen_puts(const char* str);
void screen_scroll();
void screen_update_cursor(int x, int y);

#endif
