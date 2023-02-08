#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <SDL2/SDL.h>

enum window_parameters { 
    SPRITE_SIZE = 16, 
    TILE_SIZE = 8,
    NB_SPRITES_X = 16,
    NB_SPRITES_Y = 9,
    NB_TILES_X = NB_SPRITES_X * 2,
    NB_TILES_Y = NB_SPRITES_Y * 2,
    NATIVE_WIDTH = SPRITE_SIZE * NB_SPRITES_X, 
    NATIVE_HEIGHT = SPRITE_SIZE * NB_SPRITES_Y,
    WIN_SCALE = 6,
    TEXTURE_TILES_SHIFTSIZE_X = 8 //en mini sprites
};

enum speeds {
    DEFAULT_ANIM_FRAMES = 8,
    NPC_ANIM_FRAME_LENGHT = 4,
    WALKING_ANIMATION_FRAMES = 2,
    NPC_ANIMATION_FRAMES = 4
};

enum others {
    PLAYER_COL_SHIFT = 5
};

enum player_requests {
    REQ_NONE,
    REQ_QUIT,
    
    REQ_DIR_LEFT,
    REQ_DIR_UP,
    REQ_DIR_RIGHT,
    REQ_DIR_DOWN,
    
    REQ_CAM_LEFT,
    REQ_CAM_RIGHT,
    
    REQ_JUMP
};

enum player_sprites {
    PL_SPRITE_DAN_BACK_STAND,
    PL_SPRITE_DAN_BACK_WALK,
    PL_SPRITE_ANT_BACK_STAND,
    PL_SPRITE_ANT_BACK_WALK,
    PL_SPRITE_DAN_FACE_STAND,
    PL_SPRITE_DAN_FACE_WALK,
    PL_SPRITE_ANT_FACE_STAND,
    PL_SPRITE_ANT_FACE_WALK,
    PL_SPRITE_DAN_SIDE_STAND,
    PL_SPRITE_DAN_SIDE_WALK,
    PL_SPRITE_ANT_SIDE_STAND,
    PL_SPRITE_ANT_SIDE_WALK
};

enum touches_keymap {
    TOUCHE_AR_LEFT = SDLK_LEFT,
    TOUCHE_AR_RIGHT = SDLK_RIGHT,
    TOUCHE_AR_UP = SDLK_UP,
    TOUCHE_AR_DOWN = SDLK_DOWN,
    
    TOUCHE_LEFT = SDLK_q,
    TOUCHE_RIGHT = SDLK_d,
    TOUCHE_UP = SDLK_z,
    TOUCHE_DOWN = SDLK_s,
    
    TOUCHE_JUMP = SDLK_SPACE
};

//enum character_type {
//
//};

enum interobj_types {
    BODY_TYPE_DANIEL,
    BODY_TYPE_ANTOINE,
    NPC_SANGLIER,
    IT_NONE,
    IT_ENDWALL,
    IT_WALL,
    IT_COIN
};

extern const char CROUTE_SPRITES_PNG[];
extern const char ASSETS_TILES_PNG[];
extern const char NPC_SPRITES_PNG[];
extern const char AUDIO_FILE_JUMP[];
extern const char AUDIO_FILE_HURT[];
extern const char AUDIO_FILE_BUMP[];
extern const char AUDIO_FILE_COIN[];
extern const char LEVEL_1_FILENAME[];
extern const float player_speed;
extern const SDL_Rect RECT_NULL;
extern const float gravity;
extern const float jump_init_speed;


#endif //ENUMERATIONS_H