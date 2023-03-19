#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "enumerations.h"
#include "textures_fx.h"
#include "audio.h"
#include "npc.h"
#include "anim.h"
#include "script_engine.h"
#include "types_struct_defs.h"

bool checkScriptState(script_states *st)
//checks if a called function in the script should be executed or not
//increment incs on every call
{
    bool test = (st->incs == st->curs);
    st->incs++;
    return test;
}

//void timerInit (gTimer *timer, script_states *st)
//{
//    //execute the function or not
//    if(!checkScriptState(st))
//        return;
//    
//    timer->init_t = SDL_GetTicks64();
//    //increment current state
//    st->curs++;
//    st->first_call = true;
//}

void timerWait (gTimer *timer, int wait_time, script_states *st)
{
    //execute the function or not
    if(!checkScriptState(st))
        return;
    
    if(st->first_call)
        timer->init_t = SDL_GetTicks64();
        
    st->first_call = false;
    
    timer->current_t = SDL_GetTicks64();
    //if wait time isn't elapsed
    if(timer->current_t - timer->init_t < wait_time)
        return;
    //else
    //increment current state
    st->curs++;
    st->first_call = true;
}

void scriptDialog (game_context *gctx, wchar_t *wstr, int speed, script_states *st)
{
    //execute the function or not
    if(!checkScriptState(st))
        return;
        
    int return_value;
    
    //sends the string on first call, NULL on subsequent calls
    if(st->first_call)
        return_value = textDialogUpdate(gctx, wstr, speed);
    else
        return_value = textDialogUpdate(gctx, NULL, speed);
    
    st->first_call = false;
    
    //if text is completely showed
    if(return_value == S_SUCCESS_DONE)
    {
        //increment current state
        st->curs++;
        st->first_call = true;
    }
}

void scriptDialogClear (game_context *gctx, script_states *st)
{
    //execute the function or not
    if(!checkScriptState(st))
        return;
        
    textDialogClear(gctx);
    //increment current state
    st->curs++;
    st->first_call = true;    
}

int chinit(game_context *gctx, const character *ch, script_states *st)
{
    //execute the function or not
    if(!checkScriptState(st))
        return EXIT_SUCCESS;
        
    if(gctx->nb_scpt_npcs == 0)
    {
        gctx->scpt_npcs = malloc(sizeof(character));
        if(NULL == gctx->scpt_npcs)
        {
            perror("malloc gctx->nb_scpt_npcs in function chinit: ");
            return EXIT_FAILURE;
        }
    }
    else
    {
        character *tmp = realloc(gctx->scpt_npcs, sizeof(character) * (gctx->nb_scpt_npcs + 1));
        if(NULL == tmp)
        {
            perror("realloc gctx->scpt_npcs in function chinit: ");
            return EXIT_FAILURE;
        }
        gctx->scpt_npcs = tmp;
    }
    gctx->nb_scpt_npcs++;
    gctx->scpt_npcs[gctx->nb_scpt_npcs-1] = *ch;
    
    //increment current state
    st->curs++;
    st->first_call = true;
    return EXIT_SUCCESS;
}

int search_ch(game_context *gctx, const char *name, size_t *id)
//not meant to be called in the scripts. 
//searchs a character by the name
{
    *id = 0;
    if(gctx->nb_scpt_npcs <= 0)
    {
        fprintf(stderr, "Error chdestroy: gctx->nb_scpt_npcs = 0\n.");
        return EXIT_FAILURE;
    }
    bool found = false;
    for(size_t i = 0; i < gctx->nb_scpt_npcs; i++)
    {
        if(!(NULL == strstr(gctx->scpt_npcs[i].name, name)))
        {
            found = true;
            *id = i;
            break;
        }
    }
    
    if(!found)
    {
        fprintf(stderr, "Error chdestroy: name not found\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int chdestroy(game_context *gctx, const char *name, script_states *st)
{
    //execute the function or not
    if(!checkScriptState(st))
        return EXIT_SUCCESS;
        
    size_t ch_ID;
    if(0 != search_ch(gctx, name, &ch_ID))
    {
        //increment current state
        st->curs++;
        st->first_call = true;
        return EXIT_FAILURE;
    }
 
    size_t newlength = gctx->nb_scpt_npcs - 1;
    
    if (newlength == 0)
    {
        if(gctx->scpt_npcs != NULL)
            free(gctx->scpt_npcs);
        gctx->scpt_npcs = NULL;
        gctx->nb_scpt_npcs = 0;
        //increment current state
        st->curs++;
        st->first_call = true;
        return EXIT_SUCCESS; 
    }
    
    //resize array
    character *temp_tab = malloc(sizeof(character[newlength]));
    if(NULL == temp_tab)
    {
        perror("Error malloc temp_tab in function 'deleteNPC': ");
        //increment current state
        st->curs++;
        st->first_call = true;
        return EXIT_FAILURE;
    }
    //incremental value used as the temp_tab subscript
    size_t i_newtab = 0;
    for(size_t i = 0; i < gctx->nb_scpt_npcs; i++)
    {
        //avoid the value to delete
        if(i == ch_ID)
            continue;
        
        temp_tab[i_newtab] = gctx->scpt_npcs[i];
        i_newtab++;
    }
    
    free(gctx->scpt_npcs);
    gctx->scpt_npcs = temp_tab;
    gctx->nb_scpt_npcs = newlength;
    
    //increment current state
    st->curs++;
    st->first_call = true;
    return EXIT_SUCCESS;    
}

void chwalk(game_context *gctx, const char* name, SDL_Point dest, script_states *st)
{
    //execute the function or not
    if(!checkScriptState(st))
        return;
    
    size_t ch_ID;
    if(0 != search_ch(gctx, name, &ch_ID))
    {
        //increment current state
        st->curs++;
        st->first_call = true;
        return;
    }
    
    static int direction;
    //set direction
    if(st->first_call)
    {
        if(gctx->scpt_npcs[ch_ID].obj.position.x > dest.x)
            direction = REQ_DIR_LEFT;
        else
            direction = REQ_DIR_RIGHT;
    }
    
    gctx->scpt_npcs[ch_ID].obj.direction = direction;
    st->first_call = false;
    gctx->scpt_npcs[ch_ID].state |= CH_STATE_WALKING; //walking true
    
    bool end = false;
    //if target reached
    if((direction == REQ_DIR_LEFT) && (gctx->scpt_npcs[ch_ID].obj.position.x <= dest.x))
        end = true;
    else if((direction == REQ_DIR_RIGHT) && (gctx->scpt_npcs[ch_ID].obj.position.x >= dest.x))
        end = true;
    
    if(end)
    {
        //stop walking
        gctx->scpt_npcs[ch_ID].state &= ~(CH_STATE_WALKING); //walking false
        st->curs++;
        st->first_call = true;
    }
    
}
