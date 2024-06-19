#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

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

//constantes
const char ANTOINE_SPRITES_PNG[] = "assets/images/antoine-sprites.png";
const char DANIEL_SPRITES_PNG[] = "assets/images/daniel-sprites.png";
const char ASSETS_TILES_PNG[] = "assets/images/assets-tiles3.png";
const char SANGLIER_SPRITES_PNG[] = "assets/images/sanglier-sprites.png";
const char AUDIO_FILE_JUMP[] = "assets/sounds/Jump.wav";
const char AUDIO_FILE_HURT[] = "assets/sounds/Hurt.wav";
const char AUDIO_FILE_BUMP[] = "assets/sounds/Bump.wav";
const char AUDIO_FILE_COIN[] = "assets/sounds/Coin.wav";
const char LEVEL_1_FILENAME[] = "assets/levels/level.csv";
const char TTF_FONT_FILENAME[] = "assets/m5x7.ttf";
const float player_speed = 1;
const float gravity = 0.25; //default 0.35
const float jump_init_speed = 4.5; //default 5.5
const SDL_Rect RECT_NULL = {0,0,0,0};

int main(void)
{    
    if (setlocale(LC_ALL, "") == NULL)
    {
        perror("setlocale");
        return EXIT_FAILURE;
    }
    
    game_context gctx;
    int status = EXIT_FAILURE;
    gctx.main_window = NULL;
    gctx.renderer = NULL;
    
    gctx.antoine_texture = NULL; //contient les sprites du perso principal
    gctx.daniel_texture = NULL; //contient les sprites du perso principal
    gctx.sanglier_texture = NULL; //sprites des NPCs
    gctx.assets_tiles = NULL; //contient les sprites du décor etc
    gctx.level_main_layer = NULL; //contient le décor principal du niveau (au niveau du joueur)
    gctx.level_overlay = NULL;
    gctx.hud = NULL;
    gctx.text_dialog = NULL;
    gctx.stripes_texture = NULL;
    
    gctx.cam.texLoadSrc = (SDL_Rect){0, 0, NB_SPRITES_X * SPRITE_SIZE, NB_SPRITES_Y * SPRITE_SIZE};
    gctx.cam.moving = false;
    
    //sounds effects
    gctx.jump = NULL;
    gctx.hurt = NULL;
    gctx.bump = NULL;
    gctx.coin = NULL;
    
    if (0 != initSDL(&gctx.renderer, &gctx.main_window))
        goto SDL_Cleanup;
    if (0 != initTextures(&gctx))
        goto SDL_Cleanup;
    //load audio files
    loadAudio(&gctx.jump, &gctx.hurt, &gctx.bump, &gctx.coin);
    
    initPlayer(&gctx.player, true);
    
    //===================
    //LEVEL
    
    gameLevel level1 = {
        1, //ID
        LEVEL_1_FILENAME,
        1, //nb_triggers
        1, //nb_scripts
        {0,0,0,0,0,0,0,0,0,0}, //script_link
        0 //active_scripts
    };
    
    gctx.levels = &level1;
    gctx.nb_levels = 1;
    
    gctx.main_tiles_array = NULL;
    gctx.overlay_tiles_array = NULL;
    gctx.npcs = NULL;
    gctx.objs = NULL;
    gctx.scpt_npcs = NULL;
    gctx.nb_scpt_npcs = 0;
    gctx.main_tiles_array = malloc(sizeof(int[10][10]));
    if(NULL == gctx.main_tiles_array)
    {
        fprintf(stderr, "Error malloc gctx.main_tiles_array in fct main.\n");
        goto Quit;
    }
    gctx.overlay_tiles_array = malloc(sizeof(int[10][10]));
    if(NULL == gctx.overlay_tiles_array)
    {
        fprintf(stderr, "Error malloc gctx.overlay_tiles_array in fct main.\n");
        goto Quit;
    }
    gctx.npcs = malloc(sizeof(character));
    if(NULL == gctx.npcs)
    {
        fprintf(stderr, "Error malloc gctx.npcs in fct main.\n");
        goto Quit;
    }
    gctx.objs = malloc(sizeof(interobj));
    if(NULL == gctx.objs)
    {
        fprintf(stderr, "Error malloc gctx.objs in fct main.\n");
        goto Quit;
    }    
    if ( 0 != loadLevel(level1.filename, &gctx) )
        goto Quit;
    initNPCs(&gctx.npcs, gctx.nb_npcs);
    initLevelTextures(&gctx.level_main_layer, gctx.renderer, gctx.nbTiles_x, gctx.nbTiles_y);
    loadLevelTiles(&gctx.level_main_layer, gctx.assets_tiles, gctx.main_tiles_array, gctx.nbTiles_x, gctx.nbTiles_y, gctx.renderer,
                   gctx.tsnb_x, gctx.tsnb_y);
    initLevelTextures(&gctx.level_overlay, gctx.renderer, gctx.nbTiles_x, gctx.nbTiles_y);
    loadLevelTiles(&gctx.level_overlay, gctx.assets_tiles, gctx.overlay_tiles_array, gctx.nbTiles_x, gctx.nbTiles_y, gctx.renderer,
                   gctx.tsnb_x, gctx.tsnb_y);

    gctx.requete = REQ_NONE;
    
    //movements
    gctx.jump_key_ended = true; //fin d'appui sur touche jump
    gctx.up_down = 0; 
    gctx.left_right = 0;
    
    //sound effects
    gctx.hurt_soundflag = false;
    gctx.bump_soundflag = false;    
    
    //camera
    gctx.cam_leftRight = 0;
    
    gctx.mainloop_quit = false;
    
#ifndef __EMSCRIPTEN__
    mainloop(&gctx);
#else
    emscripten_set_main_loop_arg(loopcode, &gctx, -1, 1);
#endif
    
    status = EXIT_SUCCESS;
    
Quit:
    if(NULL != gctx.overlay_tiles_array)
        free(gctx.overlay_tiles_array);
    if(NULL != gctx.main_tiles_array)
        free(gctx.main_tiles_array);
    if(NULL != gctx.npcs)
        free(gctx.npcs);
    if(NULL != gctx.objs)
        free(gctx.objs);
    if(NULL != gctx.scpt_npcs)
        free(gctx.scpt_npcs);
        
SDL_Cleanup:
    if(NULL != gctx.jump)
        Mix_FreeChunk(gctx.jump);
    if(NULL != gctx.hurt)
        Mix_FreeChunk(gctx.hurt);
    if(NULL != gctx.bump)
        Mix_FreeChunk(gctx.bump);
    if(NULL != gctx.coin)
        Mix_FreeChunk(gctx.coin);
        
    destroyTextures(&gctx);
    quitSDL(&gctx.renderer, &gctx.main_window);
    
    return status;
}
