#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "enumerations.h"
#include "fichiers.h"
#include "collisions.h"
#include "textures_fx.h"
#include "audio.h"
#include "npc.h"
#include "anim.h"
#include "script_engine.h"
#include "game_scripts.h"
#include "types_struct_defs.h"
#include "switch_events.h"
#include "sdl_init_and_quit.h"
#include "mainloop.h"

void loopcode(void *arg)
{
    game_context *gctx = arg;
    
    while(SDL_PollEvent(&gctx->e) != 0)
    {
        
        functionSwitchEvent(gctx->e, &gctx->requete, &gctx->left_right, &gctx->up_down, &gctx->jump_key_ended/*, &debug*/);
        if(gctx->requete == REQ_QUIT)
            gctx->mainloop_quit = true;
        
        //selectionne la direction où regarde le player character
        if((gctx->requete >= REQ_DIR_LEFT) && (gctx->requete <= REQ_DIR_DOWN) && 
           !(gctx->player.state & CH_STATE_BLOCK_INPUT))
            gctx->player.obj.direction = gctx->requete;
    }

    //checks if a jump can be started
    if(gctx->requete == REQ_JUMP && gctx->jump_key_ended && 
       (gctx->player.frame_jump == 0) && !(gctx->player.state & (CH_STATE_FALLING | CH_STATE_BLOCK_INPUT)))
    {
        gctx->jump_key_ended = false;
        gctx->player.state |= CH_STATE_JUMPING;
        Mix_PlayChannel(0, gctx->jump, 0);
    }
    //define if player is walking or not
    flag_assign(
        &gctx->player.state, 
        CH_STATE_WALKING, 
        ((gctx->up_down != 0) || (gctx->left_right != 0)) && !(gctx->player.state & CH_STATE_BLOCK_INPUT)
    );
    
    //define if the cam is moving or not
    if(gctx->cam_leftRight != 0)
        gctx->cam.moving = true;
    else
        gctx->cam.moving = false;
    
    //clear renderer + set background color
    SDL_SetRenderDrawColor(gctx->renderer, 115,239,232,255); //255,183,128,255
    SDL_RenderClear(gctx->renderer);
    
    //ANIMATION NPC
    anim_npc(gctx->nb_npcs, gctx->nb_objs, gctx->npcs, gctx->objs, gctx->main_tiles_array, 
             gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y, &gctx->hurt_soundflag, &gctx->bump_soundflag);
             
    //SCRIPTED NPCS
    for(size_t i = 0; i < gctx->nb_scpt_npcs; i++)
    {
        if(gctx->scpt_npcs[i].obj.enabled)
        {
            anim_scripted_npc(gctx, &gctx->scpt_npcs[i]);
        }
    }        
    //MAIN PLAYER ANIMATION
    anim_main_character(gctx);
    
    //SPECIAL COLLISIONS EVENTS
    unsigned sp_act = checkCollisionSpecialAction(gctx->nb_objs, &gctx->objs, gctx->nb_npcs, &gctx->npcs, &gctx->player, 
                                                  &gctx->main_tiles_array, &gctx->overlay_tiles_array, gctx->nbTiles_x, 
                                                  gctx->nbTiles_y, gctx->levels);
    if(sp_act != 0)
    {
        if(((sp_act & SP_AC_EARN_COIN) | (sp_act & SP_AC_EARN_HEART)) > 0)
        {
            loadLevelTiles(&gctx->level_main_layer, gctx->assets_tiles, gctx->main_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y, gctx->renderer,
                           gctx->tsnb_x, gctx->tsnb_y);
            loadLevelTiles(&gctx->level_overlay, gctx->assets_tiles, gctx->overlay_tiles_array, gctx->nbTiles_x, gctx->nbTiles_y, gctx->renderer,
                           gctx->tsnb_x, gctx->tsnb_y);
            Mix_PlayChannel(-1, gctx->coin, 0);
        }
        if ((sp_act & SP_AC_HURT) > 0)
            Mix_PlayChannel(-1, gctx->hurt, 0);
    }
    

    //CAMERA AUTO MOVEMENT :
    anim_camera(gctx->player, &gctx->cam, gctx->left_right, gctx->nbTiles_x, &gctx->cam_leftRight);
    
    //SOUNDS PLAYING
    if(gctx->hurt_soundflag)
    {
        Mix_PlayChannel(0, gctx->hurt, 0);
        gctx->hurt_soundflag = false;
    }
    if(gctx->bump_soundflag)
    {
        Mix_PlayChannel(-1, gctx->bump, 0);
        gctx->bump_soundflag = false;
    }
    
    
    //RESET PLAYER POSITION
    if((gctx->player.obj.position.y > NATIVE_HEIGHT) || (gctx->player.obj.life <= 0))
        initPlayer(&gctx->player, false);
        
    if(gctx->player.obj.life > PLAYER_MAX_LIFE)
        gctx->player.obj.life = PLAYER_MAX_LIFE;
    
    
    //RENDER
    //render level
    SDL_SetRenderTarget(gctx->renderer, NULL);
    copieTextureSurRender(gctx->renderer, gctx->level_main_layer, 0, 0, gctx->cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
    copieTextureSurRender(gctx->renderer, gctx->level_overlay, 0, 0, gctx->cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
    
    //inverts sprites
    SDL_RendererFlip flip;

    //n° of the sprite to be rendered
    int spriteID;
    //affichage des npcs
    for(int i=0; i<gctx->nb_npcs; i++)
    {
        if(gctx->npcs[i].obj.enabled)
        {
            spriteID = choseNPCSprite(gctx->npcs[i], &flip);
            loadNPCSprite(gctx, gctx->npcs[i], gctx->npcs[i].obj.position.x-gctx->cam.texLoadSrc.x, 
                          gctx->npcs[i].obj.position.y, spriteID, flip);        
        }
    }
    
    //SCRIPTED NPC
    for(size_t i = 0; i < gctx->nb_scpt_npcs; i++)
    {
        if(gctx->scpt_npcs[i].obj.enabled)
        {
            spriteID = choseCharacterSprite(gctx->scpt_npcs[i], &flip);
            loadCharacterSprite(gctx, gctx->scpt_npcs[i], gctx->scpt_npcs[i].obj.position.x-gctx->cam.texLoadSrc.x, 
                          gctx->scpt_npcs[i].obj.position.y, spriteID, flip);  
        }
    }
    
    //gives the right sprite ID depending of the player state and animation state
    spriteID = choseCharacterSprite(gctx->player, &flip);
    
    //player blinks if hurt
    static bool pl_blink = false; //if true : player disapears (used when hurt)
    if(!(gctx->player.state & CH_STATE_HURT)) 
        pl_blink = false;
    else
    {   //blinks every 8 frames
        if(gctx->player.frame_hurt % 4 == 3) pl_blink = !pl_blink;
    }
    //loads the character texture in the renderer (except when player blinks)
    if(!pl_blink)
        loadCharacterSprite(gctx, gctx->player, gctx->player.obj.position.x-gctx->cam.texLoadSrc.x, 
                            gctx->player.obj.position.y, spriteID, flip);   

    HUD_update(gctx);        
    copieTextureSurRender(gctx->renderer, gctx->hud, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
    
    sel_game_scripts(gctx, gctx->levels);
    copieTextureSurRender(gctx->renderer, gctx->stripes_texture, 0,0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
    copieTextureSurRender(gctx->renderer, gctx->text_dialog, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
    
    //update screen
    SDL_RenderPresent(gctx->renderer);

    //cycle animation
    if((gctx->player.frame_walk/DEFAULT_ANIM_FRAMES) >= WALKING_ANIMATION_FRAMES)
        gctx->player.frame_walk = 0;
    if(!(gctx->player.state & CH_STATE_JUMPING))
        gctx->player.frame_jump = 0;

}

void mainloop(game_context *gctx)
{
    while (!gctx->mainloop_quit)
    {
        loopcode(gctx);
    }
}

