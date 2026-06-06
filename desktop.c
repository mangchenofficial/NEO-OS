#include "desktop.h"
#include "video.h"
#include "window.h"
#include "mouse.h"
#include "string.h"
#include "gshell.h"
#include "heap.h"
#include "irq.h"
#include "ports.h"

#define DESKTOP_BG_COLOR 0x03
#define ICON_SIZE 32

typedef struct {
    u32 x;
    u32 y;
    char name[32];
} desktop_icon_t;

static desktop_icon_t icons[4];
static int icon_count = 4;
gshell_t *global_terminal = 0;
static mouse_state_t last_mouse_state;
static u8 last_buttons = 0;
static u8 cursor_drawn = 0;
static u8 cursor_backup[15 * 15];
static u8 need_redraw = 1;
static u8 click_handled = 0;
static volatile u8 pending_redraw = 0;
static window_t* drag_window = 0;
static u32 drag_old_x = 0;
static u32 drag_old_y = 0;
static u32 drag_old_w = 0;
static u32 drag_old_h = 0;
static u8* drag_backup = 0;
static uint32_t last_redraw_tick = 0;

static char last_key_char = ' ';
static u8 key_pressed = 0;
static int redraw_count = 0;

void key_test_set(char c) {
    last_key_char = c;
    key_pressed = 1;
    pending_redraw = 1;
}

void desktop_init() {
    video_set_double_buffer(1);
    
    icons[0].x = 20;
    icons[0].y = 30;
    string_copy("Terminal", icons[0].name);
    
    icons[1].x = 70;
    icons[1].y = 30;
    string_copy("APP2", icons[1].name);
    
    icons[2].x = 20;
    icons[2].y = 90;
    string_copy("APP3", icons[2].name);
    
    icons[3].x = 70;
    icons[3].y = 90;
    string_copy("APP4", icons[3].name);
    
    mouse_state_t* ms = mouse_get_state();
    last_mouse_state = *ms;
    last_buttons = 0;
    
    window_manager_init();
}

static void draw_desktop_icons() {
    for (int i = 0; i < icon_count; i++) {
        fill_rect(icons[i].x, icons[i].y, ICON_SIZE, ICON_SIZE, 0x08);
        fill_rect(icons[i].x + 1, icons[i].y + 1, ICON_SIZE - 2, ICON_SIZE - 2, 0x01);
        draw_string(icons[i].x + 2, icons[i].y + ICON_SIZE + 2, icons[i].name, 0x0F);
    }
}

static void save_cursor_backup(u32 x, u32 y) {
    for (u32 yy = 0; yy < 15; yy++) {
        for (u32 xx = 0; xx < 15; xx++) {
            cursor_backup[yy * 15 + xx] = getpixel(x + xx, y + yy);
        }
    }
}

static void restore_cursor_backup(u32 x, u32 y) {
    for (u32 yy = 0; yy < 15; yy++) {
        for (u32 xx = 0; xx < 15; xx++) {
            putpixel(x + xx, y + yy, cursor_backup[yy * 15 + xx]);
        }
    }
}

static u8 drag_window_backup[400 * 200];

static void save_window_backup(u32 x, u32 y, u32 w, u32 h) {
    u32 index = 0;
    for (u32 yy = 0; yy < h; yy++) {
        for (u32 xx = 0; xx < w; xx++) {
            if (index < sizeof(drag_window_backup)) {
                drag_window_backup[index] = getpixel(x + xx, y + yy);
            }
            index++;
        }
    }
}

static void restore_window_backup(u32 x, u32 y, u32 w, u32 h) {
    u32 index = 0;
    for (u32 yy = 0; yy < h; yy++) {
        for (u32 xx = 0; xx < w; xx++) {
            if (index < sizeof(drag_window_backup)) {
                putpixel(x + xx, y + yy, drag_window_backup[index]);
            }
            index++;
        }
    }
}

static void draw_mouse_cursor() {
    mouse_state_t* ms = mouse_get_state();
    u32 mx = ms->x;
    u32 my = ms->y;
    
    if (cursor_drawn) {
        restore_cursor_backup(last_mouse_state.x, last_mouse_state.y);
    }
    
    save_cursor_backup(mx, my);
    
    putpixel(mx, my, 0x0F);
    putpixel(mx + 1, my, 0x0F);
    putpixel(mx + 2, my, 0x0F);
    putpixel(mx + 3, my, 0x0F);
    putpixel(mx + 4, my, 0x0F);
    putpixel(mx, my + 1, 0x0F);
    putpixel(mx + 1, my + 1, 0x0F);
    putpixel(mx, my + 2, 0x0F);
    putpixel(mx + 1, my + 2, 0x0F);
    putpixel(mx, my + 3, 0x0F);
    putpixel(mx + 2, my + 3, 0x0F);
    putpixel(mx, my + 4, 0x0F);
    putpixel(mx + 3, my + 4, 0x0F);
    putpixel(mx, my + 5, 0x0F);
    putpixel(mx + 1, my + 5, 0x0F);
    putpixel(mx, my + 6, 0x0F);
    
    last_mouse_state = *ms;
    cursor_drawn = 1;
}

