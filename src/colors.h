#pragma once
#include <ncurses.h>
#include "bit_types.h"


typedef enum{
    MINESWEEPER_COLOR_BLANK,
    MINESWEEPER_COLOR_COVERED_TILE,
    MINESWEEPER_COLOR_RED,
    MINESWEEPER_COLOR_GREEN,
    MINESWEEPER_COLOR_YELLOW,
    MINESWEEPER_COLOR_CYAN,
    MINESWEEPER_COLOR_MAGENTA,
    MINESWEEPER_COLOR_FLAG
}COLOR_t;


#define place_color_char(x, y, ch, color) attron(COLOR_PAIR(color));  mvaddch(y, x, ch); attroff(COLOR_PAIR(color)); 
