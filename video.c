#include "video.h"
#include "font.h"
#include "types.h"

static video_info_t video_info;
static u8 palette[256 * 3] = {0};
static u8 double_buffer_enabled = 0;
static u8 back_buffer[320 * 200];
static u8 back_buffer_init = 0;

void video_init(u8* fb_addr, u32 width, u32 height, u32 bpp) {
    video_info.framebuffer = (u8*)0xA0000;
    video_info.width = 320;
    video_info.height = 200;
    video_info.bpp = 8;
    
    for (int i = 0; i < 320 * 200; i++) {
        back_buffer[i] = 0;
    }
    back_buffer_init = 1;
    
    for (int i = 0; i < 256; i++) {
        palette[i*3 + 0] = (i & 0x07) << 5;
        palette[i*3 + 1] = ((i >> 3) & 0x07) << 5;
        palette[i*3 + 2] = ((i >> 6) & 0x03) << 6;
    }
}

void putpixel(u32 x, u32 y, u32 color) {
    if (x >= video_info.width || y >= video_info.height) return;
    u32 offset = y * video_info.width + x;
    if (double_buffer_enabled && back_buffer_init) {
        back_buffer[offset] = (u8)(color & 0xFF);
    } else {
        video_info.framebuffer[offset] = (u8)(color & 0xFF);
    }
}

u8 getpixel(u32 x, u32 y) {
    if (x >= video_info.width || y >= video_info.height) return 0;
    u32 offset = y * video_info.width + x;
    if (double_buffer_enabled && back_buffer_init) {
        return back_buffer[offset];
    }
    return video_info.framebuffer[offset];
}

void fill_rect(u32 x, u32 y, u32 w, u32 h, u32 color) {
    u8 c = (u8)color;
    u8* target = double_buffer_enabled && back_buffer ? back_buffer : video_info.framebuffer;
    for (u32 yy = y; yy < y + h; yy++) {
        u32 offset = yy * video_info.width + x;
        for (u32 xx = 0; xx < w; xx++) {
            if (offset + xx < video_info.width * video_info.height) {
                target[offset + xx] = c;
            }
        }
    }
}

u32 rgb(u8 r, u8 g, u8 b) {
    // 简单的 RGB -> VGA 色映射
    u8 cr = (r >> 5) & 0x07;
    u8 cg = (g >> 5) & 0x07;
    u8 cb = (b >> 6) & 0x03;
    return (cb << 6) | (cg << 3) | cr;
}

void clear_screen(u32 color) {
    fill_rect(0, 0, video_info.width, video_info.height, color);
}

void draw_char(u32 x, u32 y, char ch, u32 color) {
    if (ch < 32 || ch > 126) return;
    
    u32 glyph_offset = (u32)(ch - 32) * 16;
    
    for (u32 yy = 0; yy < 16; yy++) {
        u8 line = font_bitmap[glyph_offset + yy];
        for (u32 xx = 0; xx < 8; xx++) {
            if (line & (1 << (7 - xx))) {
                putpixel(x + xx, y + yy, color);
            }
        }
    }
}

void draw_string(u32 x, u32 y, const char* str, u32 color) {
    u32 cx = x;
    while (*str) {
        draw_char(cx, y, *str, color);
        cx += 8;
        str++;
    }
}

void swap_buffers() {
    if (!double_buffer_enabled || !back_buffer_init) return;
    for (u32 i = 0; i < 320 * 200; i++) {
        video_info.framebuffer[i] = back_buffer[i];
    }
}

void video_set_double_buffer(u8 enable) {
    double_buffer_enabled = enable;
}

u8* video_get_backbuffer() {
    return back_buffer;
}
