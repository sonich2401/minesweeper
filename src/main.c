#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "debug.h"
#include "bit_types.h"


#include "game.h"
#include "input.h"

#define ACCESS_GLOBALS
#include "globals.h"



static void print_help(void){
    (void)puts(
        "minesweeper [width height] [mines]"
    );
}

static void init_args(int argc, char* __restrict__ argv[]){
    argc--;
    argv++;
    if(argc > 3 || argc < 0){
        print_help();
        exit(EXIT_FAILURE);
    }

    if(argc >= 2){
        SMART_ASSERT(argv[0] != NULL, "Corrupted arguments!");
        SMART_ASSERT(argv[1] != NULL, "Corrupted arguments!");
        g_game_w = atoi(argv[0]);
        g_game_h = atoi(argv[1]);
        argc -= 2;
        argv += 2;
    }

    if(argc == 1){
        SMART_ASSERT(argv[0] != NULL, "Corrupted arguments!");
        g_bomb_count = atoi(argv[0]);
        SMART_ASSERT(g_bomb_count < g_game_w * g_game_h, "Too many bombs! Please make size bigger or choose less than %u bombs!", g_game_w * g_game_h);
        argc--;
        argv++;
    }

    SMART_ASSERT(argc == 0, "There are still arguments left over!");
}

#include "colors.h"
static void init_curses(void){
    g_game_window = initscr();
    (void)noecho();
    (void)raw();
    (void)keypad(stdscr, TRUE);
    (void)mousemask(BUTTON1_PRESSED | BUTTON3_PRESSED | BUTTON_SHIFT, NULL);

    //Create colors
    start_color();
    init_pair(MINESWEEPER_COLOR_BLANK, COLOR_WHITE, COLOR_BLACK);
    init_pair(MINESWEEPER_COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(MINESWEEPER_COLOR_COVERED_TILE, COLOR_WHITE, COLOR_WHITE);
    init_pair(MINESWEEPER_COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(MINESWEEPER_COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(MINESWEEPER_COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(MINESWEEPER_COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(MINESWEEPER_COLOR_FLAG, COLOR_RED, COLOR_WHITE);
}



int main(int argc, char* argv[]){
    init_args(argc, argv);
    init_curses();

    init_input();
    init_game();

    int return_code = run_game();

    endwin();
 
    return return_code;
}
