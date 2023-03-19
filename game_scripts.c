#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "enumerations.h"
#include "textures_fx.h"
#include "audio.h"
#include "npc.h"
#include "anim.h"
#include "script_engine.h"
#include "types_struct_defs.h"

static const character daniel = {
    "Daniel",
        {
            BODY_TYPE_DANIEL,
            {8*TILE_SIZE,0*TILE_SIZE},
            {8*TILE_SIZE,0*TILE_SIZE,SPRITE_SIZE, SPRITE_SIZE},
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

void test_script(game_context *gctx)
{
    static gTimer timer0;
    static script_states st = {0, 0, true};
    st.incs = 0;
    
    //script
    timerWait(&timer0, 1000, &st);
    chinit(gctx, &daniel, &st);
    timerWait(&timer0, 1000, &st);
    chwalk(gctx, "Daniel", (SDL_Point){30, 0}, &st);
    scriptDialog(gctx, L"Daniel : Oh Antoine c'est toi!!!\nAlors ça va mec ?? \"SDK\" comme\ndisent les jeunes ?? ^^", 45, &st);
    timerWait(&timer0, 2000, &st);
    scriptDialog(gctx, L"Daniel : Haha pourquoi tu réponds pas trop bizarre sa bug nn? ^^", 45, &st);
    timerWait(&timer0, 2000, &st);
    scriptDialogClear(gctx, &st);
}

