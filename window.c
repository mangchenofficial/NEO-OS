#include "window.h"
#include "video.h"
#include "gshell.h"
#include "string.h"

static window_t windows[MAX_WINDOWS];
static int window_indices[MAX_WINDOWS];
static int window_count = 0;
static u32 next_z_order = 0;

extern gshell_t *global_terminal;

void window_manager_init() {
    window_count = 0;
    next_z_order = 0;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i].visible = 0;
        windows[i].active = 0;
        windows[i].z_order = 0;
        windows[i].dragging = 0;
        windows[i].drag_offset_x = 0;
        windows[i].drag_offset_y = 0;
        windows[i].user_data = 0;
        window_indices[i] = i;
    }
}

window_t* window_create(u32 x, u32 y, u32 w, u32 h, const char* title) {
    if (window_count >= MAX_WINDOWS) return 0;
    
    int idx = window_count;
    window_t* win = &windows[idx];
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    win->active = 1;
    win->visible = 1;
    win->z_order = next_z_order++;
    win->dragging = 0;
    win->drag_offset_x = 0;
    win->drag_offset_y = 0;
    win->user_data = 0;
    
    int i = 0;
    while (title[i] && i < 63) {
        win->title[i] = title[i];
        i++;
    }
    win->title[i] = '\0';
    
    window_indices[window_count] = idx;
    window_count++;
    
    return win;
}

int window_in_titlebar(window_t* win, u32 mx, u32 my) {
    if (!win || !win->visible) return 0;
    u32 title_h = 12;
    return (mx >= win->x && mx < win->x + win->w - 16 &&
            my >= win->y && my < win->y + title_h);
}

void window_start_drag(window_t* win, u32 mx, u32 my) {
    if (!win) return;
    win->dragging = 1;
    win->drag_offset_x = (s32)mx - (s32)win->x;
    win->drag_offset_y = (s32)my - (s32)win->y;
}

void window_drag_update(window_t* win, u32 mx, u32 my) {
    if (!win || !win->dragging) return;
    s32 new_x = (s32)mx - win->drag_offset_x;
    s32 new_y = (s32)my - win->drag_offset_y;
    
    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;
    if (new_x + (s32)win->w > (s32)VBE_WIDTH) new_x = (s32)VBE_WIDTH - (s32)win->w;
    if (new_y + (s32)win->h > (s32)VBE_HEIGHT) new_y = (s32)VBE_HEIGHT - (s32)win->h;
    
    win->x = (u32)new_x;
    win->y = (u32)new_y;
}

void window_stop_drag(window_t* win) {
    if (!win) return;
    win->dragging = 0;
}

void window_draw(window_t* win) {
    if (!win || !win->visible) return;
    
    u32 title_h = 12;
    u32 border_color = win->active ? 0x0F : 0x08;
    u32 title_color = win->active ? 0x01 : 0x08;
    u32 body_color = 0x07;
    
    fill_rect(win->x, win->y, win->w, title_h, title_color);
    
    fill_rect(win->x, win->y + title_h, win->w, win->h - title_h, body_color);
    
    fill_rect(win->x, win->y, 1, win->h, border_color);
    fill_rect(win->x + win->w - 1, win->y, 1, win->h, border_color);
    fill_rect(win->x, win->y, win->w, 1, border_color);
    fill_rect(win->x, win->y + win->h - 1, win->w, 1, border_color);
    
    draw_string(win->x + 2, win->y + 2, win->title, 0x0F);
    
    window_draw_close_button(win);
    
    if (win->user_data) {
        gshell_draw((gshell_t*)win->user_data);
    }
}

void window_draw_all() {
    for (int i = 0; i < window_count; i++) {
        int idx = window_indices[i];
        if (windows[idx].visible) {
            window_draw(&windows[idx]);
        }
    }
}

int window_contains(window_t* win, u32 mx, u32 my) {
    if (!win || !win->visible) return 0;
    return (mx >= win->x && mx < win->x + win->w &&
            my >= win->y && my < win->y + win->h);
}

window_t* window_find_at(u32 mx, u32 my) {
    for (int i = window_count - 1; i >= 0; i--) {
        int idx = window_indices[i];
        if (windows[idx].visible && window_contains(&windows[idx], mx, my)) {
            return &windows[idx];
        }
    }
    return 0;
}

void window_bring_to_front(window_t* win) {
    if (!win) return;
    
    int win_idx = -1;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (&windows[i] == win) {
            win_idx = i;
            break;
        }
    }
    
    if (win_idx == -1) return;
    
    int order_idx = -1;
    for (int i = 0; i < window_count; i++) {
        if (window_indices[i] == win_idx) {
            order_idx = i;
            break;
        }
    }
    
    if (order_idx == -1) return;
    
    if (order_idx == window_count - 1) {
        for (int i = 0; i < window_count; i++) {
            int idx = window_indices[i];
            windows[idx].active = 0;
        }
        windows[win_idx].active = 1;
        if (windows[win_idx].user_data) {
            global_terminal = (gshell_t*)windows[win_idx].user_data;
            gshell_set_focus(global_terminal, 1);
        }
        return;
    }
    
    int temp = window_indices[order_idx];
    
    for (int i = order_idx; i < window_count - 1; i++) {
        window_indices[i] = window_indices[i + 1];
    }
    
    window_indices[window_count - 1] = temp;
    
    for (int i = 0; i < window_count; i++) {
        int idx = window_indices[i];
        windows[idx].active = 0;
    }
    int last_idx = window_indices[window_count - 1];
    windows[last_idx].active = 1;
    
    if (windows[last_idx].user_data) {
        global_terminal = (gshell_t*)windows[last_idx].user_data;
        gshell_set_focus(global_terminal, 1);
    }
}

void window_draw_close_button(window_t* win) {
    if (!win || !win->visible) return;
    
    u32 btn_x = win->x + win->w - 16;
    u32 btn_y = win->y + 2;
    
    fill_rect(btn_x, btn_y, 14, 10, 0x08);
    
    u32 x1 = btn_x + 2;
    u32 y1 = btn_y + 2;
    u32 x2 = btn_x + 11;
    
    for (u32 i = 0; i < 8; i++) {
        putpixel(x1 + i, y1 + i, 0x0F);
        putpixel(x1 + i + 1, y1 + i, 0x0F);
    }
    
    for (u32 i = 0; i < 8; i++) {
        putpixel(x2 - i, y1 + i, 0x0F);
        putpixel(x2 - i + 1, y1 + i, 0x0F);
    }
}

int window_hit_close_button(window_t* win, u32 mx, u32 my) {
    if (!win || !win->visible) return 0;
    
    u32 btn_x = win->x + win->w - 16;
    u32 btn_y = win->y + 2;
    
    return (mx >= btn_x && mx < btn_x + 14 &&
            my >= btn_y && my < btn_y + 10);
}

void window_close(window_t* win) {
    if (!win) return;
    
    int win_idx = -1;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (&windows[i] == win) {
            win_idx = i;
            break;
        }
    }
    
    if (win_idx == -1) return;
    
    int order_idx = -1;
    for (int i = 0; i < window_count; i++) {
        if (window_indices[i] == win_idx) {
            order_idx = i;
            break;
        }
    }
    
    if (order_idx == -1) return;
    
    if (win->user_data && (gshell_t*)win->user_data == global_terminal) {
        global_terminal = 0;
    }
    
    for (int i = order_idx; i < window_count - 1; i++) {
        window_indices[i] = window_indices[i + 1];
    }
    
    window_count--;
    windows[win_idx].visible = 0;
}
