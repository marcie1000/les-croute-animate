#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <SDL.h>

enum window_parameters { 
    SPRITE_SIZE = 16, 
    TILE_SIZE = 8,
    NB_SPRITES_X = 16, //screen size X by sprites
    NB_SPRITES_Y = 9, //screen size Y by sprites
    NB_TILES_X = NB_SPRITES_X * 2, //screen size X by tiles
    NB_TILES_Y = NB_SPRITES_Y * 2, //screen size Y by tiles
    NATIVE_WIDTH = SPRITE_SIZE * NB_SPRITES_X, 
    NATIVE_HEIGHT = SPRITE_SIZE * NB_SPRITES_Y,
    WIN_SCALE = 5,
    TEXTURE_TILES_SHIFTSIZE_X = 8 //en mini sprites
};

enum speeds {
    DEFAULT_ANIM_FRAMES = 8,
    NPC_ANIM_FRAME_LENGHT = 4,
    WALKING_ANIMATION_FRAMES = 2,
    NPC_ANIMATION_FRAMES = 4
};

enum others {
    PLAYER_COL_SHIFT = 5,
    MAX_TILE_WALL = 36,
    PLAYER_MAX_LIFE = 3
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

enum touches_keymap {
    
    TOUCHE_LEFT = SDL_SCANCODE_A,
    TOUCHE_RIGHT = SDL_SCANCODE_D,
    TOUCHE_UP = SDL_SCANCODE_W,
    TOUCHE_DOWN = SDL_SCANCODE_S,
    
    TOUCHE_JUMP = SDL_SCANCODE_SPACE
};


enum interobj_types {
    BODY_TYPE_DANIEL,
    BODY_TYPE_ANTOINE,
    NPC_SANGLIER,
    IT_NONE,
    IT_WALL,
    IT_TRIGGER
};

enum items_tiles {
    ITEM_COIN = 123,
    ITEM_HEART = 134,
    ITEM_EMPTY_HEART = 124
};

enum script_functions_returns {
    S_SUCCESS_IN_PROGRESS = EXIT_SUCCESS,   //work in progress
    S_FAILURE = EXIT_FAILURE,               //failure
    S_SUCCESS_DONE                          //work is done
};

//SPECIAL ACTIONS
#define SP_AC_NONE          (0U     )
#define SP_AC_EARN_COIN     (1U << 0)
#define SP_AC_EARN_HEART    (1U << 1)
#define SP_AC_HURT          (1U << 2)
#define SP_AC_NPC_HURT      (1U << 3)

//CHARACTER STATES
#define CH_STATE_NONE           (0U     )
#define CH_STATE_WALKING        (1U << 0)
#define CH_STATE_FALLING        (1U << 1)
#define CH_STATE_JUMPING        (1U << 2)
#define CH_STATE_HURT           (1U << 3)
#define CH_STATE_MOVED          (1U << 4)
#define CH_STATE_BLOCK_INPUT    (1U << 5)

extern const char ANTOINE_SPRITES_PNG[];
extern const char DANIEL_SPRITES_PNG[];
extern const char ASSETS_TILES_PNG[];
extern const char SANGLIER_SPRITES_PNG[];
extern const char AUDIO_FILE_JUMP[];
extern const char AUDIO_FILE_HURT[];
extern const char AUDIO_FILE_BUMP[];
extern const char AUDIO_FILE_COIN[];
extern const char LEVEL_1_FILENAME[];
extern const char TTF_FONT_FILENAME[];
extern const float player_speed;
extern const SDL_Rect RECT_NULL;
extern const float gravity;
extern const float jump_init_speed;


#endif //ENUMERATIONS_H
