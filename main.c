#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "enumerations.h"
#include "fichiers.h"
#include "pl_mouvements.h"
#include "textures_fx.h"
#include "audio.h"
#include "npc.h"

//constantes
const char CROUTE_SPRITES_PNG[] = "images/croute_sprites.png";
const char ASSETS_TILES_PNG[] = "images/GBC_assets_n_tiles.png";
const char NPC_SPRITES_PNG[] = "images/NPC_sprites.png";
const char AUDIO_FILE_JUMP[] = "sounds/Jump.wav";
const char AUDIO_FILE_HURT[] = "sounds/Hurt.wav";
const char AUDIO_FILE_BUMP[] = "sounds/Bump.wav";
const char AUDIO_FILE_COIN[] = "sounds/Coin.wav";
const char LEVEL_1_FILENAME[] = "levels/niveau1_builder.csv";
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
    SDL_Texture *level_main = NULL; //contient le décor principal du niveau (au niveau du joueur)
    
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
        goto Quitter;
    if (0 != initTextures(main_renderer, &croute_texture, &assets_tiles, &npc_texture))
        goto Quitter;
    //charge les fichiers audio
    loadAudio(&jump, &hurt, &bump, &coin);
    character player;
    initPlayer(&player, true);
    
    
    //mainloop flag
    bool quit = false;
    //event handler
    SDL_Event e;
    int requete = REQ_NONE;
    //current animation walking and jumping frame
