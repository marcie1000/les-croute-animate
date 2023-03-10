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

extern int initTextures(SDL_Renderer *renderer, SDL_Texture **croute_texture, 
                        SDL_Texture **assets_tiles, SDL_Texture **npc_texture,
                        SDL_Texture **hud, int *ts_nb_x, int *ts_nb_y);
extern int destroyTextures(SDL_Texture **croute_texture, SDL_Texture **assets_tiles, 
                           SDL_Texture **level_main, SDL_Texture **npc_texture, 
                           SDL_Texture **level_overlay, SDL_Texture **hud);
extern int copieTextureSurRender(SDL_Renderer *renderer, SDL_Texture *texture,
                                 int pos_x, int pos_y, SDL_Rect source, 
                                 SDL_RendererFlip flip, int scale);
extern int loadPlayerSprite(SDL_Renderer *renderer, SDL_Texture *croute_texture,
                            int pos_x, int pos_y, int sprite_ID, SDL_RendererFlip flip);
int loadLevelTiles(SDL_Texture **dest_text, SDL_Texture *assets_tiles, int *level_tiles_grid, 
                   int nbt_x, int nbt_y, SDL_Renderer *renderer, int ts_nb_x, int ts_nb_y);
extern int chosePlayerSprite(int direction, bool player_moving, bool playerID, 
                             int rapport_frame, SDL_RendererFlip *flip);
extern int selectLvlAssetsTile(int sprite_ID, SDL_Rect *source, int ts_nb_x, int ts_nb_y);
extern int initLevelTextures(SDL_Texture **level_main, SDL_Renderer *renderer, 
                             int nb_tuiles_x, int nb_tuiles_y);
extern int choseNPCSprite(int direction, bool moving, int rapport_frame, SDL_RendererFlip *flip);
extern int loadNPCSprite(SDL_Renderer *renderer, SDL_Texture *npc_texture,
                     int pos_x, int pos_y, int sprite_ID, SDL_RendererFlip flip);
extern int HUD_update(SDL_Renderer *renderer, SDL_Texture **hud, SDL_Texture *assets_tiles,
                      int ts_nb_x, int ts_nb_y, character player);
extern SDL_Texture *createText(text_format tf, const char *string, SDL_Renderer *ren);


#endif //TEXTURES_FX_H
