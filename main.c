#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

//constantes
const char CROUTE_SPRITES_PNG[] = "assets/images/croute-sprites.png";
const char ASSETS_TILES_PNG[] = "assets/images/GBC-assets-n-tiles.png";
const char NPC_SPRITES_PNG[] = "assets/images/NPC-sprites.png";
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

typedef struct context {
    SDL_Window *main_window;
    SDL_Renderer *main_renderer;
    
    SDL_Texture *croute_texture; //contient les sprites du perso principal
    SDL_Texture *npc_texture; //sprites des NPCs
    SDL_Texture *assets_tiles; //contient les sprites du décor etc
    SDL_Texture *level_main_layer; //contient le décor principal du niveau (au niveau du joueur)
    SDL_Texture *level_overlay;
    SDL_Texture *hud;
    Mix_Chunk *jump;
    Mix_Chunk *hurt;
    Mix_Chunk *bump;
    Mix_Chunk *coin;
    camera cam;
    int tsnb_x;
    int tsnb_y;
    character player;
    int *main_tiles_grid;
    int *overlay_tiles_grid;
    character *npcs;
    interobj *objs;
    int nbTiles_x;
    int nbTiles_y;
    int nb_objs;
    int nb_npcs;
    SDL_Event e;
    int requete;
    bool jump_key_ended; //fin d'appui sur touche jump
    int up_down; 
    int left_right;
    //sound effects
    bool hurt_soundflag;
    bool bump_soundflag;    
    
    //camera
    int cam_leftRight;
} context;

//void mainloop(void *arg)
//{
//    context *ctx = arg;
//}

