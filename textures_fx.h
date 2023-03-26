#ifndef TEXTURES_FX_H
#define TEXTURES_FX_H

#include <SDL.h>
#include <stdbool.h>
#include "types_struct_defs.h"

typedef struct text_format {
    const char *file;
    int size;
    SDL_Color color;
}text_format;

extern int initTextures(game_context *gctx);
extern void destroyTextures(game_context *gctx);
extern int copieTextureSurRender(SDL_Renderer *renderer, SDL_Texture *texture,
                                 int pos_x, int pos_y, SDL_Rect source, 
                                 SDL_RendererFlip flip, int scale);
extern int loadCharacterSprite(game_context *gctx, character charact,
                               int pos_x, int pos_y, int sprite_ID, SDL_RendererFlip flip);
int loadLevelTiles(SDL_Texture **dest_text, SDL_Texture *assets_tiles, int *level_tiles_grid, 
                   int nbt_x, int nbt_y, SDL_Renderer *renderer, int ts_nb_x, int ts_nb_y);
extern int choseCharacterSprite(character charact, SDL_RendererFlip *flip);
extern int selectLvlAssetsTile(int sprite_ID, SDL_Rect *source, int ts_nb_x, int ts_nb_y);
extern int initLevelTextures(SDL_Texture **level_main, SDL_Renderer *renderer, 
                             int nb_tuiles_x, int nb_tuiles_y);
extern int choseNPCSprite(character npc, SDL_RendererFlip *flip);
extern int loadNPCSprite(game_context *gctx, character npc,
                         int pos_x, int pos_y, int sprite_ID, SDL_RendererFlip flip);
extern int HUD_update(game_context *gctx);
extern SDL_Texture *createText(text_format tf, const char *string, SDL_Renderer *ren, Uint32 wrap);
extern int textDialogUpdate(game_context *gctx, wchar_t *wstr, int speed);
extern void textDialogClear(game_context *gctx);
extern void black_stripes(game_context *gctx, int anim_values);



#endif //TEXTURES_FX_H
