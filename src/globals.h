#pragma once




//To avoid including globals into the 'global' namespace, you must define ACCESS_GLOBALS to define the globals. You can only do this in a .c file
#ifdef ACCESS_GLOBALS
    #include "bit_types.h"
    #include <ncurses.h>

    extern u16 g_game_w;
    extern u16 g_game_h;
    extern u16 g_bomb_count;

    extern WINDOW* g_game_window;

#endif
