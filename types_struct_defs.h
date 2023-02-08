#ifndef TYPES_STRUCT_DEFS_H
#define TYPES_STRUCT_DEFS_H

#include <SDL2/SDL.h>
#include "enumerations.h"

typedef struct flpoint_struct {
    float x;
    float y;
}flpoint;

typedef struct interobject_struct { //objets pouvant interragir avec le joueur
    int type; //le type : wall, pnj etc
    flpoint position;
    SDL_Rect collider;
    int pdv; //points de vie
    int direction; //direction l/r
    bool enabled; //l'objet est-il activé
}interobj;

typedef struct character_struct { //pour tout ce qui bouge : joueur, NPC
    interobj obj;
    int puissance;
    bool walking;
    bool jumping;
    bool falling;
    int money;
}character;

typedef struct character_struct character;

typedef struct camera_struct {
    SDL_Rect texLoadSrc;
    SDL_Rect absCoord;
    bool moving;
}cam;

#endif //TYPES_STRUCT_DEFS_H