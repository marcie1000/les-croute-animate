#ifndef ANIM_H
#define ANIM_H

#include <stdbool.h>
#include <SDL.h>
#include "types_struct_defs.h"

extern void anim_npc(int nb_npcs, int nb_objs, character *npcs, interobj *objs, int *main_tiles_array, 
                     int *overlay_tiles_array, int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag);
extern void anim_main_character(game_context *gctx);
extern void anim_camera(character player, camera *cam, int left_right, int nb_tuiles_x, int *cam_leftRight);
extern void flag_assign(unsigned *var, unsigned flag, bool condition);
extern void anim_scripted_npc(game_context *gctx, character *ch);

#endif //ANIM_H
