#ifndef VIDEO_H
#define VIDEO_H

#include "types.h"

#define VBE_WIDTH 320
#define VBE_HEIGHT 200
#define VBE_BPP 8

typedef struct {
    u8* framebuffer;
    u32 width;
    u32 height;
    u32 bpp;
} video_info_t;

void video_init(u8* fb_addr, u32 width, u32 height, u32 bpp);
void putpixel(u32 x, u32 y, u32 color);
u8 getpixel(u32 x, u32 y);
void fill_rect(u32 x, u32 y, u32 w, u32 h, u32 color);
void draw_char(u32 x, u32 y, char ch, u32 color);
void draw_string(u32 x, u32 y, const char* str, u32 color);
void clear_screen(u32 color);
void swap_buffers();
void video_set_double_buffer(u8 enable);
u8* video_get_backbuffer();

u32 rgb(u8 r, u8 g, u8 b);

#endif
