#define ACCESS_GLOBALS
#include "globals.h"

#define DEFAULT_GAME_W (16)
#define DEFAULT_GAME_H (16)
#define DEFAULT_BOMB_COUNT (40)

u16 g_game_w = DEFAULT_GAME_W;
u16 g_game_h = DEFAULT_GAME_H;
u16 g_bomb_count = DEFAULT_BOMB_COUNT;
GAMESTATE_t g_game_state = GAMESTATE_PLAYING;

WINDOW* g_game_window;