static int click_icon_at(u32 mx, u32 my) {
    for (int i = 0; i < icon_count; i++) {
        if (mx >= icons[i].x && mx < icons[i].x + ICON_SIZE &&
            my >= icons[i].y && my < icons[i].y + ICON_SIZE + 12) {
            return i;
        }
    }
    return -1;
}

extern volatile int keypress_count;

extern void keyboard_poll(void);

static void draw_number(u32 x, u32 y, int num, u8 color) {
    char digits[12];
    int count = 0;
    
    if (num == 0) {
        digits[0] = '0';
        count = 1;
    } else {
        while (num > 0 && count < 11) {
            digits[count++] = '0' + (num % 10);
            num /= 10;
        }
    }
    
    int dx = 0;
    for (int i = count - 1; i >= 0; i--) {
        draw_char(x + dx, y, digits[i], color);
        dx += 8;
    }
}

void desktop_draw() {
    redraw_count++;
    
    fill_rect(0, 0, VBE_WIDTH, VBE_HEIGHT, DESKTOP_BG_COLOR);
    
    fill_rect(0, 0, VBE_WIDTH, 20, 0x01);
    draw_string(5, 5, "Cnt:", 0x0F);
    draw_number(30, 5, keypress_count, 0x0F);
    draw_string(70, 5, "RD:", 0x0F);
    draw_number(90, 5, redraw_count, 0x0F);
    
    if (key_pressed) {
        draw_char(150, 5, last_key_char, 0x0F);
    }
    
    if (global_terminal) {
        draw_string(180, 5, "TERM", 0x0E);
    }
    
    draw_desktop_icons();
    
    window_draw_all();
    
    draw_mouse_cursor();
    
    swap_buffers();
}

void desktop_update() {
    keyboard_poll();
    
    mouse_poll();
    
    mouse_state_t* ms = mouse_get_state();
    u8 current_buttons = ms->buttons;
    int mouse_moved = (ms->x != last_mouse_state.x) || (ms->y != last_mouse_state.y);
    
    if ((current_buttons & 1) && !(last_buttons & 1) && !click_handled) {
        u32 mx = ms->x;
        u32 my = ms->y;
        
        if (my > 20) {
            window_t* clicked_win = window_find_at(mx, my);
            if (clicked_win) {
                int hit_close = window_hit_close_button(clicked_win, mx, my);
                int in_titlebar = window_in_titlebar(clicked_win, mx, my);
                
                if (hit_close) {
                    window_close(clicked_win);
                    need_redraw = 1;
                } else {
                    window_bring_to_front(clicked_win);
                    
                    if (in_titlebar) {
                        window_t* top_win = window_find_at(mx, my);
                        if (top_win) {
                            drag_window = top_win;
                            window_start_drag(top_win, mx, my);
                            drag_old_x = top_win->x;
                            drag_old_y = top_win->y;
                            drag_old_w = top_win->w;
                            drag_old_h = top_win->h;
                        }
                        cursor_drawn = 0;
                        need_redraw = 0;
                    } else {
                        need_redraw = 1;
                    }
                }
                click_handled = 1;
            } else {
                int icon_idx = click_icon_at(mx, my);
                if (icon_idx >= 0) {
                    if (strcmp(icons[icon_idx].name, "Terminal") == 0) {
                        window_t *win = window_create(120, 40, 240, 160, "Terminal");
                        if (win) {
                            static gshell_t term;
                            gshell_init(&term, win);
                            win->user_data = &term;
                            global_terminal = &term;
                            need_redraw = 1;
                        }
                    } else {
                        char title[32];
                        string_copy("窗口-", title);
                        string_append(title, icons[icon_idx].name);
                        
                        u32 win_x = 120 + (icon_idx * 20) % 150;
                        u32 win_y = 40 + (icon_idx * 15) % 100;
                        window_create(win_x, win_y, 140, 100, title);
                        need_redraw = 1;
                    }
                    click_handled = 1;
                }
            }
        }
    }
    
    if (!(current_buttons & 1)) {
        click_handled = 0;
    }
    
    if (!(current_buttons & 1) && (last_buttons & 1)) {
        if (drag_window) {
            window_stop_drag(drag_window);
            drag_window = 0;
            need_redraw = 1;
        }
    }
    
    if (drag_window && mouse_moved) {
        window_drag_update(drag_window, ms->x, ms->y);
        
        drag_old_x = drag_window->x;
        drag_old_y = drag_window->y;
        
        cursor_drawn = 0;
        desktop_draw();
        draw_mouse_cursor();
        swap_buffers();
        
        mouse_moved = 0;
    }
    
    last_buttons = current_buttons;
    
    if (pending_redraw) {
        pending_redraw = 0;
        cursor_drawn = 0;
        desktop_draw();
    } else if (need_redraw) {
        need_redraw = 0;
        cursor_drawn = 0;
        desktop_draw();
    } else if (mouse_moved && !drag_window) {
        draw_mouse_cursor();
        swap_buffers();
    }
}

void desktop_request_redraw(void) {
    pending_redraw = 1;
}
