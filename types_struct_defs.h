#ifndef TYPES_STRUCT_DEFS_H
#define TYPES_STRUCT_DEFS_H

#include <SDL2/SDL.h>
#include "enumerations.h"

typedef struct interobject_struct { //objets pouvant interragir avec le joueur
    int type; //le type : wall, pnj etc
    SDL_Point position;
    SDL_Rect collider;
    int pdv; //points de vie
    bool enabled; //l'objet est-il activé
}interobj;

typedef struct flpoint_struct {
    float x;
    float y;
}flpoint;

typedef struct character_struct { //pour tout ce qui bouge : joueur, NPC
    enum character_type body_type;
    flpoint position;
    SDL_Rect collider;
    int pt_de_vie;
    int puissance;
    bool walking;
    bool jumping;
    bool falling;
    int direction;
    int money;
}character;

typedef struct character_struct character;

typedef struct camera_struct {
    SDL_Rect texLoadSrc;
    SDL_Rect absCoord;
    bool moving;
}cam;

#endif //TYPES_STRUCT_DEFS_H