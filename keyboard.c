#include "keyboard.h"
#include "screen.h"
#include "gshell.h"
#include "desktop.h"

extern gshell_t *global_terminal;

static unsigned char shift_pressed = 0;

static unsigned char scancode_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};

static unsigned char scancode_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' '
};

#define KEYBUF_SIZE 256
static char keybuf[KEYBUF_SIZE];
static volatile int keybuf_in = 0;
static volatile int keybuf_out = 0;

volatile int keypress_count = 0;

static inline unsigned char inportb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void keyboard_init() {
    shift_pressed = 0;
    keybuf_in = 0;
    keybuf_out = 0;
    keypress_count = 0;
}

void keyboard_handle_interrupt() {
    unsigned char scancode = inportb(0x60);
    
    keypress_count++;
    key_test_set('K');
    
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        return;
    }
    
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    
    if (scancode >= 128) {
        return;
    }
    
    char c = shift_pressed ? scancode_shift[scancode] : scancode_map[scancode];
    
    if (c == 0) {
        return;
    }
    
    key_test_set(c);
    
    if (global_terminal) {
        gshell_input(global_terminal, c);
    } else {
        int next_in = (keybuf_in + 1) % KEYBUF_SIZE;
        if (next_in != keybuf_out) {
            keybuf[keybuf_in] = c;
            keybuf_in = next_in;
        }
    }
}

void keyboard_handle_scancode(uint8_t scancode) {
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        return;
    }
    
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    
    if (scancode >= 128) {
        return;
    }
    
    char c = shift_pressed ? scancode_shift[scancode] : scancode_map[scancode];
    
    if (c == 0) {
        return;
    }
    
    keypress_count++;
    key_test_set(c);
    
    if (global_terminal) {
        gshell_input(global_terminal, c);
    } else {
        int next_in = (keybuf_in + 1) % KEYBUF_SIZE;
        if (next_in != keybuf_out) {
            keybuf[keybuf_in] = c;
            keybuf_in = next_in;
        }
    }
}

void keyboard_poll(void) {
    int max_iterations = 10;
    int iterations = 0;
    
    while (iterations < max_iterations) {
        u8 status = inportb(0x64);
        
        if (!(status & 1)) break;
        
        if (status & 0x20) {
            break;
        }
        
        unsigned char scancode = inportb(0x60);
        keyboard_handle_scancode(scancode);
        
        iterations++;
    }
}

int keyboard_has_key() {
    return keybuf_in != keybuf_out;
}

char keyboard_pop_key() {
    if (keybuf_in == keybuf_out) {
        return 0;
    }
    char c = keybuf[keybuf_out];
    keybuf_out = (keybuf_out + 1) % KEYBUF_SIZE;
    return c;
}

char keyboard_getchar() {
    if (!keyboard_has_key()) {
        while (1) {
            if (inportb(0x64) & 1) {
                unsigned char scancode = inportb(0x60);
                keyboard_handle_scancode(scancode);
                if (keyboard_has_key()) {
                    break;
                }
            }
        }
    }
    return keyboard_pop_key();
}

char buffer[256];

char* keyboard_readline() {
    int pos = 0;
    buffer[0] = '\0';

    while (1) {
        char c = keyboard_getchar();

        if (c == '\n' || c == '\r') {
            screen_putchar('\n');
            buffer[pos] = '\0';
            return buffer;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                screen_putchar('\b');
            }
        } else if (pos < 254) {
            buffer[pos++] = c;
            screen_putchar(c);
        }
    }
}
