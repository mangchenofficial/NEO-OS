#include "mouse.h"
#include "screen.h"
#include "ports.h"

static mouse_state_t mouse_state;
static int mouse_packet[3];
static int packet_idx = 0;

static void mouse_wait(u8 a_type) {
    u32 timeout = 100000;
    if (a_type == 0) {
        while (timeout-- && (inb(0x64) & 1) == 0);
    } else {
        while (timeout-- && (inb(0x64) & 2) != 0);
    }
}

static u8 mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

static void mouse_write(u8 a_write) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, a_write);
}

void mouse_init() {
    mouse_state.x = VBE_WIDTH / 2;
    mouse_state.y = VBE_HEIGHT / 2;
    mouse_state.buttons = 0;
    packet_idx = 0;
    
    mouse_wait(1);
    outb(0x64, 0xA8);
    
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    u8 status = inb(0x60) | 2;
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);
    
    mouse_write(0xF6);
    mouse_read();
    
    mouse_write(0xF4);
    mouse_read();
}

void mouse_handle_interrupt() {
    u8 status = inb(0x64);
    if (!(status & 1)) return;
    if (!(status & 0x20)) return;
    
    u8 data = inb(0x60);
    
    mouse_packet[packet_idx++] = data;
    
    if (packet_idx == 3) {
        packet_idx = 0;
        
        int dx = mouse_packet[1];
        int dy = mouse_packet[2];
        
        if (mouse_packet[0] & 0x10) dx -= 256;
        if (mouse_packet[0] & 0x20) dy -= 256;
        
        mouse_state.buttons = mouse_packet[0] & 0x07;
        
        mouse_state.x += dx;
        mouse_state.y -= dy;
        
        if (mouse_state.x < 0) mouse_state.x = 0;
        if (mouse_state.y < 0) mouse_state.y = 0;
        if (mouse_state.x >= VBE_WIDTH) mouse_state.x = VBE_WIDTH - 1;
        if (mouse_state.y >= VBE_HEIGHT) mouse_state.y = VBE_HEIGHT - 1;
    }
}

void mouse_poll() {
    while (1) {
        u8 status = inb(0x64);
        
        if (!(status & 1)) break;
        
        if (status & 0x20) {
            u8 data = inb(0x60);
            
            mouse_packet[packet_idx++] = data;
            
            if (packet_idx == 3) {
                packet_idx = 0;
                
                int dx = mouse_packet[1];
                int dy = mouse_packet[2];
                
                if (mouse_packet[0] & 0x10) dx -= 256;
                if (mouse_packet[0] & 0x20) dy -= 256;
                
                mouse_state.buttons = mouse_packet[0] & 0x07;
                
                mouse_state.x += dx;
                mouse_state.y -= dy;
                
                if (mouse_state.x < 0) mouse_state.x = 0;
                if (mouse_state.y < 0) mouse_state.y = 0;
                if (mouse_state.x >= VBE_WIDTH) mouse_state.x = VBE_WIDTH - 1;
                if (mouse_state.y >= VBE_HEIGHT) mouse_state.y = VBE_HEIGHT - 1;
            }
        } else {
            break;
        }
    }
}

mouse_state_t* mouse_get_state() {
    return &mouse_state;
}
