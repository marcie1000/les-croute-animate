#ifndef TYPES_STRUCT_DEFS_H
#define TYPES_STRUCT_DEFS_H

#include <SDL2/SDL.h>
#include "enumerations.h"

typedef struct flpoint {
    float x;
    float y;
}flpoint;

typedef struct interobj { //objets pouvant interragir avec le joueur
    int type; //le type : wall, pnj etc
    flpoint position;
    SDL_Rect collider;
    int pdv; //points de vie
    int direction; //direction l/r
    bool enabled; //l'objet est-il activé
}interobj;

typedef struct character { //pour tout ce qui bouge : joueur, NPC
    interobj obj;
    int puissance;
    int money;
    unsigned state;
    int frame_jump;
    int frame_fall;
    int frame_walk;
    int frame_hurt;
}character;

typedef struct camera {
    SDL_Rect texLoadSrc; //rect from the texture to be rendered
    //SDL_Rect absCoord; //absolute coordinates of the camera (for now it's the same...)
    bool moving;
}camera;

#endif //TYPES_STRUCT_DEFS_H
