#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "audio.h"
#include "enumerations.h"

int loadAudio(Mix_Chunk **jump, Mix_Chunk **hurt, Mix_Chunk **bump, Mix_Chunk **coin)
{
    int status = EXIT_SUCCESS;
    *jump = Mix_LoadWAV(AUDIO_FILE_JUMP);
    if (NULL == jump)
    {
        fprintf(stderr, "erreur Mix_LoadWAV(%s) : %s\n", AUDIO_FILE_JUMP, Mix_GetError());
        status = EXIT_FAILURE;
    }
    
    *hurt = Mix_LoadWAV(AUDIO_FILE_HURT);
    if (NULL == hurt)
    {
        fprintf(stderr, "erreur Mix_LoadWAV(%s) : %s\n", AUDIO_FILE_HURT, Mix_GetError());
        status = EXIT_FAILURE;
    }
    
    *bump = Mix_LoadWAV(AUDIO_FILE_BUMP);
    if (NULL == hurt)
    {
        fprintf(stderr, "erreur Mix_LoadWAV(%s) : %s\n", AUDIO_FILE_BUMP, Mix_GetError());
        status = EXIT_FAILURE;
    }
    
    *coin = Mix_LoadWAV(AUDIO_FILE_COIN);
    if (NULL == coin)
    {
        fprintf(stderr, "erreur Mix_LoadWAV(%s) : %s\n", AUDIO_FILE_COIN, Mix_GetError());
        status = EXIT_FAILURE;
    }
    return status;
}