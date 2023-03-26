#ifndef TYPES_STRUCT_DEFS_H
#define TYPES_STRUCT_DEFS_H

#include <SDL.h>
#include <SDL_mixer.h>
#include "enumerations.h"

typedef struct flpoint {
    float x;
    float y;
} flpoint;

typedef struct interobj { //objets pouvant interragir avec le joueur
    //int ID;
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

typedef struct gameLevel {
    int ID;
    const char *filename;
    int nb_triggers;
    int nb_scripts;
    unsigned script_link[10]; 
    //eg. : script_link[0] = 1  ->   object n°0 which is a trigger links to the script n°1
    unsigned active_scripts;
} gameLevel;

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
    SDL_Texture *stripes_texture;
    
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
    int *main_tiles_array;
    int *overlay_tiles_array;
    
    gameLevel *levels;
    int nb_levels;
    
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
    
    bool mainloop_quit;
} game_context;

//typedef struct level {
//    
//} level;


#endif //TYPES_STRUCT_DEFS_H
