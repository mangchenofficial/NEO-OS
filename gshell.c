#include "gshell.h"
#include "video.h"
#include "string.h"
#include "shell.h"
#include "window.h"
#include "irq.h"
#include "memory.h"

extern void desktop_request_redraw(void);

#define TITLE_BAR_HEIGHT 12

static u8 gshell_called = 0;

void gshell_init(gshell_t *sh, window_t *win) {
    if (!sh || !win) return;
    sh->win = win;
    sh->input_len = 0;
    sh->cursor_blink = 0;
    sh->output_count = 0;
    sh->display_offset = 0;
    sh->focused = 1;
    memset(sh->input_buf, 0, MAX_INPUT_LEN);
    memset(sh->output_lines, 0, sizeof(sh->output_lines));
}

static void draw_string_wrapped(u32 x, u32 y, u32 max_width, const char* str, u32 color) {
    u32 cx = x;
    u32 cy = y;
    while (*str) {
        if (cx + CHAR_W > x + max_width) {
            cx = x;
            cy += CHAR_H;
        }
        draw_char(cx, cy, *str, color);
        cx += CHAR_W;
        str++;
    }
}

void gshell_draw(gshell_t *sh) {
    if (!sh || !sh->win) return;
    
    window_t *w = sh->win;
    
    int content_x = (int)w->x + 2;
    int content_y = (int)w->y + TITLE_BAR_HEIGHT + 2;
    int content_w = (int)w->w - 4;
    int content_h = (int)w->h - TITLE_BAR_HEIGHT - 4;
    
    if (content_h <= 0 || content_w <= 0) return;

    fill_rect((u32)content_x, (u32)content_y, (u32)content_w, (u32)content_h, 0x00);

    if (gshell_called) {
        fill_rect((u32)content_x, (u32)content_y, 8, 8, 0x0F);
    }

    int max_visible = content_h / CHAR_H;
    if (max_visible < 2) return;

    for (int i = 0; i < max_visible - 1; i++) {
        int line_index = sh->display_offset + i;
        if (line_index >= sh->output_count) break;
        if (sh->output_lines[line_index][0] != '\0') {
            draw_string_wrapped((u32)content_x + 2, (u32)(content_y + i * CHAR_H), (u32)(content_w - 4), sh->output_lines[line_index], 0x0F);
        }
    }

    int input_y = content_y + (max_visible - 1) * CHAR_H;
    char display_line[OUTPUT_LINE_LEN];
    int prompt_len = strlen(PROMPT);
    for (int i = 0; i < prompt_len && i < OUTPUT_LINE_LEN; i++) {
        display_line[i] = PROMPT[i];
    }
    for (int i = 0; i < sh->input_len && i + prompt_len < OUTPUT_LINE_LEN - 1; i++) {
        display_line[prompt_len + i] = sh->input_buf[i];
    }
    display_line[prompt_len + sh->input_len] = '\0';
    draw_string_wrapped((u32)content_x + 2, (u32)input_y, (u32)(content_w - 4), display_line, 0x0F);

    uint32_t ticks = irq_get_ticks();
    if ((ticks % 30) < 15 && sh->focused) {
        int cursor_x = content_x + 2 + (prompt_len + sh->input_len) * CHAR_W;
        if (cursor_x > content_x + content_w - 2) {
            cursor_x = content_x + 2;
        }
        fill_rect((u32)cursor_x, (u32)input_y, 2, CHAR_H, 0x0F);
    }
}

void gshell_input(gshell_t *sh, char c) {
    if (!sh) return;
    
    gshell_called = 1;
    
    if (c == '\b') {
        if (sh->input_len > 0) {
            sh->input_len--;
            sh->input_buf[sh->input_len] = '\0';
        }
    } else if (c == '\n' || c == '\r') {
        if (sh->input_len > 0 && sh->input_buf[0] != '\0') {
            sh->input_buf[sh->input_len] = '\0';
            
            // 检测 clear 命令
            int is_clear = 0;
            const char* cmd = sh->input_buf;
            while (*cmd == ' ') cmd++;
            if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r' && (cmd[5] == '\0' || cmd[5] == ' ')) {
                is_clear = 1;
            }
            
            if (is_clear) {
                sh->output_count = 0;
                sh->display_offset = 0;
                sh->input_len = 0;
                desktop_request_redraw();
                return;
            }
            
            if (sh->output_count < MAX_OUTPUT_LINES) {
                sh->output_lines[sh->output_count][0] = '>';
                sh->output_lines[sh->output_count][1] = ' ';
                for (int j = 0; j < sh->input_len && j < OUTPUT_LINE_LEN - 3; j++) {
                    sh->output_lines[sh->output_count][2 + j] = sh->input_buf[j];
                }
                sh->output_lines[sh->output_count][2 + sh->input_len] = '\0';
                sh->output_count++;
            }

            char result[OUTPUT_LINE_LEN];
            shell_execute(sh->input_buf, result, OUTPUT_LINE_LEN);
            
            if (result[0] != '\0' && sh->output_count < MAX_OUTPUT_LINES) {
                int j = 0;
                for (; result[j] != '\0' && j < OUTPUT_LINE_LEN - 1; j++) {
                    sh->output_lines[sh->output_count][j] = result[j];
                }
                sh->output_lines[sh->output_count][j] = '\0';
                sh->output_count++;
            }
            
            int max_visible = ((int)sh->win->h - TITLE_BAR_HEIGHT - 4) / CHAR_H;
            if (sh->output_count > max_visible - 1) {
                sh->display_offset = sh->output_count - (max_visible - 1);
            }
            sh->input_len = 0;
        }
    } else if (sh->input_len < MAX_INPUT_LEN - 1) {
        sh->input_buf[sh->input_len++] = c;
        sh->input_buf[sh->input_len] = '\0';
    }
    
    desktop_request_redraw();
}

void gshell_set_focus(gshell_t *sh, u8 f) {
    if (sh) sh->focused = f;
}
