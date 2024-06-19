#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>

#include "collisions.h"
#include "enumerations.h"
#include "types_struct_defs.h"
#include "anim.h"

void flag_assign(unsigned *var, unsigned flag, bool condition)
{
    *var = condition ? *var | flag : *var & ~flag;
}

void anim_npc(int nb_npcs, int nb_objs, character *npcs, interobj *objs, int *main_tiles_array, 
              int *overlay_tiles_array, int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag)
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
                                   main_tiles_array, overlay_tiles_array, nb_tuiles_x, nb_tuiles_y)
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
                              main_tiles_array, overlay_tiles_array, nb_tuiles_x, nb_tuiles_y)
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
                updatePositionWalk(nb_objs, objs, &npcs[i], npc_left_right,
                                   main_tiles_array, overlay_tiles_array, nb_tuiles_x, nb_tuiles_y/*, false*/)
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

void anim_main_character(game_context *gctx)
{
    flag_assign(&gctx->player.state, CH_STATE_MOVED, false);
    
    //===================== GRAVITY ======================
    if(gctx->player.frame_jump == 0)
    {
        flag_assign(
            &gctx->player.state,
            CH_STATE_FALLING,
            CharacterFall(gctx->nb_objs, gctx->objs, &gctx->player, gctx->main_tiles_array, 
                          gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y)
        );

        if(gctx->player.state & CH_STATE_FALLING) 
        {
            gctx->player.frame_fall++;
            flag_assign(&gctx->player.state, CH_STATE_JUMPING, false);
        }
        else 
            gctx->player.frame_fall=0;    
    }
    //====================================================
    
    //===================== JUMP =========================
    if((gctx->player.state & CH_STATE_JUMPING) && !(gctx->player.state & CH_STATE_FALLING))
    {
        flag_assign(
            &gctx->player.state,
            CH_STATE_JUMPING,
            //change la position du personnage et renvoie true jusqu'à la fin du saut
            updatePositionJump(gctx->nb_objs, gctx->objs, &gctx->player, gctx->player.frame_jump, &gctx->hurt_soundflag,
                               gctx->main_tiles_array, gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y)
        );
        //next frame jump
        gctx->player.frame_jump++;
    }
    
    if(!(gctx->player.state & CH_STATE_JUMPING)) 
        gctx->player.frame_jump = 0;
    //====================================================
    
    //================== WALK / MOVE =====================
    if(gctx->player.state & CH_STATE_WALKING)
    {
        //change la position du personnage si on marche
        flag_assign(
            &gctx->player.state,
            CH_STATE_MOVED,
            updatePositionWalk(gctx->nb_objs, gctx->objs, &gctx->player, gctx->left_right, 
                               gctx->main_tiles_array, gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y/*, renderer, debug*/)
        );                                  
        
        gctx->player.obj.collider.x = gctx->player.obj.position.x + PLAYER_COL_SHIFT;
        //go to next gctx->player.frame_walk
        gctx->player.frame_walk++;
    }
    //====================================================
    
    if(gctx->player.state & CH_STATE_HURT)
    {
        gctx->player.frame_hurt++;
        if(gctx->player.frame_hurt >= 120) 
        {
            flag_assign(&gctx->player.state, CH_STATE_HURT, false);
            gctx->player.frame_hurt = 0;
        }
    }
    
    //bump sound
    static bool fall_tmp = true;
    if(fall_tmp && !(gctx->player.state & CH_STATE_FALLING))
        gctx->bump_soundflag = true;
    fall_tmp = (gctx->player.state & CH_STATE_FALLING);
}

void anim_scripted_npc(game_context *gctx, character *ch)
{
    flag_assign(&ch->state, CH_STATE_MOVED, false);
    
    //===================== GRAVITY ======================
    if(ch->frame_jump == 0)
    {
        flag_assign(
            &ch->state,
            CH_STATE_FALLING,
            CharacterFall(gctx->nb_objs, gctx->objs, ch, gctx->main_tiles_array, 
                          gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y)
        );

        if(ch->state & CH_STATE_FALLING) 
        {
            ch->frame_fall++;
            flag_assign(&ch->state, CH_STATE_JUMPING, false);
        }
        else 
            ch->frame_fall=0;    
    }
    //====================================================
    
    //===================== JUMP =========================
    if((ch->state & CH_STATE_JUMPING) && !(ch->state & CH_STATE_FALLING))
    {
        flag_assign(
            &ch->state,
            CH_STATE_JUMPING,
            //change la position du personnage et renvoie true jusqu'à la fin du saut
            updatePositionJump(gctx->nb_objs, gctx->objs, ch, ch->frame_jump, &gctx->hurt_soundflag,
                               gctx->main_tiles_array, gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y)
        );
        //next frame jump
        ch->frame_jump++;
    }
    
    if(!(ch->state & CH_STATE_JUMPING)) 
        ch->frame_jump = 0;
    //====================================================
    
    //================== WALK / MOVE =====================
    if(ch->state & CH_STATE_WALKING)
    {
        int left_right;
        if(ch->obj.direction == REQ_DIR_LEFT)
            left_right = -1;
        else if(ch->obj.direction == REQ_DIR_RIGHT)
            left_right = +1;
        else
            left_right = 0;
        //change la position du personnage si on marche
        flag_assign(
            &ch->state,
            CH_STATE_MOVED,
            updatePositionWalk(gctx->nb_objs, gctx->objs, ch, left_right, 
                               gctx->main_tiles_array, gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y/*, renderer, debug*/)
        );                                  
        
        ch->obj.collider.x = ch->obj.position.x + PLAYER_COL_SHIFT;
        //go to next ch->frame_walk
        ch->frame_walk++;
    }
    //====================================================
    
    if(ch->state & CH_STATE_HURT)
    {
        ch->frame_hurt++;
        if(ch->frame_hurt >= 120) 
        {
            flag_assign(&ch->state, CH_STATE_HURT, false);
            ch->frame_hurt = 0;
        }
    }
    
    //bump sound
    static bool fall_tmp = true;
    if(fall_tmp && !(ch->state & CH_STATE_FALLING))
        gctx->bump_soundflag = true;
    fall_tmp = (ch->state & CH_STATE_FALLING);
    
    //cycle animation
    if((ch->frame_walk/DEFAULT_ANIM_FRAMES) >= WALKING_ANIMATION_FRAMES)
        ch->frame_walk = 0;
    if(!(ch->state & CH_STATE_JUMPING))
        ch->frame_jump = 0;
    
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
    if((diff < 0) || (diff > SPRITE_SIZE*NB_SPRITES_X))
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
