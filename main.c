#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

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

//constantes
const char ANTOINE_SPRITES_PNG[] = "assets/images/antoine-sprites.png";
const char DANIEL_SPRITES_PNG[] = "assets/images/daniel-sprites.png";
const char ASSETS_TILES_PNG[] = "assets/images/assets-tiles.png";
const char SANGLIER_SPRITES_PNG[] = "assets/images/sanglier-sprites.png";
const char AUDIO_FILE_JUMP[] = "assets/sounds/Jump.wav";
const char AUDIO_FILE_HURT[] = "assets/sounds/Hurt.wav";
const char AUDIO_FILE_BUMP[] = "assets/sounds/Bump.wav";
const char AUDIO_FILE_COIN[] = "assets/sounds/Coin.wav";
const char LEVEL_1_FILENAME[] = "assets/levels/niveau1-builder.csv";
const char TTF_FONT_FILENAME[] = "assets/m5x7.ttf";
const float player_speed = 1;
const float gravity = 0.35; //default 0.35
const float jump_init_speed = 5.5; //default 5.5
const SDL_Rect RECT_NULL = {0,0,0,0};

//prototypes
extern int initSDL(SDL_Renderer **renderer, SDL_Window **window);
extern int quitSDL(SDL_Renderer **renderer, SDL_Window **window);
extern void functionSwitchEvent(SDL_Event e, int *requete, int *left_right, int *up_down, bool *jump_ended/*, bool *debug*/);


