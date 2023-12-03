#include "game.h"
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <limits.h>

#define SHORT_MAX (0xFFFF)

#include "input.h"

#include "debug.h"
#define ACCESS_GLOBALS
#include "globals.h"
#include "colors.h"


enum{
    TILE_UNCOVERED,
    TILE_1,
    TILE_2,
    TILE_3,
    TILE_4,
    TILE_5,
    TILE_6,
    TILE_7,
    TILE_8,
    TILE_MINE,
    TILE_INVALID,
    TILE_COVERED,
    TILE_FLAG,
    TILE_ENUM_SIZE
};


static const char IMG_LIST[TILE_ENUM_SIZE] = {
    ' ',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '*',
    '\0',
    ' ',
    '>',
};

static const COLOR_t COLOR_LIST[TILE_ENUM_SIZE] = {
    MINESWEEPER_COLOR_BLANK,
    MINESWEEPER_COLOR_CYAN, //1
    MINESWEEPER_COLOR_GREEN,
    MINESWEEPER_COLOR_RED,
    MINESWEEPER_COLOR_MAGENTA,
    MINESWEEPER_COLOR_YELLOW,
    MINESWEEPER_COLOR_GREEN,
    MINESWEEPER_COLOR_CYAN,
    MINESWEEPER_COLOR_RED, //8
    MINESWEEPER_COLOR_RED, //'*'
    MINESWEEPER_COLOR_BLANK,
    MINESWEEPER_COLOR_COVERED_TILE,
    MINESWEEPER_COLOR_RED, //'>'
};



#define IMG_MINE (IMG_LIST[TILE_MINE])
#define IMG_COVERED (IMG_LIST[TILE_COVERED])
#define IMG_UNCONVERED (IMG_LIST[TILE_UNCOVERED])
#define IMG_FLAG (IMG_LIST[TILE_FLAG])



#define INDEX_BOARD(buffer, x, y) buffer[g_game_w * y + x]

static volatile bool is_running = false;
static pthread_t game_thread_id;


typedef u8 tile_t;

static tile_t* solution_board = NULL;
static tile_t* game_board = NULL;

static bool game_over = false;


#define tile_to_color(tile) (COLOR_LIST[tile])

static void uncover_tile(u16 x, u16 y){
	if(INDEX_BOARD(game_board, x, y) == TILE_FLAG) return;
    if(INDEX_BOARD(game_board, x, y) == TILE_UNCOVERED) return;
    if(INDEX_BOARD(solution_board, x, y) == TILE_MINE){
        INDEX_BOARD(game_board, x, y) = TILE_MINE;
        place_color_char(x, y, IMG_MINE, MINESWEEPER_COLOR_RED);
        game_over = true;
        return;
    }

    tile_t solution_tile = INDEX_BOARD(solution_board, x, y);
    INDEX_BOARD(game_board, x, y) = solution_tile;

    place_color_char(x, y, IMG_LIST[solution_tile], tile_to_color(solution_tile));

    if(solution_tile != TILE_UNCOVERED) return; //Dont uncover the radius around it if it has a number

    u16 minx, miny;
    u16 maxx, maxy;
    minx = x - (x > 0);
    miny = y - (y > 0);
    maxx = x + (x+1 < g_game_w);
    maxy = y + (y+1 < g_game_h);

    for(u16 tmpy = miny; tmpy <= maxy; tmpy++){
        for(u16 tmpx = minx; tmpx <= maxx; tmpx++){
            if(tmpx == x && tmpy == y) continue;
            tile_t tile = INDEX_BOARD(game_board, tmpx, tmpy);
            if(tile != TILE_MINE && tile != TILE_UNCOVERED){
                uncover_tile(tmpx, tmpy);
            }
        }
    }
}



