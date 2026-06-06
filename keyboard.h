#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

void keyboard_init();
void keyboard_handle_interrupt();
void keyboard_handle_scancode(uint8_t scancode);
void keyboard_poll(void);
int keyboard_has_key();
char keyboard_pop_key();
char keyboard_getchar();
char* keyboard_readline();

#endif
