#include "input.h"


#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include "debug.h"
#include "vector.h"

#include <pthread.h>


static pthread_t input_thread_id;
static pthread_mutex_t input_mutex;
static vector input_fifo;


static void* input_thread(void* args){
    (void)args;

    int input_cache;
    MEVENT mouse_event;
    
    InputEvent_t event;
    while(1){
    	usleep(1000);
        input_cache = getch();

        switch(input_cache){
            case KEY_MOUSE:
                if(getmouse(&mouse_event) == OK){
                    event.packet_type = INPUT_EVENT_MOUSE;
                    
                    event.Mouse.x = mouse_event.x;
                    event.Mouse.y = mouse_event.y;

					event.Mouse.shift = false;
                    event.Mouse.button_index = MOUSE_NONE;
                    if(mouse_event.bstate & BUTTON1_PRESSED){
                        event.Mouse.button_index = MOUSE_LEFT;
                    }
                    if(mouse_event.bstate & BUTTON3_PRESSED){
                        event.Mouse.button_index = MOUSE_RIGHT;
                    }
                    if(mouse_event.bstate & BUTTON_SHIFT){
                    	event.Mouse.shift = true;
                    }
                }else{
                    DFATAL("Unknown mouse state error!");
                }
            break;
            
            case ERR:
            	continue;
            break;
            
            default:
                event.packet_type = INPUT_EVENT_KEYDOWN;
                event.KeyDown.key = input_cache;
            break;
        }

        
        pthread_mutex_lock(&input_mutex);
        vector_push_back(&input_fifo, &event);
        pthread_mutex_unlock(&input_mutex);
    }
    

    return NULL;
}

static const InputEvent_t NULL_EVENT = {
    .packet_type = INPUT_EVENT_NONE
};

InputEvent_t input_poll_input(void){
    pthread_mutex_lock(&input_mutex);
    size_t size = vector_size(&input_fifo);
    if(size == 0){
        //No new inputs. Return NONE
        pthread_mutex_unlock(&input_mutex);
        return NULL_EVENT;
    }

    InputEvent_t returned_data =  *(InputEvent_t*)vector_index(&input_fifo, 0); //Get front
    
    vector_erase(&input_fifo, 0); //Delete front
    pthread_mutex_unlock(&input_mutex);

    return returned_data;
}



#define ACCESS_GLOBALS
#include "globals.h"


void init_input(void){
	nodelay(g_game_window, 0);

    vector_init(&input_fifo, sizeof(InputEvent_t), NULL);

    int return_code;
    return_code = pthread_create(&input_thread_id, NULL, input_thread, NULL);
    SMART_ASSERT(return_code == 0, "input thread failed to create!");

    return_code = pthread_mutex_init(&input_mutex, NULL);
    SMART_ASSERT(return_code == 0, "input mutex failed to create!");
}
