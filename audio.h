#ifndef AUDIO_H
#define AUDIO_H

#include <SDL.h>
#include <SDL_mixer.h>

extern int loadAudio(Mix_Chunk **jump, Mix_Chunk **hurt, Mix_Chunk **bump, Mix_Chunk **coin);

#endif //AUDIO_H
