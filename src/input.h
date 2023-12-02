#pragma once

#include <stdbool.h>
#include "bit_types.h"

typedef enum{
    INPUT_EVENT_NONE,
    INPUT_EVENT_MOUSE,
    INPUT_EVENT_KEYDOWN,
}INPUT_EVENT_PACKET_t;

typedef enum{
    MOUSE_NONE,
    MOUSE_LEFT,
    MOUSE_RIGHT
}INPUT_EVENT_MOUSE_t;

typedef struct{
    INPUT_EVENT_PACKET_t packet_type;

    union{
        struct{
            u16 x;
            u16 y;
            INPUT_EVENT_MOUSE_t button_index;
            bool shift;
        }Mouse;

        struct{
            int key;
        }KeyDown;

    };

}InputEvent_t;


InputEvent_t input_poll_input(void);

void init_input(void);
