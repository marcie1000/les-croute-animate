#ifndef TEXTURES_FX_H
#define TEXTURES_FX_H

#include <SDL2/SDL.h>
#include <stdbool.h>

extern int initTextures(SDL_Renderer *, SDL_Texture **, SDL_Texture **, SDL_Texture **);
extern int destroyTextures(SDL_Texture **, SDL_Texture **, SDL_Texture **, SDL_Texture **);
extern int copieTextureSurRender(SDL_Renderer *, SDL_Texture *, int, int, SDL_Rect, SDL_RendererFlip, int);
extern int loadPlayerSprite(SDL_Renderer *, SDL_Texture *, int, int, int, SDL_RendererFlip);
int loadLevelTiles(SDL_Texture **, SDL_Texture *, int *, int, int, SDL_Renderer *);
extern int chosePlayerSprite(int, bool, bool, int, SDL_RendererFlip *);
extern int selectLvlAssetsTile(int, SDL_Rect *);
extern int initLevelTextures(SDL_Texture **, SDL_Renderer *, int, int);
extern int choseNPCSprite(int, bool, int, SDL_RendererFlip *);
extern int loadNPCSprite(SDL_Renderer *, SDL_Texture *, int, int, int, SDL_RendererFlip);

#endif //TEXTURES_FX_H