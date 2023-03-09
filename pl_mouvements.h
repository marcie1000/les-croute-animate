#ifndef MOUVEMENTS_H
#define MOUVEMENTS_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "enumerations.h"
#include "types_struct_defs.h"

extern bool CheckPlayerOnTheGround(int, interobj *, character *, int*, int, int);
extern bool CheckPlayerJumpOnObj(interobj obj, character *player);
extern bool playerFall(int, interobj *, character *, int, bool*, int*, int, int);
extern bool checkCollision(SDL_Rect, SDL_Rect);
extern bool checkCollisionX(SDL_Rect, SDL_Rect, int);
extern bool checkAllCollisions(SDL_Rect, int, interobj *, int);
extern bool checkAllEndwallCollisions(SDL_Rect, int, interobj *, int);
extern bool checkCollisionEndwall(SDL_Rect, SDL_Rect);
extern bool updatePositionJump(int , interobj *, character *, int, bool*, int *, int, int);
extern bool updatePositionWalk(int, interobj *, character *, int, int, int *, int, int);
extern int updatePositionCam(int, interobj *, cam *, int);
extern void initPlayer(character *, bool);
extern void objCollisionSpecialActions(int, interobj **, character *, int **, int);
extern int checkCollisionSpecialAction(int, interobj **, int, character**, character *, int **, int** , int, int, int*);
extern int checkItemInLayer(int **tiles_grid, int subscript, SDL_Rect pl_col, SDL_Rect *tilebnd, 
                            int nb_tiles_x, int nb_tiles_y);
extern int checkItemCollision(character *player, int **main_tiles_grid, int **overlay_tiles_grid, 
                              int nb_tiles_x, int nb_tiles_y);

#endif //MOUVEMENTS_H
