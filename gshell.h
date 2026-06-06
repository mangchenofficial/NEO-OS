#ifndef GSHELL_H
#define GSHELL_H

#include "window.h"
#include "types.h"

#define MAX_INPUT_LEN    256
#define MAX_OUTPUT_LINES 100
#define OUTPUT_LINE_LEN  128
#define CHAR_W           8
#define CHAR_H           16
#define PROMPT           "> "

typedef struct gshell_t {
    window_t *win;
    char input_buf[MAX_INPUT_LEN];
    int  input_len;
    int  cursor_blink;
    char output_lines[MAX_OUTPUT_LINES][OUTPUT_LINE_LEN];
    int  output_count;
    int  display_offset;
    u8   focused;
} gshell_t;

void gshell_init(gshell_t *sh, window_t *win);
void gshell_draw(gshell_t *sh);
void gshell_input(gshell_t *sh, char c);
void gshell_set_focus(gshell_t *sh, u8 f);

#endif