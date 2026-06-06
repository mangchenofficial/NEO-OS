#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"

#define VBE_WIDTH 320
#define VBE_HEIGHT 200

typedef struct {
    s32 x;
    s32 y;
    u8 buttons;
} mouse_state_t;

void mouse_init();
void mouse_handle_interrupt();
void mouse_poll();
mouse_state_t* mouse_get_state();

#endif
