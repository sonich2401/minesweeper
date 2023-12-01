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
    TILE_COVERED = 0xFF
};




static volatile bool is_running = false;
static pthread_t game_thread_id;


typedef u8 tile_t;

tile_t* solution_board = NULL;
tile_t* game_board = NULL;



static void* game_thread(void* args){
    (void)args;
    while(!is_running){usleep(1000);} //wait for game to start

    InputEvent_t event;
    while(true){
        //Check for new input
        event = input_poll_input();
        while(event.packet_type != INPUT_EVENT_NONE){
            
            switch(event.packet_type){
                case INPUT_EVENT_NONE:
                    DFATAL("This should never occur!");
                break;
                case INPUT_EVENT_MOUSE:
                    //mvprintw(0,0,"a %i, %i: %i", event.Mouse.x, event.Mouse.y, event.Mouse.button_index);
                    mvaddch(event.Mouse.y, event.Mouse.x, rand() % 10 + 'A');
                break;

                case INPUT_EVENT_KEYDOWN:
                break;
            }
            
            event = input_poll_input(); //Get next input
        }

        
    }
    
    return NULL;
}


#define ACCESS_GLOBALS
#include "globals.h"


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

        for(u16 tmpy = y - (y > 0); tmpy < (y + (y != SHORT_MAX)); tmpy++){
            if(tmpy >= g_game_h) continue;
            for(u16 tmpx = x - (x > 0); tmpx < (x + (x != SHORT_MAX)); tmpx++){
                if(tmpx >= g_game_w) continue;

                tile_t* tile = &solution_board[g_game_w * tmpy + tmpx];

                if(*tile == TILE_MINE) continue;
                SMART_ASSERT(*tile < TILE_8, "this should never happen");
                *tile = *tile + 1;
            }
        }
    }
}


#if TILE_UNCOVERED != 0
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
