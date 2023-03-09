
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "enumerations.h"

void fonctionSwitchEvent(SDL_Event e, int *requete, int *left_right, int *up_down, bool *jump_ended /*, int *cam_leftRight*/)
{
    
    switch(e.type)
    {
        case (SDL_QUIT):
            *requete = REQ_QUIT;
            break;
        case (SDL_KEYDOWN):
            switch(e.key.keysym.sym)
            {
                case (TOUCHE_LEFT):
                    *requete = REQ_DIR_LEFT;
                    *left_right = -1;
                    break;
                case (TOUCHE_RIGHT):
                    *requete = REQ_DIR_RIGHT;
                    *left_right = +1;
                    break;
                case (TOUCHE_UP):
                    *requete = REQ_DIR_UP;
                    *up_down = +1;
                    break;
                case (TOUCHE_DOWN):
                    *requete = REQ_DIR_DOWN;
                    *up_down = -1;
                    break;
                case (TOUCHE_JUMP):
                    if (*jump_ended)
                    {
                        *requete = REQ_JUMP;
                    }
                    else
                        *requete = REQ_NONE;
                    break;
//                case (TOUCHE_AR_LEFT):
//                    *requete = REQ_CAM_LEFT;
//                    *cam_leftRight = -1;
//                    break;
//                case (TOUCHE_AR_RIGHT):
//                    *requete = REQ_CAM_RIGHT;
//                    *cam_leftRight = +1;
//                    break;
            }
            break;
        case (SDL_KEYUP):
            //srequete = REQ_NONE;
            //si la touche relachée = la direction actuelle
            switch(e.key.keysym.sym)
            {
                case (TOUCHE_LEFT):
                    *requete = REQ_NONE;
                    if(*left_right == -1)
                        *left_right = 0;
                    break;
                case (TOUCHE_RIGHT):
                    *requete = REQ_NONE;
                    if(*left_right == +1)
                        *left_right = 0;
                    break;
                case (TOUCHE_UP):
                    *requete = REQ_NONE;
                    if(*up_down == +1)
                        *up_down = 0;
                    break;
                case (TOUCHE_DOWN):
                    *requete = REQ_NONE;
                    if(*up_down == -1)
                        *up_down = 0;
                    break;
                case (TOUCHE_JUMP):
                    *requete = REQ_NONE;
                    *jump_ended = true;
                    break;
                    
//                case (TOUCHE_AR_LEFT):
//                    *requete = REQ_NONE;
//                    if(*cam_leftRight == -1)
//                        *cam_leftRight = 0;
//                    break;
//                case (TOUCHE_AR_RIGHT):
//                    *requete = REQ_NONE;
//                    if(*cam_leftRight == +1)
//                        *cam_leftRight = 0;
//                    break;
            }
            break;
    } //end switch (e.type)
}
