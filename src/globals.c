#define ACCESS_GLOBALS
#include "globals.h"

#define DEFAULT_GAME_W (20)
#define DEFAULT_GAME_H (20)
#define DEFAULT_BOMB_COUNT (5)

u16 g_game_w = DEFAULT_GAME_W;
u16 g_game_h = DEFAULT_GAME_H;
u16 g_bomb_count = DEFAULT_BOMB_COUNT;

WINDOW* g_game_window;
