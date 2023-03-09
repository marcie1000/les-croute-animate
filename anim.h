#ifndef ANIM_H
#define ANIM_H

#include <stdbool.h>
#include "types_struct_defs.h"

extern void anim_npc(int nb_npcs, int nb_objs, character *npcs, interobj *objs, int *main_tiles_grid, 
                     int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag);
extern void anim_main_character(int nb_objs, character *player, interobj *objs, int *main_tiles_grid, 
                                int nb_tuiles_x, int nb_tuiles_y, bool *hurt_soundflag, bool *bump_soundflag, 
                                bool *player_moved, int up_down, int left_right);
extern void anim_camera(character player, cam *camera, bool player_moved, int left_right, int nb_tuiles_x, int *cam_leftRight);

#endif //ANIM_H
