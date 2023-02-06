#ifndef MOUVEMENTS_H
#define MOUVEMENTS_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "enumerations.h"
#include "types_struct_defs.h"

extern bool CheckPlayerOnTheGround(int, interobj *, character *);
extern bool playerFall(int, interobj *, character *, int, bool*);
extern bool checkCollision(SDL_Rect, SDL_Rect);
extern bool checkCollisionX(SDL_Rect, SDL_Rect, int);
extern bool checkAllCollisions(SDL_Rect, int, interobj *, int);
extern bool checkAllEndwallCollisions(SDL_Rect, int, interobj *, int);
extern bool checkCollisionEndwall(SDL_Rect, SDL_Rect);
extern bool updatePositionJump(int , interobj *, character *, int, bool*);
extern bool updatePositionWalk(int, interobj *, character *, int, int);
extern int updatePositionCam(int, interobj *, cam *, int);
extern void initPlayer(character *);

#endif //MOUVEMENTS_H