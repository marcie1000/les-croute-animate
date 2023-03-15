#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "collisions.h"
#include "enumerations.h"
#include "types_struct_defs.h"
#include "anim.h"

void flag_assign(unsigned *var, unsigned flag, bool condition)
{
    *var = condition ? *var | flag : *var & ~flag;
}

void anim_npc(int nb_npcs, int nb_objs, character *npcs, interobj *objs, int *main_tiles_grid, 
              int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag)
{
    *bump_soundflag = false;
    //ACTUALISATIONS POSITION NPCS ==============================
    for(int i=0; i<nb_npcs; i++)
    {
        //ne fait rien si le npc est désactivé
        if(!npcs[i].obj.enabled)
            continue;
        if(npcs[i].state & CH_STATE_JUMPING)
        {
            //change la position du personnage et renvoie true jusqu'à la fin du saut
            flag_assign(
                &npcs[i].state,
                CH_STATE_JUMPING,
                updatePositionJump(nb_objs, objs, &npcs[i], npcs[i].frame_jump, hurt_soundflag,
                                   main_tiles_grid, nb_tuiles_x, nb_tuiles_y)
            );
            //next frame jump
            npcs[i].frame_jump++;
        }
        else
        {
            flag_assign(
                &npcs[i].state,
                CH_STATE_FALLING,
                CharacterFall(nb_objs, objs, &npcs[i],
                           main_tiles_grid, nb_tuiles_x, nb_tuiles_y)
            );
            if(npcs[i].state & CH_STATE_FALLING)
            {
                npcs[i].frame_jump = 0;
                npcs[i].frame_fall++;
            }
            else
                npcs[i].frame_fall=0;
        }
        if(npcs[i].state & CH_STATE_WALKING)
        {
            int npc_left_right;
            if (npcs[i].obj.direction == REQ_DIR_LEFT)
                npc_left_right = -1;
            else
                npc_left_right = +1;
            //change la position du personnage si on marche
            flag_assign(
                &npcs[i].state,
                CH_STATE_MOVED,
                updatePositionWalk(nb_objs, objs, &npcs[i], 0, npc_left_right,
                                   main_tiles_grid, nb_tuiles_x, nb_tuiles_y/*, false*/)
            );
            //go to next npcs[i].frame_walk
            npcs[i].frame_walk++;
            //inverse la direction si mur rencontré
            if(!(npcs[i].state & CH_STATE_MOVED))
            {
                if (npcs[i].obj.direction == REQ_DIR_LEFT)
                    npcs[i].obj.direction = REQ_DIR_RIGHT;
                else
                    npcs[i].obj.direction = REQ_DIR_LEFT;
            }
        }
        
        if((npcs[i].frame_walk/NPC_ANIM_FRAME_LENGHT) >= NPC_ANIMATION_FRAMES)
            npcs[i].frame_walk = 0;
        if(!(npcs[i].state & CH_STATE_JUMPING))
            npcs[i].frame_jump = 0;
    }
}

void anim_main_character(int nb_objs, character *player, interobj *objs, int *main_tiles_grid, 
                         int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag, 
                         int up_down, int left_right/*, SDL_Renderer *renderer, bool debug*/)
{
    flag_assign(&player->state, CH_STATE_MOVED, false);
    
    //===================== GRAVITY ======================
    if(player->frame_jump == 0)
    {
        flag_assign(
            &player->state,
            CH_STATE_FALLING,
            CharacterFall(nb_objs, objs, player, main_tiles_grid, 
                          nb_tuiles_x, nb_tuiles_y)
        );

        if(player->state & CH_STATE_FALLING) 
        {
            player->frame_fall++;
            flag_assign(&player->state, CH_STATE_JUMPING, false);
        }
        else 
            player->frame_fall=0;    
    }
    //====================================================
    
    //===================== JUMP =========================
    if((player->state & CH_STATE_JUMPING) && !(player->state & CH_STATE_FALLING))
    {
        flag_assign(
            &player->state,
            CH_STATE_JUMPING,
            //change la position du personnage et renvoie true jusqu'à la fin du saut
            updatePositionJump(nb_objs, objs, player, player->frame_jump, hurt_soundflag,
                               main_tiles_grid, nb_tuiles_x, nb_tuiles_y)
        );
        //next frame jump
        player->frame_jump++;
    }
    
    if(!(player->state & CH_STATE_JUMPING)) 
        player->frame_jump = 0;
    //====================================================
    
    //================== WALK / MOVE =====================
    if(player->state & CH_STATE_WALKING)
    {
        //change la position du personnage si on marche
        flag_assign(
            &player->state,
            CH_STATE_MOVED,
            updatePositionWalk(nb_objs, objs, player, up_down, left_right, 
                               main_tiles_grid, nb_tuiles_x, nb_tuiles_y/*, renderer, debug*/)
        );                                  
        
        player->obj.collider.x = player->obj.position.x + PLAYER_COL_SHIFT;
        //go to next player->frame_walk
        player->frame_walk++;
    }
    //====================================================
    
    if(player->state & CH_STATE_HURT)
    {
        player->frame_hurt++;
        if(player->frame_hurt >= 120) 
        {
            flag_assign(&player->state, CH_STATE_HURT, false);
            player->frame_hurt = 0;
        }
    }
    
    //bump sound
    static bool fall_tmp = true;
    if(fall_tmp && !(player->state & CH_STATE_FALLING))
        *bump_soundflag = true;
    fall_tmp = (player->state & CH_STATE_FALLING);
}

void anim_camera(character player, camera *cam, int left_right, int nb_tuiles_x, int *cam_leftRight)
{
    int diff = player.obj.collider.x - cam->texLoadSrc.x;
    //si le perso a pu avancer
    if(player.state & CH_STATE_MOVED)
    {
        //bouge la caméra vers la droite si pos joueur > 8 minisprites et gauche si pos joueur < 8 minisprites
        if( ( (diff > 8*TILE_SIZE) && (left_right == +1) ) || ( (diff < 8*TILE_SIZE) && (left_right == -1) ) )
        {
            cam->moving = true;
            *cam_leftRight = left_right;
            
            //dépassement des limites de déplacement caméra
            int x_max = nb_tuiles_x * TILE_SIZE;
            if(*cam_leftRight == -1 && cam->texLoadSrc.x <= 0)
            {
                //cam->absCoord.x = 0;
                cam->texLoadSrc.x = 0;
                cam->moving = false;
                *cam_leftRight = 0;
            }
            else if((*cam_leftRight == +1) && (cam->texLoadSrc.x + cam->texLoadSrc.w >= x_max))
            {
                //cam->absCoord.x = x_max - cam->absCoord.w;
                cam->texLoadSrc.x = x_max - cam->texLoadSrc.w;
                cam->moving = false;
                *cam_leftRight = 0;
            }
        }
    }
    else
    {
        cam->moving = false;
        *cam_leftRight = 0;
    }
    //si joueur en dehors de la caméra
    if(!(player.state & CH_STATE_MOVED) && ((diff < 0) || (diff > SPRITE_SIZE*NB_SPRITES_X)))
    {
        cam->texLoadSrc.x = player.obj.position.x;
        //cam->absCoord.x = player.obj.position.x;
    }
    if(cam->moving)
    {
        cam->texLoadSrc.x += *cam_leftRight;
        //cam->absCoord.x += *cam_leftRight;
    }
}
