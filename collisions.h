#ifndef MOUVEMENTS_H
#define MOUVEMENTS_H

#include <stdbool.h>
#include <SDL.h>

#include "enumerations.h"
#include "types_struct_defs.h"

extern bool CheckCharacterOnTheGround(int nb_objs, interobj *objs, character *ch, int *main_tiles_array, 
                                      int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y);
extern bool CheckCharacterJumpOnObj(interobj obj, character *ch);
extern bool CharacterFall(int nb_objs, interobj *objs, character *ch, 
                          int *main_tiles_array, int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y);
extern bool checkCollision(SDL_Rect a, SDL_Rect b);
extern bool checkCollisionX(SDL_Rect ch_coll, SDL_Rect b, int dir/*, bool debug*/);
extern bool checkAllCollisions(SDL_Rect a, int nb_objs, interobj *objs, int req);
extern bool checkCollisionJump(SDL_Rect a, SDL_Rect b);
extern bool updatePositionJump(int nb_objs, interobj *objs, character *ch, int frame_jump, bool *hurt_soundflag,
                               int *main_tiles_array, int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y);
extern bool updatePositionWalk(int nb_objs, interobj *objs, character *ch, int up_down, int left_right, 
                               int *main_tiles_array, int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y);
extern void initPlayer(character *ch, bool initmoney);
extern int checkCollisionSpecialAction( int nb_objs, interobj **objs, int nb_npcs, character **npcs,
                                        character *ch, int **main_tiles_array, int **overlay_tiles_array,
                                        int nb_tiles_x, int nb_tiles_y, gameLevel *level);
extern int checkItemInLayer(int **tiles_grid, int subscript, SDL_Rect pl_col, SDL_Rect *tilebnd, 
                            int nb_tiles_x, int nb_tiles_y);
extern int checkItemCollision(character *ch, int **main_tiles_array, int **overlay_tiles_array, 
                              int nb_tiles_x, int nb_tiles_y);
                              
                              
//extern bool checkAllEndwallCollisions(SDL_Rect a, int nb_objs, interobj *objs, int cam_leftRight);
//extern bool checkCollisionEndwall(SDL_Rect a, SDL_Rect b);
//extern int updatePositionCam(int, interobj *, camera *, int);
//extern void objCollisionSpecialActions(int id_obj, interobj **objs, character *ch, int **main_tiles_array, int nb_tiles_x);


#endif //MOUVEMENTS_H
