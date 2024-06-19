#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "enumerations.h"
#include "textures_fx.h"
#include "audio.h"
#include "npc.h"
#include "anim.h"
#include "script_engine.h"
#include "types_struct_defs.h"
#include "game_scripts.h"

enum fade {FADE_IN, FADE_OUT};

static const character daniel = {
    "Daniel",
        {
            BODY_TYPE_DANIEL,
            {109*TILE_SIZE,0*TILE_SIZE},
            {109*TILE_SIZE,0*TILE_SIZE,SPRITE_SIZE, SPRITE_SIZE},
            1,
            REQ_DIR_DOWN,
            true
        },
    1,
    0,
    CH_STATE_NONE,
    0,
    0,
    0,
    0
};

void sel_game_scripts(game_context *gctx, gameLevel *level)
{
    switch(level->ID)
    {
        case 1:
            if(level->active_scripts & (1U << 0))
                level1_script0(gctx, level);
            break;
    }
}

void level1_script0(game_context *gctx, gameLevel *level)
{
    static gTimer timer0;
    static script_states st = {0, 0, true};
    st.incs = 0;
    
    //script
    player_block_input(gctx, true, &st);
    draw_black_stripes(gctx, FADE_IN, &st);
    timerWait(&timer0, 1000, &st);
    chinit(gctx, &daniel, &st);
    timerWait(&timer0, 1000, &st);
    chwalk(gctx, "Daniel", (SDL_Point){101 * TILE_SIZE, 0}, &st);
    scriptDialog(gctx, L"Daniel : Attention!! Un monstre très dangereux se situe vers là-bas !", 35, &st);
    timerWait(&timer0, 1000, &st);
    scriptDialog(gctx, L"Daniel : Sois prudent...", 35, &st);
    timerWait(&timer0, 1000, &st);
    scriptDialogClear(gctx, &st);
    player_block_input(gctx, false, &st);
    draw_black_stripes(gctx, FADE_OUT, &st);
    toggle_script(level, 1U << 0, false, &st);
}

