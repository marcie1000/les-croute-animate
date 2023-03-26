#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "enumerations.h"
#include "sdl_init_and_quit.h"

int initSDL(SDL_Renderer **renderer, SDL_Window **window)
{
    int status = EXIT_FAILURE;
    
    //init sdl
    if (0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Erreur SDL_init : %s", SDL_GetError());
        return status;
    }
    //init SDL_image
    if (0 == IMG_Init(IMG_INIT_PNG))
    {
        fprintf(stderr, "Erreur IMG_Init : %s", IMG_GetError());
        return status;
    }
    //initialise SDL_mixer
    if(0 != Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024))
    {
        fprintf(stderr, "erreur Mix_OpenAudio : %s\n", Mix_GetError());
        return status;
    }
    //initialize SDL_ttf
    if(0 != TTF_Init())
    {
        fprintf(stderr, "Erreur TTF_Init : %s", TTF_GetError());
        return EXIT_FAILURE;
    }
    
    *window = SDL_CreateWindow("Les Croute", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              NATIVE_WIDTH*WIN_SCALE, NATIVE_HEIGHT*WIN_SCALE, SDL_WINDOW_SHOWN);
    if(NULL == *window)
    {
        fprintf(stderr, "erreur SDL_CreateWindow : %s", SDL_GetError());
        return status;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(NULL == *renderer)
    {
        fprintf(stderr, "erreur SDL_CreateRenderer : %s", SDL_GetError());
        return status;
    }
    
    if(0 != SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND))
    {
        fprintf(stderr, "erreur SDL_SetRenderDrawBlendMode : %s", SDL_GetError());
        return status;
    }
    
    status = EXIT_SUCCESS;
    return status;
}

int quitSDL(SDL_Renderer **renderer, SDL_Window **window)
{
    int status = EXIT_FAILURE;
    
    if(NULL != *renderer)
        SDL_DestroyRenderer(*renderer);
    if(NULL != *window)
        SDL_DestroyWindow(*window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    
    status = EXIT_SUCCESS;
    return status;
}