static void* game_thread(void* args){
    (void)args;
    while(!is_running){usleep(1000);} //wait for game to start

    InputEvent_t event;
    while(true){
		//Create delay to not max out CPU
		usleep(1000);
    
        //Check for new input
        event = input_poll_input();
        while(event.packet_type != INPUT_EVENT_NONE){
            
            switch(event.packet_type){
                case INPUT_EVENT_NONE:
                    DFATAL("This should never occur!");
                break;
                case INPUT_EVENT_MOUSE:
                    //mvprintw(0,0,"a %i, %i: %i", event.Mouse.x, event.Mouse.y, event.Mouse.button_index);
					if(event.Mouse.shift == true || (event.Mouse.button_index & MOUSE_RIGHT)){
						tile_t* cur_tile = &INDEX_BOARD(game_board, event.Mouse.x, event.Mouse.y);
						if(*cur_tile == TILE_COVERED){
							*cur_tile = TILE_FLAG;
						}
						else if(*cur_tile == TILE_FLAG){
							*cur_tile = TILE_COVERED;
						}
						place_color_char(event.Mouse.x, event.Mouse.y, IMG_LIST[*cur_tile], tile_to_color(*cur_tile));
					}else{
                   		uncover_tile(event.Mouse.x, event.Mouse.y);
                    }
                break;

                case INPUT_EVENT_KEYDOWN:
					if(event.KeyDown.key == 'q'){
						return NULL; //exit game
					}         	
                break;
            }
            
            event = input_poll_input(); //Get next input
        }


        if(game_over){
            return NULL;
        }
        
    }
    
    return NULL;
}



static void game_build_board(void){
    srand(time(NULL));
    for(u16 i = 0; i < g_bomb_count; i++){
        u16 x, y, failsafe;
        failsafe = 0;
        do{
            x = rand() % g_game_w;
            y = rand() % g_game_h;
            SMART_ASSERT(failsafe++ < g_game_h * g_game_w, "failsafe! Game locked up!");
        }while(solution_board[g_game_w * y + x] == TILE_MINE);

        solution_board[y * g_game_w + x] = TILE_MINE;

        u16 minx, miny;
        u16 maxx, maxy;
        minx = x - (x > 0);
        miny = y - (y > 0);
        maxx = x + (x+1 < g_game_w);
        maxy = y + (y+1 < g_game_h);

        for(u16 tmpy = miny; tmpy <= maxy; tmpy++){
            for(u16 tmpx = minx; tmpx <= maxx; tmpx++){
                if(tmpx == x && tmpy == y) continue;
                tile_t* tile = &INDEX_BOARD(solution_board, tmpx, tmpy);

                if(*tile == TILE_MINE) continue;
                SMART_ASSERT(*tile < TILE_8, "this should never happen");
                *tile = *tile + 1;
            }
        }
    }
}


#if TILE_UNCOVERED != 0 || TILE_1 != 0
    #error "TILE_UNCOVERED must be zero for game_build_board to work!"
#endif

void init_game(void){
    size_t byte_count =  g_game_w * g_game_h * sizeof(tile_t);
    game_board = (tile_t*)malloc(byte_count);
    SMART_ASSERT(game_board != NULL, "ran out of memory!\n Tried to allocate %lu, bytes!", byte_count);
    solution_board = (tile_t*)malloc(byte_count);
    SMART_ASSERT(solution_board != NULL, "ran out of memory!\n Tried to allocate %lu, bytes!", byte_count);

    for(size_t i = 0; i < g_game_w * g_game_h; i++){
        game_board[i] = TILE_COVERED;
        solution_board[i] = TILE_UNCOVERED;
    }

    game_build_board();

    for(u16 x = 0; x < g_game_w; x++){
        for(u16 y = 0; y < g_game_h; y++){
            place_color_char(x, y, IMG_COVERED, MINESWEEPER_COLOR_COVERED_TILE);
        }
    }

    int return_code;
    //return_code = pthread_create(&game_thread_id, NULL, game_thread, NULL);
    //SMART_ASSERT(return_code == 0, "failed to create game thread");
}


int run_game(void){
    
    is_running = true;
    //(void)pthread_join(game_thread_id, NULL);
    (void)game_thread(NULL);
    
    return EXIT_SUCCESS;
}