int main(int argc, char *argv[])
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
    
    gctx.cam.texLoadSrc = (SDL_Rect){0, 0, NB_SPRITES_X * SPRITE_SIZE, NB_SPRITES_Y * SPRITE_SIZE};
    gctx.cam.moving = false;
    
    //music
    //Mix_Music *music = NULL;
    
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
    gctx.main_tiles_grid = NULL;
    gctx.overlay_tiles_grid = NULL;
    gctx.npcs = NULL;
    gctx.objs = NULL;
    gctx.scpt_npcs = NULL;
    gctx.nb_scpt_npcs = 0;
    gctx.main_tiles_grid = malloc(sizeof(int[10][10]));
    if(NULL == gctx.main_tiles_grid)
    {
        fprintf(stderr, "Error malloc gctx.main_tiles_grid in fct main.\n");
        goto Quit;
    }
    gctx.overlay_tiles_grid = malloc(sizeof(int[10][10]));
    if(NULL == gctx.overlay_tiles_grid)
    {
        fprintf(stderr, "Error malloc gctx.overlay_tiles_grid in fct main.\n");
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
    if ( 0 != loadLevel(LEVEL_1_FILENAME, &gctx) )
        goto Quit;
    initNPCs(&gctx.npcs, gctx.nb_npcs);
    initLevelTextures(&gctx.level_main_layer, gctx.renderer, gctx.nbTiles_x, gctx.nbTiles_y);
    loadLevelTiles(&gctx.level_main_layer, gctx.assets_tiles, gctx.main_tiles_grid, gctx.nbTiles_x, gctx.nbTiles_y, gctx.renderer,
                   gctx.tsnb_x, gctx.tsnb_y);
    initLevelTextures(&gctx.level_overlay, gctx.renderer, gctx.nbTiles_x, gctx.nbTiles_y);
    loadLevelTiles(&gctx.level_overlay, gctx.assets_tiles, gctx.overlay_tiles_grid, gctx.nbTiles_x, gctx.nbTiles_y, gctx.renderer,
                   gctx.tsnb_x, gctx.tsnb_y);
    
    //mainloop flag
    bool quit;
    quit = false;
    
    //event handler
    
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
    //special effect collision
    //event loop
    while (!quit)
    {
        /*static bool debug = false;*/
        //printf("%f, %f\n", gctx.player.obj.position.x, gctx.player.obj.position.y);
        while(SDL_PollEvent(&gctx.e) != 0)
        {
            
            functionSwitchEvent(gctx.e, &gctx.requete, &gctx.left_right, &gctx.up_down, &gctx.jump_key_ended/*, &debug*/);
            if(gctx.requete == REQ_QUIT)
                quit = true;
            
            //selectionne la direction où regarde le player character
            if((gctx.requete >= REQ_DIR_LEFT) && (gctx.requete <= REQ_DIR_DOWN))
                gctx.player.obj.direction = gctx.requete;
        }

        //checks if a jump can be started
        if(gctx.requete == REQ_JUMP && gctx.jump_key_ended && (gctx.player.frame_jump == 0) && !(gctx.player.state & CH_STATE_FALLING))
        {
            gctx.jump_key_ended = false;
            gctx.player.state |= CH_STATE_JUMPING;
            Mix_PlayChannel(0, gctx.jump, 0);
        }
        //define if player is walking or not
        flag_assign(&gctx.player.state, CH_STATE_WALKING, ((gctx.up_down != 0) || (gctx.left_right != 0)) );
        
        //define if the cam is moving or not
        if(gctx.cam_leftRight != 0)
            gctx.cam.moving = true;
        else
            gctx.cam.moving = false;
        
        //clear renderer + set background color
        SDL_SetRenderDrawColor(gctx.renderer, 115,239,232,255); //255,183,128,255
        SDL_RenderClear(gctx.renderer);
        
        //ANIMATION NPC
        anim_npc(gctx.nb_npcs, gctx.nb_objs, gctx.npcs, gctx.objs, gctx.main_tiles_grid, 
                 gctx.nbTiles_x, gctx.nbTiles_y, &gctx.hurt_soundflag, &gctx.bump_soundflag);
                 
        //SCRIPTED NPCS
        for(size_t i = 0; i < gctx.nb_scpt_npcs; i++)
        {
            if(gctx.scpt_npcs[i].obj.enabled)
            {
                anim_scripted_npc(&gctx, &gctx.scpt_npcs[i]);
            }
        }        
        //MAIN PLAYER ANIMATION
        anim_main_character(&gctx);
        
        //SPECIAL COLLISIONS EVENTS
        unsigned sp_act = checkCollisionSpecialAction(gctx.nb_objs, &gctx.objs, gctx.nb_npcs, &gctx.npcs, &gctx.player, 
                                                      &gctx.main_tiles_grid, &gctx.overlay_tiles_grid, gctx.nbTiles_x, 
                                                      gctx.nbTiles_y);
        if(sp_act != 0)
        {
            if(((sp_act & SP_AC_EARN_COIN) | (sp_act & SP_AC_EARN_HEART)) > 0)
            {
                loadLevelTiles(&gctx.level_main_layer, gctx.assets_tiles, gctx.main_tiles_grid, gctx.nbTiles_x, gctx.nbTiles_y, gctx.renderer,
                               gctx.tsnb_x, gctx.tsnb_y);
                loadLevelTiles(&gctx.level_overlay, gctx.assets_tiles, gctx.overlay_tiles_grid, gctx.nbTiles_x, gctx.nbTiles_y, gctx.renderer,
                               gctx.tsnb_x, gctx.tsnb_y);
                Mix_PlayChannel(-1, gctx.coin, 0);
            }
            if ((sp_act & SP_AC_HURT) > 0)
                Mix_PlayChannel(-1, gctx.hurt, 0);
        }
        

        //CAMERA AUTO MOVEMENT :
        anim_camera(gctx.player, &gctx.cam, gctx.left_right, gctx.nbTiles_x, &gctx.cam_leftRight);
        
        //SOUNDS PLAYING
        if(gctx.hurt_soundflag)
        {
            Mix_PlayChannel(0, gctx.hurt, 0);
            gctx.hurt_soundflag = false;
        }
        if(gctx.bump_soundflag)
        {
            Mix_PlayChannel(-1, gctx.bump, 0);
            gctx.bump_soundflag = false;
        }
        
        
        //RESET PLAYER POSITION
        if((gctx.player.obj.position.y > NATIVE_HEIGHT) || (gctx.player.obj.life <= 0))
            initPlayer(&gctx.player, false);
            
        if(gctx.player.obj.life > PLAYER_MAX_LIFE)
            gctx.player.obj.life = PLAYER_MAX_LIFE;
        
        
        //RENDER
        //render level
        SDL_SetRenderTarget(gctx.renderer, NULL);
        copieTextureSurRender(gctx.renderer, gctx.level_main_layer, 0, 0, gctx.cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
        copieTextureSurRender(gctx.renderer, gctx.level_overlay, 0, 0, gctx.cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
        
        //inverts sprites
        SDL_RendererFlip flip;

        //n° of the sprite to be rendered
        int spriteID;
        //affichage des npcs
        for(int i=0; i<gctx.nb_npcs; i++)
        {
            if(gctx.npcs[i].obj.enabled)
            {
                spriteID = choseNPCSprite(gctx.npcs[i], &flip);
                loadNPCSprite(&gctx, gctx.npcs[i], gctx.npcs[i].obj.position.x-gctx.cam.texLoadSrc.x, 
                              gctx.npcs[i].obj.position.y, spriteID, flip);        
            }
        }
        
        //SCRIPTED NPC
        for(size_t i = 0; i < gctx.nb_scpt_npcs; i++)
        {
            if(gctx.scpt_npcs[i].obj.enabled)
            {
                spriteID = choseCharacterSprite(gctx.scpt_npcs[i], &flip);
                loadCharacterSprite(&gctx, gctx.scpt_npcs[i], gctx.scpt_npcs[i].obj.position.x-gctx.cam.texLoadSrc.x, 
                              gctx.scpt_npcs[i].obj.position.y, spriteID, flip);  
            }
        }
        
        //gives the right sprite ID depending of the player state and animation state
        spriteID = choseCharacterSprite(gctx.player, &flip);
        
        //player blinks if hurt
        static bool pl_blink = false; //if true : player disapears (used when hurt)
        if(!(gctx.player.state & CH_STATE_HURT)) 
            pl_blink = false;
        else
        {   //blinks every 8 frames
            if(gctx.player.frame_hurt % 4 == 3) pl_blink = !pl_blink;
        }
        //loads the character texture in the renderer (except when player blinks)
        if(!pl_blink)
            loadCharacterSprite(&gctx, gctx.player, gctx.player.obj.position.x-gctx.cam.texLoadSrc.x, 
                                gctx.player.obj.position.y, spriteID, flip);   

        HUD_update(gctx.renderer, &gctx.hud, gctx.assets_tiles, gctx.tsnb_x, gctx.tsnb_y, gctx.player);
        copieTextureSurRender(gctx.renderer, gctx.hud, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
        
        test_script(&gctx);
        
        copieTextureSurRender(gctx.renderer, gctx.text_dialog, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
        
        //update screen
        SDL_RenderPresent(gctx.renderer);

        //cycle animation
        if((gctx.player.frame_walk/DEFAULT_ANIM_FRAMES) >= WALKING_ANIMATION_FRAMES)
            gctx.player.frame_walk = 0;
        if(!(gctx.player.state & CH_STATE_JUMPING))
            gctx.player.frame_jump = 0;
    }

    status = EXIT_SUCCESS;
    
Quit:
    if(NULL != gctx.overlay_tiles_grid)
        free(gctx.overlay_tiles_grid);
    if(NULL != gctx.main_tiles_grid)
        free(gctx.main_tiles_grid);
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
