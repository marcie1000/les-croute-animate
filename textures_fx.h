#ifndef TEXTURES_FX_H
#define TEXTURES_FX_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "types_struct_defs.h"

typedef struct text_format {
    const char *file;
    int size;
    SDL_Color color;
}text_format;

extern int initTextures(SDL_Renderer *, SDL_Texture **, SDL_Texture**, SDL_Texture **, SDL_Texture **, int*, int*);
extern int destroyTextures(SDL_Texture **, SDL_Texture **, SDL_Texture **, SDL_Texture **);
extern int copieTextureSurRender(SDL_Renderer *, SDL_Texture *, int, int, SDL_Rect, SDL_RendererFlip, int);
extern int loadPlayerSprite(SDL_Renderer *, SDL_Texture *, int, int, int, SDL_RendererFlip);
int loadLevelTiles(SDL_Texture **, SDL_Texture *, int *, int, int, SDL_Renderer *, int, int);
extern int chosePlayerSprite(int, bool, bool, int, SDL_RendererFlip *);
extern int selectLvlAssetsTile(int, SDL_Rect *, int, int);
extern int initLevelTextures(SDL_Texture **, SDL_Renderer *, int, int);
extern int choseNPCSprite(int, bool, int, SDL_RendererFlip *);
extern int loadNPCSprite(SDL_Renderer *, SDL_Texture *, int, int, int, SDL_RendererFlip);
extern int HUD_update(SDL_Renderer *renderer, SDL_Texture **hud, SDL_Texture *assets_tiles,
                      int ts_nb_x, int ts_nb_y, character player);
extern SDL_Texture *createText(text_format tf, const char *string, SDL_Renderer *ren);


#endif //TEXTURES_FX_H
