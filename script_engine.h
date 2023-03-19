#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "stdio.h"
#include "wchar.h"
#include "types_struct_defs.h"

typedef struct script_state {
    int incs; //incremental state (reset at every frame, increment at every new function called in script
    int curs; //current state (kept in memory between frames)
    bool first_call; 
    //if it's the 1st call of a function (is set to true when the work
    //of a script function is done, to be used by the next one)
} script_states;

extern bool checkScriptState(script_states *st);
//extern void timerInit (gTimer *timer, script_states *st);
extern void timerWait (gTimer *timer, int wait_time, script_states *st);
extern void scriptDialog (game_context *gctx, wchar_t *wstr, int speed, script_states *st);
extern void scriptDialogClear (game_context *gctx, script_states *st);
extern int chinit(game_context *gctx, const character *ch, script_states *st);
extern int search_ch(game_context *gctx, const char *name, size_t *id);
extern int chdestroy(game_context *gctx, const char *name, script_states *st);
extern void chwalk(game_context *gctx, const char* name, SDL_Point dest, script_states *st);

#endif //SCRIPT_ENGINE_H