//    int frame_walking = 0;
//    int frame_jump = 0;
//    int frame_fall = 0;
//    int frame_npc = 0;
    //int frame_camMov = 0;
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
    int *level_tiles_grid = malloc(sizeof(int[10][10]));
    character *npcs = malloc(sizeof(character));
    interobj *objs = malloc(sizeof(interobj));
    int nb_tuiles_x, nb_tuiles_y, nb_objs, nb_npcs;
    if ( 0 != loadLevel(LEVEL_1_FILENAME, &nb_tuiles_x, &nb_tuiles_y, &level_tiles_grid, &objs, &nb_objs, &npcs, &nb_npcs) )
        goto Quitter;
    initNPCs(&npcs, nb_npcs);
    initLevelTextures(&level_main, main_renderer, nb_tuiles_x, nb_tuiles_y);
    loadLevelTiles(&level_main, assets_tiles, level_tiles_grid, nb_tuiles_x, nb_tuiles_y, main_renderer);
    
    

    //camera
    int cam_leftRight = 0;
    //special effect collision
    int obj_type = IT_NONE;
    //event loop
    while (!quit)
    {
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
        if(requete == REQ_JUMP && jump_ended && (player.frame_jump == 0) && !player.falling)
        {
            jump_ended = false;
            player.jumping = true;
            Mix_PlayChannel(0, jump, 0);
        }
        //définit si l'on est ou non en mouvement
        if((up_down != 0) || (left_right != 0))
            player.walking = true;
        else
            player.walking = false;
        
        //idem pour la cam
        if(cam_leftRight != 0)
            camera.moving = true;
        else
            camera.moving = false;
        
        //clear renderer
        SDL_SetRenderDrawColor(main_renderer, 255,183,128,255);
        SDL_RenderClear(main_renderer);
        
        //ACTUALISATIONS POSITION NPCS ==============================
        for(int i=0; i<nb_npcs; i++)
        {
            //ne fait rien si le npc est désactivé
            if(!npcs[i].obj.enabled)
                continue;
            if(npcs[i].jumping)
            {
                //change la position du personnage et renvoie true jusqu'à la fin du saut
                npcs[i].jumping = updatePositionJump(nb_objs, objs, &npcs[i], npcs[i].frame_jump, &hurt_soundflag);
                //next frame jump
                npcs[i].frame_jump++;
            }
            else
            {
                npcs[i].falling = playerFall(nb_objs, objs, &npcs[i], npcs[i].frame_fall, &bump_soundflag);
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
                npc_moved = updatePositionWalk(nb_objs, objs, &npcs[i], up_down, npc_left_right);
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
        
        //ACTUALISATIONS POSITION PERSONNAGE ================================================
        if(player.jumping)
        {
            //change la position du personnage et renvoie true jusqu'à la fin du saut
            player.jumping = updatePositionJump(nb_objs, objs, &player, player.frame_jump, &hurt_soundflag);
            //next frame jump
            player.frame_jump++;
        }
        else
        {
            player.falling = playerFall(nb_objs, objs, &player, player.frame_fall, &bump_soundflag);
            if(player.falling)
            {
                player.frame_jump = 0;
                player.frame_fall++;
            }
            else
                player.frame_fall=0;
        }
        bool player_moved = false;
        if(player.walking)
        {
            //change la position du personnage si on marche
            player_moved = updatePositionWalk(nb_objs, objs, &player, up_down, left_right);
            player.obj.collider.x = player.obj.position.x + PLAYER_COL_SHIFT;
            //go to next player.frame_walk
            player.frame_walk++;
        }
        
        //RECHERCHE D'ÉVÉNEMENTS LIÉS À LA RENCONTRE D'OBJETS SPÉCIAUX
        if(checkCollisionSpecialEffect(nb_objs, &objs, &player, &level_tiles_grid, nb_tuiles_x, &obj_type))
        {
            loadLevelTiles(&level_main, assets_tiles, level_tiles_grid, nb_tuiles_x, nb_tuiles_y, main_renderer);
            Mix_PlayChannel(-1, coin, 0);
        }
        

        //MOUVEMENT AUTO DE LA CAMÉRA :
        int diff = player.obj.collider.x - camera.texLoadSrc.x;
        //si le perso a pu avancer
        if(player_moved)
        {
            //bouge la caméra vers la droite si pos joueur > 8 minisprites et gauche si pos joueur < 8 minisprites
            if( ( (diff > 8*TILE_SIZE) && (left_right == +1) ) || ( (diff < 8*TILE_SIZE) && (left_right == -1) ) )
            {
                camera.moving = true;
                cam_leftRight = left_right;
            }
        }
        else
        {
            camera.moving = false;
            cam_leftRight = 0;
        }
        //si joueur en dehors de la caméra
        if(!player_moved && ((diff < 0) || (diff > SPRITE_SIZE*NB_SPRITES_X)))
        {
            camera.texLoadSrc.x = player.obj.position.x;
            camera.absCoord.x = player.obj.position.x;
        }
        if(camera.moving)
        {
            //change la position de la cam
            updatePositionCam(nb_objs, objs, &camera, cam_leftRight);
        }
        
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
        
        
        
        //RENDER======================
        //render level
        SDL_SetRenderTarget(main_renderer, NULL);
        copieTextureSurRender(main_renderer, level_main, 0, 0, camera.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);

        //affichage des npcs
        for(int i=0; i<nb_npcs; i++)
        {
            if(npcs[i].obj.enabled)
            {
                spriteID = choseNPCSprite(npcs[i].obj.direction, npcs[i].walking, npcs[i].frame_walk/NPC_ANIM_FRAME_LENGHT, &flip);
                loadNPCSprite(main_renderer, npc_texture, npcs[i].obj.position.x-camera.texLoadSrc.x, npcs[i].obj.position.y, spriteID, flip);        
            }
        }
        
        //donne le bon identifiant de sprite en fonction des 3 paramètres
        spriteID = chosePlayerSprite(player.obj.direction, player.walking, player.obj.type, player.frame_walk/DEFAULT_ANIM_FRAMES, &flip);
        //charge la texture du perso dans le renderer
        loadPlayerSprite(main_renderer, croute_texture, player.obj.position.x-camera.texLoadSrc.x, player.obj.position.y, spriteID, flip);        
        //update screen
        SDL_RenderPresent(main_renderer);

        //cycle animation
        if((player.frame_walk/DEFAULT_ANIM_FRAMES) >= WALKING_ANIMATION_FRAMES)
            player.frame_walk = 0;
        if(!player.jumping)
            player.frame_jump = 0;
    }

    status = EXIT_SUCCESS;
Quitter:
    free(level_tiles_grid);
    free(npcs);
    free(objs);
    if(NULL != jump)
        Mix_FreeChunk(jump);
    if(NULL != hurt)
        Mix_FreeChunk(hurt);
    if(NULL != bump)
        Mix_FreeChunk(bump);
    if(NULL != coin)
        Mix_FreeChunk(coin);
    destroyTextures(&croute_texture, &assets_tiles, &level_main, &npc_texture);
    quitSDL(&main_renderer, &main_window);
    
    return status;
}