int main(int argc, char *argv[])
{    
    context ctx;
    int status = EXIT_FAILURE;
    ctx.main_window = NULL;
    ctx.main_renderer = NULL;
    
    ctx.croute_texture = NULL; //contient les sprites du perso principal
    ctx.npc_texture = NULL; //sprites des NPCs
    ctx.assets_tiles = NULL; //contient les sprites du décor etc
    ctx.level_main_layer = NULL; //contient le décor principal du niveau (au niveau du joueur)
    ctx.level_overlay = NULL;
    ctx.hud = NULL;
    
    ctx.cam.texLoadSrc = (SDL_Rect){0, 0, NB_SPRITES_X * SPRITE_SIZE, NB_SPRITES_Y * SPRITE_SIZE};
    ctx.cam.moving = false;
    
    //music
    //Mix_Music *music = NULL;
    
    //sounds effects
    ctx.jump = NULL;
    ctx.hurt = NULL;
    ctx.bump = NULL;
    ctx.coin = NULL;
    
    //number of tiles in the tileset
    
    
    if (0 != initSDL(&ctx.main_renderer, &ctx.main_window))
        goto SDL_Cleanup;
    if (0 != initTextures(ctx.main_renderer, &ctx.croute_texture, &ctx.assets_tiles, &ctx.npc_texture, &ctx.hud, &ctx.tsnb_x, &ctx.tsnb_y))
        goto SDL_Cleanup;
    //load audio files
    loadAudio(&ctx.jump, &ctx.hurt, &ctx.bump, &ctx.coin);
    
    initPlayer(&ctx.player, true);
    
    //===================
    //LEVEL
    ctx.main_tiles_grid = NULL;
    ctx.overlay_tiles_grid = NULL;
    ctx.npcs = NULL;
    ctx.objs = NULL;
    ctx.main_tiles_grid = malloc(sizeof(int[10][10]));
    if(NULL == ctx.main_tiles_grid)
    {
        fprintf(stderr, "Error malloc ctx.main_tiles_grid in fct main.\n");
        goto Quit;
    }
    ctx.overlay_tiles_grid = malloc(sizeof(int[10][10]));
    if(NULL == ctx.overlay_tiles_grid)
    {
        fprintf(stderr, "Error malloc ctx.overlay_tiles_grid in fct main.\n");
        goto Quit;
    }
    ctx.npcs = malloc(sizeof(character));
    if(NULL == ctx.npcs)
    {
        fprintf(stderr, "Error malloc ctx.npcs in fct main.\n");
        goto Quit;
    }
    ctx.objs = malloc(sizeof(interobj));
    if(NULL == ctx.objs)
    {
        fprintf(stderr, "Error malloc ctx.objs in fct main.\n");
        goto Quit;
    }    
    if ( 0 != loadLevel(LEVEL_1_FILENAME, &ctx.nbTiles_x, &ctx.nbTiles_y, &ctx.main_tiles_grid, &ctx.overlay_tiles_grid,
                        &ctx.objs, &ctx.nb_objs, &ctx.npcs, &ctx.nb_npcs) )
        goto Quit;
    initNPCs(&ctx.npcs, ctx.nb_npcs);
    initLevelTextures(&ctx.level_main_layer, ctx.main_renderer, ctx.nbTiles_x, ctx.nbTiles_y);
    loadLevelTiles(&ctx.level_main_layer, ctx.assets_tiles, ctx.main_tiles_grid, ctx.nbTiles_x, ctx.nbTiles_y, ctx.main_renderer,
                   ctx.tsnb_x, ctx.tsnb_y);
    initLevelTextures(&ctx.level_overlay, ctx.main_renderer, ctx.nbTiles_x, ctx.nbTiles_y);
    loadLevelTiles(&ctx.level_overlay, ctx.assets_tiles, ctx.overlay_tiles_grid, ctx.nbTiles_x, ctx.nbTiles_y, ctx.main_renderer,
                   ctx.tsnb_x, ctx.tsnb_y);
    
    //mainloop flag
    bool quit;
    quit = false;
    
    //event handler
    
    ctx.requete = REQ_NONE;
    
    //movements
    ctx.jump_key_ended = true; //fin d'appui sur touche jump
    ctx.up_down = 0; 
    ctx.left_right = 0;
    
    //sound effects
    ctx.hurt_soundflag = false;
    ctx.bump_soundflag = false;    
    
    //camera
    ctx.cam_leftRight = 0;
    //special effect collision
    //event loop
    while (!quit)
    {
        /*static bool debug = false;*/
        //printf("%f, %f\n", ctx.player.obj.position.x, ctx.player.obj.position.y);
        while(SDL_PollEvent(&ctx.e) != 0)
        {
            
            functionSwitchEvent(ctx.e, &ctx.requete, &ctx.left_right, &ctx.up_down, &ctx.jump_key_ended/*, &debug*/);
            if(ctx.requete == REQ_QUIT)
                quit = true;
            
            //selectionne la direction où regarde le player character
            if((ctx.requete >= REQ_DIR_LEFT) && (ctx.requete <= REQ_DIR_DOWN))
                ctx.player.obj.direction = ctx.requete;
        }

        //checks if a jump can be started
        if(ctx.requete == REQ_JUMP && ctx.jump_key_ended && (ctx.player.frame_jump == 0) && !(ctx.player.state & CH_STATE_FALLING))
        {
            ctx.jump_key_ended = false;
            ctx.player.state |= CH_STATE_JUMPING;
            Mix_PlayChannel(0, ctx.jump, 0);
        }
        //define if player is walking or not
        flag_assign(&ctx.player.state, CH_STATE_WALKING, ((ctx.up_down != 0) || (ctx.left_right != 0)) );
        
        //define if the cam is moving or not
        if(ctx.cam_leftRight != 0)
            ctx.cam.moving = true;
        else
            ctx.cam.moving = false;
        
        //clear renderer + set background color
        SDL_SetRenderDrawColor(ctx.main_renderer, 115,239,232,255); //255,183,128,255
        SDL_RenderClear(ctx.main_renderer);
        
        //ANIMATION NPC
        anim_npc(ctx.nb_npcs, ctx.nb_objs, ctx.npcs, ctx.objs, ctx.main_tiles_grid, 
                 ctx.nbTiles_x, ctx.nbTiles_y, &ctx.hurt_soundflag, &ctx.bump_soundflag);
        
        //MAIN PLAYER ANIMATION
        anim_main_character(ctx.nb_objs, &ctx.player, ctx.objs, ctx.main_tiles_grid, ctx.nbTiles_x, ctx.nbTiles_y, 
                            &ctx.hurt_soundflag, &ctx.bump_soundflag, ctx.up_down, ctx.left_right/*, ctx.main_renderer, debug*/);
        
        //SPECIAL COLLISIONS EVENTS
        unsigned sp_act = checkCollisionSpecialAction(ctx.nb_objs, &ctx.objs, ctx.nb_npcs, &ctx.npcs, &ctx.player, 
                                                      &ctx.main_tiles_grid, &ctx.overlay_tiles_grid, ctx.nbTiles_x, 
                                                      ctx.nbTiles_y);
        if(sp_act != 0)
        {
            if(((sp_act & SP_AC_EARN_COIN) | (sp_act & SP_AC_EARN_HEART)) > 0)
            {
                loadLevelTiles(&ctx.level_main_layer, ctx.assets_tiles, ctx.main_tiles_grid, ctx.nbTiles_x, ctx.nbTiles_y, ctx.main_renderer,
                               ctx.tsnb_x, ctx.tsnb_y);
                loadLevelTiles(&ctx.level_overlay, ctx.assets_tiles, ctx.overlay_tiles_grid, ctx.nbTiles_x, ctx.nbTiles_y, ctx.main_renderer,
                               ctx.tsnb_x, ctx.tsnb_y);
                Mix_PlayChannel(-1, ctx.coin, 0);
            }
            if ((sp_act & SP_AC_HURT) > 0)
                Mix_PlayChannel(-1, ctx.hurt, 0);
        }
        

        //CAMERA AUTO MOVEMENT :
        anim_camera(ctx.player, &ctx.cam, ctx.left_right, ctx.nbTiles_x, &ctx.cam_leftRight);
        
        //SOUNDS PLAYING
        if(ctx.hurt_soundflag)
        {
            Mix_PlayChannel(0, ctx.hurt, 0);
            ctx.hurt_soundflag = false;
        }
        if(ctx.bump_soundflag)
        {
            Mix_PlayChannel(-1, ctx.bump, 0);
            ctx.bump_soundflag = false;
        }
        
        
        //RESET PLAYER POSITION
        if((ctx.player.obj.position.y > NATIVE_HEIGHT) || (ctx.player.obj.pdv <= 0))
            initPlayer(&ctx.player, false);
            
        if(ctx.player.obj.pdv > PLAYER_MAX_LIFE)
            ctx.player.obj.pdv = PLAYER_MAX_LIFE;
        
        
        //RENDER
        //render level
        SDL_SetRenderTarget(ctx.main_renderer, NULL);
        copieTextureSurRender(ctx.main_renderer, ctx.level_main_layer, 0, 0, ctx.cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
        copieTextureSurRender(ctx.main_renderer, ctx.level_overlay, 0, 0, ctx.cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
        
        //inverts sprites
        SDL_RendererFlip flip;

        //n° of the sprite to be rendered
        int spriteID;
        //affichage des npcs
        for(int i=0; i<ctx.nb_npcs; i++)
        {
            if(ctx.npcs[i].obj.enabled)
            {
                spriteID = choseNPCSprite(ctx.npcs[i].obj.direction, ctx.npcs[i].state & CH_STATE_WALKING,
                                          ctx.npcs[i].frame_walk/NPC_ANIM_FRAME_LENGHT, &flip);
                loadNPCSprite(ctx.main_renderer, ctx.npc_texture, ctx.npcs[i].obj.position.x-ctx.cam.texLoadSrc.x, 
                              ctx.npcs[i].obj.position.y, spriteID, flip);        
            }
        }
        
        //gives the right sprite ID depending of the player state and animation state
        spriteID = chosePlayerSprite(ctx.player.obj.direction, ctx.player.state & CH_STATE_WALKING,
                                     ctx.player.obj.type, ctx.player.frame_walk/DEFAULT_ANIM_FRAMES, &flip);
        
        //player blinks if hurt
        static bool pl_blink = false; //if true : player disapears (used when hurt)
        if(!(ctx.player.state & CH_STATE_HURT)) 
            pl_blink = false;
        else
        {   //blinks every 8 frames
            if(ctx.player.frame_hurt % 4 == 3) pl_blink = !pl_blink;
        }
        //loads the character texture in the renderer (except when player blinks)
        if(!pl_blink)
            loadPlayerSprite(ctx.main_renderer, ctx.croute_texture, ctx.player.obj.position.x-ctx.cam.texLoadSrc.x, 
                             ctx.player.obj.position.y, spriteID, flip);   

        HUD_update(ctx.main_renderer, &ctx.hud, ctx.assets_tiles, ctx.tsnb_x, ctx.tsnb_y, ctx.player);
        copieTextureSurRender(ctx.main_renderer, ctx.hud, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
        //update screen
        SDL_RenderPresent(ctx.main_renderer);

        //cycle animation
        if((ctx.player.frame_walk/DEFAULT_ANIM_FRAMES) >= WALKING_ANIMATION_FRAMES)
            ctx.player.frame_walk = 0;
        if(!(ctx.player.state & CH_STATE_JUMPING))
            ctx.player.frame_jump = 0;
    }

    status = EXIT_SUCCESS;
    
Quit:
    free(ctx.main_tiles_grid);
    free(ctx.npcs);
    free(ctx.objs);
SDL_Cleanup:
    if(NULL != ctx.jump)
        Mix_FreeChunk(ctx.jump);
    if(NULL != ctx.hurt)
        Mix_FreeChunk(ctx.hurt);
    if(NULL != ctx.bump)
        Mix_FreeChunk(ctx.bump);
    if(NULL != ctx.coin)
        Mix_FreeChunk(ctx.coin);
        
    destroyTextures(&ctx.croute_texture, &ctx.assets_tiles, &ctx.level_main_layer, &ctx.npc_texture,
                    &ctx.level_overlay, &ctx.hud);
    quitSDL(&ctx.main_renderer, &ctx.main_window);
    
    return status;
}
