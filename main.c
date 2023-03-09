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
#include "pl_mouvements.h"
#include "textures_fx.h"
#include "audio.h"
#include "npc.h"
#include "anim.h"

//constantes
const char CROUTE_SPRITES_PNG[] = "images/croute_sprites.png";
const char ASSETS_TILES_PNG[] = "images/GBC_assets_n_tiles.png";
const char NPC_SPRITES_PNG[] = "images/NPC_sprites.png";
const char AUDIO_FILE_JUMP[] = "sounds/Jump.wav";
const char AUDIO_FILE_HURT[] = "sounds/Hurt.wav";
const char AUDIO_FILE_BUMP[] = "sounds/Bump.wav";
const char AUDIO_FILE_COIN[] = "sounds/Coin.wav";
const char LEVEL_1_FILENAME[] = "levels/niveau1_builder.csv";
const char TTF_FONT_FILENAME[] = "m5x7.ttf";
const float player_speed = 1;
const float gravity = 0.35; //default 0.35
const float jump_init_speed = 5.5; //default 5.5
const SDL_Rect RECT_NULL = {0,0,0,0};

//prototypes
extern int initSDL(SDL_Renderer **, SDL_Window **);
extern int quitSDL(SDL_Renderer **, SDL_Window **);
extern void fonctionSwitchEvent(SDL_Event, int *, int *, int *, bool */*, int * */);


