#ifndef SDL_INIT_AND_QUIT_H
#define SDL_INIT_AND_QUIT_H

#include <SDL.h>

extern int initSDL(SDL_Renderer **renderer, SDL_Window **window);
extern int quitSDL(SDL_Renderer **renderer, SDL_Window **window);

#endif //SDL_INIT_AND_QUIT_H

