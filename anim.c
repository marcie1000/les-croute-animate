#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "pl_mouvements.h"
#include "enumerations.h"
#include "types_struct_defs.h"
#include "anim.h"


void anim_npc(int nb_npcs, int nb_objs, character *npcs, interobj *objs, int *main_tiles_grid, 
              int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag)
{
    //ACTUALISATIONS POSITION NPCS ==============================
    for(int i=0; i<nb_npcs; i++)
    {
        //ne fait rien si le npc est désactivé
        if(!npcs[i].obj.enabled)
            continue;
        if(npcs[i].jumping)
        {
            //change la position du personnage et renvoie true jusqu'à la fin du saut
            npcs[i].jumping = updatePositionJump(nb_objs, objs, &npcs[i], npcs[i].frame_jump, hurt_soundflag,
                                                 main_tiles_grid, nb_tuiles_x, nb_tuiles_y);
            //next frame jump
            npcs[i].frame_jump++;
        }
        else
        {
            npcs[i].falling = playerFall(nb_objs, objs, &npcs[i], npcs[i].frame_fall, bump_soundflag,
                                         main_tiles_grid, nb_tuiles_x, nb_tuiles_y);
            if(npcs[i].falling)
            {
                npcs[i].frame_jump = 0;
                npcs[i].frame_fall++;
            }
            else
                npcs[i].frame_fall=0;
        }
        bool npc_moved;
        if(npcs[i].walking)
        {
            int npc_left_right;
            if (npcs[i].obj.direction == REQ_DIR_LEFT)
                npc_left_right = -1;
            else
                npc_left_right = +1;
            //change la position du personnage si on marche
            npc_moved = updatePositionWalk(nb_objs, objs, &npcs[i], 0, npc_left_right,
                                           main_tiles_grid, nb_tuiles_x, nb_tuiles_y);
            //go to next npcs[i].frame_walk
            npcs[i].frame_walk++;
            //inverse la direction si mur rencontré
            if(!npc_moved)
            {
                if (npcs[i].obj.direction == REQ_DIR_LEFT)
                    npcs[i].obj.direction = REQ_DIR_RIGHT;
                else
                    npcs[i].obj.direction = REQ_DIR_LEFT;
            }
        }
        
        if((npcs[i].frame_walk/NPC_ANIM_FRAME_LENGHT) >= NPC_ANIMATION_FRAMES)
            npcs[i].frame_walk = 0;
        if(!npcs[i].jumping)
            npcs[i].frame_jump = 0;
    }
}

void anim_main_character(int nb_objs, character *player, interobj *objs, int *main_tiles_grid, 
                         int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag, 
                         bool *player_moved, int up_down, int left_right)
{
    if(player->jumping)
    {
        //change la position du personnage et renvoie true jusqu'à la fin du saut
        player->jumping = updatePositionJump(nb_objs, objs, player, player->frame_jump, hurt_soundflag,
                                            main_tiles_grid, nb_tuiles_x, nb_tuiles_y);
        //next frame jump
        player->frame_jump++;
    }
    else
    {
        player->falling = playerFall(nb_objs, objs, player, player->frame_fall, bump_soundflag, 
                                    main_tiles_grid, nb_tuiles_x, nb_tuiles_y);
        if(player->falling)
        {
            player->frame_jump = 0;
            player->frame_fall++;
        }
        else
            player->frame_fall=0;
    }
    if(player->walking)
    {
        //change la position du personnage si on marche
        *player_moved = updatePositionWalk(nb_objs, objs, player, up_down, left_right, 
                                          main_tiles_grid, nb_tuiles_x, nb_tuiles_y);
        player->obj.collider.x = player->obj.position.x + PLAYER_COL_SHIFT;
        //go to next player->frame_walk
        player->frame_walk++;
    }
}

void anim_camera(character player, cam *camera, bool player_moved, int left_right, int nb_tuiles_x, int *cam_leftRight)
{
    int diff = player.obj.collider.x - camera->texLoadSrc.x;
    //si le perso a pu avancer
    if(player_moved)
    {
        //bouge la caméra vers la droite si pos joueur > 8 minisprites et gauche si pos joueur < 8 minisprites
        if( ( (diff > 8*TILE_SIZE) && (left_right == +1) ) || ( (diff < 8*TILE_SIZE) && (left_right == -1) ) )
        {
            camera->moving = true;
            *cam_leftRight = left_right;
            
            //dépassement des limites de déplacement caméra
            int x_max = nb_tuiles_x * TILE_SIZE;
            if(*cam_leftRight == -1 && camera->absCoord.x <= 0)
            {
                camera->absCoord.x = 0;
                camera->texLoadSrc.x = 0;
                camera->moving = false;
                *cam_leftRight = 0;
            }
            else if((*cam_leftRight == +1) && (camera->absCoord.x + camera->absCoord.w >= x_max))
            {
                camera->absCoord.x = x_max - camera->absCoord.w;
                camera->texLoadSrc.x = x_max - camera->texLoadSrc.w;
                camera->moving = false;
                *cam_leftRight = 0;
            }
        }
    }
    else
    {
        camera->moving = false;
        *cam_leftRight = 0;
    }
    //si joueur en dehors de la caméra
    if(!player_moved && ((diff < 0) || (diff > SPRITE_SIZE*NB_SPRITES_X)))
    {
        camera->texLoadSrc.x = player.obj.position.x;
        camera->absCoord.x = player.obj.position.x;
    }
    if(camera->moving)
    {
//            //change la position de la cam
//            updatePositionCam(nb_objs, objs, &camera, *cam_leftRight);
        camera->texLoadSrc.x += *cam_leftRight;
        camera->absCoord.x += *cam_leftRight;
    }
}
