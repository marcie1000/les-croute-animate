#ifndef TYPES_STRUCT_DEFS_H
#define TYPES_STRUCT_DEFS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "enumerations.h"

typedef struct flpoint {
    float x;
    float y;
} flpoint;

typedef struct interobj { //objets pouvant interragir avec le joueur
    int type;
    flpoint position;
    SDL_Rect collider;
    int life; //points de vie
    int direction; //direction l/r
    bool enabled; //l'objet est-il activé
} interobj;

typedef struct character { //pour tout ce qui bouge : joueur, NPC
    char *name;
    interobj obj;
    int puissance;
    int money;
    unsigned state;
    int frame_jump;
    int frame_fall;
    int frame_walk;
    int frame_hurt;
} character;

typedef struct camera {
    SDL_Rect texLoadSrc; //rect from the texture to be rendered
    //SDL_Rect absCoord; //absolute coordinates of the camera (for now it's the same...)
    bool moving;
} camera;

typedef struct gTimer {
    Uint64 init_t; //initial time
    Uint64 current_t; //current time
} gTimer;

typedef struct game_context {
    SDL_Window *main_window;
    SDL_Renderer *renderer;
    
    //textures
    SDL_Texture *antoine_texture; //contient les sprites du perso antoine
    SDL_Texture *daniel_texture; //contient les sprites du perso antoine
    
    SDL_Texture *sanglier_texture; //sprites des NPCs
    SDL_Texture *assets_tiles; //contient les sprites du décor etc
    SDL_Texture *level_main_layer; //contient le décor principal du niveau (au niveau du joueur)
    SDL_Texture *level_overlay;
    SDL_Texture *hud;
    SDL_Texture *text_dialog;
    
    //sounds
    Mix_Chunk *jump;
    Mix_Chunk *hurt;
    Mix_Chunk *bump;
    Mix_Chunk *coin;
    
    camera cam;
    
    //number of tiles in the tileset
    size_t tsnb_x;
    size_t tsnb_y;
    
    //level variables
    character player;
    character *npcs;
    character *scpt_npcs; //scripted npcs
    interobj *objs;
    size_t nbTiles_x;
    size_t nbTiles_y;
    size_t nb_objs;
    size_t nb_npcs;
    size_t nb_scpt_npcs;
    int *main_tiles_grid;
    int *overlay_tiles_grid;
    
    //events
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
} game_context;

//typedef struct level {
//    
//} level;


#endif //TYPES_STRUCT_DEFS_H