int main(int argc, char *argv[])
{    
    int status = EXIT_FAILURE;
    SDL_Window *main_window = NULL;
    SDL_Renderer *main_renderer = NULL;
    
    SDL_Texture *croute_texture = NULL; //contient les sprites du perso principal
    SDL_Texture *npc_texture = NULL; //sprites des NPCs
    SDL_Texture *assets_tiles = NULL; //contient les sprites du décor etc
    SDL_Texture *level_main_layer = NULL; //contient le décor principal du niveau (au niveau du joueur)
    SDL_Texture *level_overlay = NULL;
    SDL_Texture *hud = NULL;
    
    int tileset_nb_x, tileset_nb_y;
    
    cam camera = {
        {0,0,NB_SPRITES_X*SPRITE_SIZE,NB_SPRITES_Y*SPRITE_SIZE},
        {0,0,NB_SPRITES_X*SPRITE_SIZE,NB_SPRITES_Y*SPRITE_SIZE},
        false //moving
    };
    
    //music
    //Mix_Music *music = NULL;
    
    //sounds effects
    Mix_Chunk *jump = NULL;
    Mix_Chunk *hurt = NULL;
    Mix_Chunk *bump = NULL;
    Mix_Chunk *coin = NULL;
    
    if (0 != initSDL(&main_renderer, &main_window))
        goto SDL_Cleanup;
    if (0 != initTextures(main_renderer, &croute_texture, &assets_tiles, &npc_texture, &hud, &tileset_nb_x, &tileset_nb_y))
        goto SDL_Cleanup;
    //charge les fichiers audio
    loadAudio(&jump, &hurt, &bump, &coin);
    character player;
    initPlayer(&player, true);
    
    //mainloop flag
    bool quit = false;
    //event handler
    SDL_Event e;
    int requete = REQ_NONE;
    //mouvement du personnage flag
    bool jump_ended = true; //fin d'appui sur touche jump
    int up_down = 0;
    int left_right = 0;
    //numero de la sprite
    int spriteID;
    SDL_RendererFlip flip;
    //sound effects
    bool hurt_soundflag = false;
    bool bump_soundflag = false;
    
    //===================
    //LEVEL
    int *main_tiles_grid = NULL;
    int *overlay_tiles_grid = NULL;
    character *npcs = NULL;
    interobj *objs = NULL;
    main_tiles_grid = malloc(sizeof(int[10][10]));
    if(NULL == main_tiles_grid)
    {
        fprintf(stderr, "Error malloc main_tiles_grid in fct main.\n");
        goto Quit;
    }
    overlay_tiles_grid = malloc(sizeof(int[10][10]));
    if(NULL == overlay_tiles_grid)
    {
        fprintf(stderr, "Error malloc overlay_tiles_grid in fct main.\n");
        goto Quit;
    }
    npcs = malloc(sizeof(character));
    if(NULL == npcs)
    {
        fprintf(stderr, "Error malloc npcs in fct main.\n");
        goto Quit;
    }
    objs = malloc(sizeof(interobj));
    if(NULL == objs)
    {
        fprintf(stderr, "Error malloc objs in fct main.\n");
        goto Quit;
    }    
    int nb_tuiles_x, nb_tuiles_y, nb_objs, nb_npcs;
    if ( 0 != loadLevel(LEVEL_1_FILENAME, &nb_tuiles_x, &nb_tuiles_y, &main_tiles_grid, &overlay_tiles_grid,
                        &objs, &nb_objs, &npcs, &nb_npcs) )
        goto Quit;
    initNPCs(&npcs, nb_npcs);
    initLevelTextures(&level_main_layer, main_renderer, nb_tuiles_x, nb_tuiles_y);
    loadLevelTiles(&level_main_layer, assets_tiles, main_tiles_grid, nb_tuiles_x, nb_tuiles_y, main_renderer,
                   tileset_nb_x, tileset_nb_y);
    initLevelTextures(&level_overlay, main_renderer, nb_tuiles_x, nb_tuiles_y);
    loadLevelTiles(&level_overlay, assets_tiles, overlay_tiles_grid, nb_tuiles_x, nb_tuiles_y, main_renderer,
                   tileset_nb_x, tileset_nb_y);
    
    //camera
    int cam_leftRight = 0;
    //special effect collision
    int obj_type = IT_NONE;
    //event loop
    while (!quit)
    {
        //printf("%f, %f\n", player.obj.position.x, player.obj.position.y);
        while(SDL_PollEvent(&e) != 0)
        {
            fonctionSwitchEvent(e, &requete, &left_right, &up_down, &jump_ended /*, &cam_leftRight*/);
            if(requete == REQ_QUIT)
                quit = true;
            
            //selectionne la direction où regarde le player character
            if((requete >= REQ_DIR_LEFT) && (requete <= REQ_DIR_DOWN))
                player.obj.direction = requete;
        }

        //commence le saut
        if(requete == REQ_JUMP && jump_ended && (player.frame_jump == 0) && !(player.state & CH_STATE_FALLING))
        {
            jump_ended = false;
            player.state |= CH_STATE_JUMPING;
            Mix_PlayChannel(0, jump, 0);
        }
        //définit si l'on est ou non en mouvement
        flag_assign(&player.state, CH_STATE_WALKING, ((up_down != 0) || (left_right != 0)) );
        
        //idem pour la cam
        if(cam_leftRight != 0)
            camera.moving = true;
        else
            camera.moving = false;
        
        //clear renderer
        SDL_SetRenderDrawColor(main_renderer, 115,239,232,255); //255,183,128,255
        SDL_RenderClear(main_renderer);
        
        //ANIMATION NPC
        anim_npc(nb_npcs, nb_objs, npcs, objs, main_tiles_grid, 
                 nb_tuiles_x, nb_tuiles_y, &hurt_soundflag, &bump_soundflag);
        
        //ACTUALISATIONS POSITION PERSONNAGE ================================================
        anim_main_character(nb_objs, &player, objs, main_tiles_grid, nb_tuiles_x, nb_tuiles_y, &hurt_soundflag, &bump_soundflag,
                            up_down, left_right);
        
        //RECHERCHE D'ÉVÉNEMENTS LIÉS À LA RENCONTRE D'OBJETS SPÉCIAUX
        unsigned sp_act = checkCollisionSpecialAction(nb_objs, &objs, nb_npcs, &npcs, &player, 
                                                 &main_tiles_grid, &overlay_tiles_grid, nb_tuiles_x, nb_tuiles_y, &obj_type);
        if(sp_act != 0)
        {
            if(((sp_act & SP_AC_EARN_COIN) | (sp_act & SP_AC_EARN_HEART)) > 0)
            {
                loadLevelTiles(&level_main_layer, assets_tiles, main_tiles_grid, nb_tuiles_x, nb_tuiles_y, main_renderer,
                               tileset_nb_x, tileset_nb_y);
                loadLevelTiles(&level_overlay, assets_tiles, overlay_tiles_grid, nb_tuiles_x, nb_tuiles_y, main_renderer,
                               tileset_nb_x, tileset_nb_y);
                Mix_PlayChannel(-1, coin, 0);
            }
            if ((sp_act & SP_AC_HURT) > 0)
                Mix_PlayChannel(-1, hurt, 0);
        }
        

        //MOUVEMENT AUTO DE LA CAMÉRA :
        anim_camera(player, &camera, left_right, nb_tuiles_x, &cam_leftRight);
        
        //SOUNDFLAGS
        if(hurt_soundflag)
        {
            Mix_PlayChannel(0, hurt, 0);
            hurt_soundflag = false;
        }
        if(bump_soundflag)
        {
            Mix_PlayChannel(-1, bump, 0);
            bump_soundflag = false;
        }
        
        
        //RÉINITIALISATION DE LA POSITION DU JOUEUR
        if((player.obj.position.y > NATIVE_HEIGHT) || (player.obj.pdv <= 0))
            initPlayer(&player, false);
            
        if(player.obj.pdv > PLAYER_MAX_LIFE)
            player.obj.pdv = PLAYER_MAX_LIFE;
        
        
        //RENDER======================
        //render level
        SDL_SetRenderTarget(main_renderer, NULL);
        copieTextureSurRender(main_renderer, level_main_layer, 0, 0, camera.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
        copieTextureSurRender(main_renderer, level_overlay, 0, 0, camera.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);

        //affichage des npcs
        for(int i=0; i<nb_npcs; i++)
        {
            if(npcs[i].obj.enabled)
            {
                spriteID = choseNPCSprite(npcs[i].obj.direction, npcs[i].state & CH_STATE_WALKING,
                                          npcs[i].frame_walk/NPC_ANIM_FRAME_LENGHT, &flip);
                loadNPCSprite(main_renderer, npc_texture, npcs[i].obj.position.x-camera.texLoadSrc.x, 
                              npcs[i].obj.position.y, spriteID, flip);        
            }
        }
        
        //donne le bon identifiant de sprite en fonction des 3 paramètres
        spriteID = chosePlayerSprite(player.obj.direction, player.state & CH_STATE_WALKING,
                                     player.obj.type, player.frame_walk/DEFAULT_ANIM_FRAMES, &flip);
        
        //player blinks if hurt
        static bool pl_blink = false; //if true : player disapears (used when hurt)
        if(!(player.state & CH_STATE_HURT)) pl_blink = false;
        else
        {
            if(player.frame_hurt % 4 == 3) pl_blink = !pl_blink;
        }
        //charge la texture du perso dans le renderer
        if(!pl_blink)
            loadPlayerSprite(main_renderer, croute_texture, player.obj.position.x-camera.texLoadSrc.x, 
                             player.obj.position.y, spriteID, flip);   

        HUD_update(main_renderer, &hud, assets_tiles, tileset_nb_x, tileset_nb_y, player);
        copieTextureSurRender(main_renderer, hud, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
        //update screen
        SDL_RenderPresent(main_renderer);

        //cycle animation
        if((player.frame_walk/DEFAULT_ANIM_FRAMES) >= WALKING_ANIMATION_FRAMES)
            player.frame_walk = 0;
        if(!(player.state & CH_STATE_JUMPING))
            player.frame_jump = 0;
    }

    status = EXIT_SUCCESS;
Quit:
    free(main_tiles_grid);
    free(npcs);
    free(objs);
SDL_Cleanup:
    if(NULL != jump)
        Mix_FreeChunk(jump);
    if(NULL != hurt)
        Mix_FreeChunk(hurt);
    if(NULL != bump)
        Mix_FreeChunk(bump);
    if(NULL != coin)
        Mix_FreeChunk(coin);
    if(NULL != level_overlay)
        SDL_DestroyTexture(level_overlay);
    if(NULL != hud)
        SDL_DestroyTexture(hud);
        
    destroyTextures(&croute_texture, &assets_tiles, &level_main_layer, &npc_texture);
    quitSDL(&main_renderer, &main_window);
    
    return status;
}
