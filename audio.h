#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

extern int loadAudio(Mix_Chunk **jump, Mix_Chunk **hurt, Mix_Chunk **bump, Mix_Chunk **coin);

#endif //AUDIO_H
