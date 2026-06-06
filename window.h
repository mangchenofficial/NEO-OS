#ifndef WINDOW_H
#define WINDOW_H

#include "types.h"

#define MAX_WINDOWS 16

typedef struct {
    u32 x;
    u32 y;
    u32 w;
    u32 h;
    char title[64];
    u32 active;
    u32 visible;
    u32 z_order;
    u32 dragging;
    s32 drag_offset_x;
    s32 drag_offset_y;
    void *user_data;
} window_t;

void window_manager_init();
window_t* window_create(u32 x, u32 y, u32 w, u32 h, const char* title);
void window_draw(window_t* win);
void window_draw_all();
int window_contains(window_t* win, u32 mx, u32 my);
window_t* window_find_at(u32 mx, u32 my);
void window_bring_to_front(window_t* win);
int window_in_titlebar(window_t* win, u32 mx, u32 my);
void window_start_drag(window_t* win, u32 mx, u32 my);
void window_drag_update(window_t* win, u32 mx, u32 my);
void window_stop_drag(window_t* win);
void window_close(window_t* win);
int window_hit_close_button(window_t* win, u32 mx, u32 my);
void window_draw_close_button(window_t* win);

#endif
