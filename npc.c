#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "npc.h"
#include "enumerations.h"
#include "types_struct_defs.h"

void initNPCs(character **npcs, int nb_npcs)
{
    for(int i=0; i<nb_npcs; i++)
    {
        (*npcs)[i].obj.position.x *= TILE_SIZE;
        (*npcs)[i].obj.position.y *= TILE_SIZE;
        (*npcs)[i].puissance = 1;
        (*npcs)[i].walking = true;
        (*npcs)[i].jumping = false;
        (*npcs)[i].falling = false;
        (*npcs)[i].frame_walk = 0;
        (*npcs)[i].frame_jump = 0;
        (*npcs)[i].frame_fall = 0;
        if((*npcs)[i].obj.direction == 0)
            (*npcs)[i].obj.direction = REQ_DIR_LEFT;
        else
            (*npcs)[i].obj.direction = REQ_DIR_RIGHT;
        (*npcs)[i].money = 0;
    }
}
