#ifndef SWITCH_EVENTS_H
#define SWITCH_EVENTS_H

#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

extern void functionSwitchEvent(SDL_Event e, int *requete, int *left_right, int *up_down, bool *jump_ended);

#endif //SWITCH_EVENTS_H
