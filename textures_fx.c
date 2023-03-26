#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "textures_fx.h"
#include "enumerations.h"
#include "types_struct_defs.h"
#include "script_engine.h"

static text_format hud_tf = {
    TTF_FONT_FILENAME, //font
    15, //size
    {0,0,0,255} //color
};

static text_format dialog_tf = {
    TTF_FONT_FILENAME, //font
    15, //size
    {255,255,255,255} //color
};

int initTextures(game_context *gctx)
{
    int status = EXIT_FAILURE;
    
    gctx->antoine_texture = IMG_LoadTexture(gctx->renderer, ANTOINE_SPRITES_PNG);
    if(NULL == gctx->antoine_texture)
    {
        fprintf(stderr, "erreur IMG_LoadTexture() antoine_texture : %s\n", IMG_GetError());
        return status;
    }
    
    gctx->daniel_texture = IMG_LoadTexture(gctx->renderer, DANIEL_SPRITES_PNG);
    if(NULL == gctx->daniel_texture)
    {
        fprintf(stderr, "erreur IMG_LoadTexture() daniel_texture : %s\n", IMG_GetError());
        return status;
    }
    
    gctx->assets_tiles = IMG_LoadTexture(gctx->renderer, ASSETS_TILES_PNG);
    if(NULL == gctx->assets_tiles)
    {
        fprintf(stderr, "erreur IMG_LoadTexture() assets_tiles : %s\n", IMG_GetError());
        return status;
    }
    SDL_Point bounds;
    if(0 != SDL_QueryTexture(gctx->assets_tiles, NULL, NULL, &bounds.x, &bounds.y))
    {
        fprintf(stderr, "error SDL_QueryTexture gctx->assets_tiles in fct initTextures: %s\n", SDL_GetError());
        return status;
    }
    gctx->tsnb_x = bounds.x / 8;
    gctx->tsnb_y = bounds.y / 8;
    
    gctx->sanglier_texture = IMG_LoadTexture(gctx->renderer, SANGLIER_SPRITES_PNG);
    if(NULL == gctx->sanglier_texture)
    {
        fprintf(stderr, "erreur IMG_LoadTexture() sanglier_texture : %s\n", IMG_GetError());
        return status;
    }
    
    gctx->hud = SDL_CreateTexture(gctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                             NB_TILES_X * TILE_SIZE, 2 * TILE_SIZE);
    if(NULL == gctx->hud)
    {
        fprintf(stderr, "error SDL_CreateTexture HUD_init in function initTextures : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetTextureBlendMode(gctx->hud, SDL_BLENDMODE_BLEND);
    
    gctx->stripes_texture = SDL_CreateTexture(gctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                              NB_TILES_X * TILE_SIZE, NB_TILES_Y * TILE_SIZE);
    if(NULL == gctx->stripes_texture)
    {
        fprintf(stderr, "error SDL_CreateTexture stripes_texture in function initTextures : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetTextureBlendMode(gctx->stripes_texture, SDL_BLENDMODE_BLEND);
    
    gctx->text_dialog = SDL_CreateTexture(gctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                          NB_TILES_X * TILE_SIZE, NB_TILES_Y * TILE_SIZE);
    if(NULL == gctx->text_dialog)
    {
        fprintf(stderr, "error SDL_CreateTexture gctx->text_dialog in function initTextures: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetTextureBlendMode(gctx->text_dialog, SDL_BLENDMODE_BLEND);
    
    status = EXIT_SUCCESS;
    return status;
}

void destroyTextures(game_context *gctx)
{
//    int status = EXIT_FAILURE;
    
    if(NULL != gctx->sanglier_texture)
        SDL_DestroyTexture(gctx->sanglier_texture);
    if(NULL != gctx->antoine_texture)
        SDL_DestroyTexture(gctx->antoine_texture);
    if(NULL != gctx->daniel_texture)
        SDL_DestroyTexture(gctx->daniel_texture);
    if(NULL != gctx->assets_tiles)
        SDL_DestroyTexture(gctx->assets_tiles);
    if(NULL != gctx->level_main_layer)
        SDL_DestroyTexture(gctx->level_main_layer);
    if(NULL != gctx->level_overlay)
        SDL_DestroyTexture(gctx->level_overlay);
    if(NULL != gctx->hud)
        SDL_DestroyTexture(gctx->hud);
    if(NULL != gctx->text_dialog)
        SDL_DestroyTexture(gctx->text_dialog);
    if(NULL != gctx->stripes_texture)
        SDL_DestroyTexture(gctx->stripes_texture);
    
//    status = EXIT_SUCCESS;
//    return status;
}


int copieTextureSurRender(SDL_Renderer *renderer, SDL_Texture *texture,
                          int pos_x, int pos_y, SDL_Rect source, 
                          SDL_RendererFlip flip, int scale)
{
    int status = EXIT_FAILURE;
    if(source.w == 0 || source.h == 0)
    //si ces valeurs sont à 0 on rend la texture entière
    {
        if(0 != SDL_QueryTexture(texture, NULL, NULL, &(source.w), &(source.h)))
        {
            fprintf(stderr, "erreur SDL_QueryTexture() : %s", SDL_GetError());
            return status;
        }
    }
    //on agrandit à la taille de la fenetre
    SDL_Rect dst = {pos_x*scale, pos_y*scale, source.w * scale, source.h * scale};
    if(0 != SDL_RenderCopyEx(renderer, texture, &source, &dst, 0, NULL, flip))
    {
        fprintf(stderr, "erreur SDL_RenderCopy() : %s", SDL_GetError());
        return status;
    }
    status = EXIT_SUCCESS;
    return status;
}



int loadCharacterSprite(game_context *gctx, character charact,
                        int pos_x, int pos_y, int sprite_ID, SDL_RendererFlip flip)
{
    int status = EXIT_FAILURE;
    if (sprite_ID < 1 || sprite_ID > 12)
    {
        fprintf(stderr, "Erreur loadCharacterSprite() : sprite_ID valeur interdite (%d)\n", sprite_ID);
        return status;
    }
    //calcul de l'emplacement de la sprite sur la texture
    int ligne = sprite_ID / 2;
    int colonne = sprite_ID % 2;
    colonne--;
    if(colonne < 0)
    {
        ligne--;
        colonne = 1;
    }
    SDL_Rect source = {colonne*SPRITE_SIZE, ligne*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    int ret_value;
    switch(charact.obj.type)
    {
        case BODY_TYPE_ANTOINE:
            ret_value = copieTextureSurRender(gctx->renderer, gctx->antoine_texture, 
                                              pos_x, pos_y, source, flip, WIN_SCALE);
            break;
        case BODY_TYPE_DANIEL:
            ret_value = copieTextureSurRender(gctx->renderer, gctx->daniel_texture, 
                                              pos_x, pos_y, source, flip, WIN_SCALE);
            break;
        default:
            ret_value = EXIT_FAILURE;
            break;
    }
    if (0 != ret_value)
        return status;
    
    status = EXIT_SUCCESS;
    return status;
}

int initLevelTextures(SDL_Texture **level_main, SDL_Renderer *renderer, int nbt_x, int nbt_y)
//crée la texture du niveau
{
    int largeur_texture = nbt_x*TILE_SIZE;
    *level_main = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
                                    largeur_texture, nbt_y * TILE_SIZE);
    if(NULL == *level_main)
    {
        fprintf(stderr, "erreur SDL_CreateTexture level_main : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


int HUD_update(game_context *gctx)
{
    static int money;
    static int life;
    bool changes = true;
    
    if(money != gctx->player.money)
    {
        changes = true;
        money = gctx->player.money;
    }
    else if(life != gctx->player.obj.life)
    {
        changes = true;
        life = gctx->player.obj.life;
    }
    else 
        changes = false;
    
    if(changes)
    {
        SDL_SetRenderTarget(gctx->renderer, gctx->hud);
        SDL_SetRenderDrawColor(gctx->renderer, 0,0,0,0);
        SDL_RenderClear(gctx->renderer);
        
        int posX = 1; 
        
        //money
        SDL_Rect src;
        selectLvlAssetsTile(ITEM_COIN, &src, gctx->tsnb_x, gctx->tsnb_y);
        copieTextureSurRender(gctx->renderer, gctx->assets_tiles, posX, 2, src, SDL_FLIP_NONE, 1);
        //money amount text
        char buf[50];
        sprintf(buf, "%d", gctx->player.money);
        SDL_Texture *tmp = createText(hud_tf, buf, gctx->renderer, 200);
        if(NULL == tmp)
            return EXIT_FAILURE;
        posX += 3+TILE_SIZE;
        copieTextureSurRender(gctx->renderer, tmp, posX, -1, RECT_NULL, SDL_FLIP_NONE, 1);
        SDL_DestroyTexture(tmp);
        
        //life
        posX += 2*TILE_SIZE;
        for(int i = 1; i<=PLAYER_MAX_LIFE; i++)
        {
            if(gctx->player.obj.life >= i)
                selectLvlAssetsTile(ITEM_HEART, &src, gctx->tsnb_x, gctx->tsnb_y);
            else
                selectLvlAssetsTile(ITEM_EMPTY_HEART, &src, gctx->tsnb_x, gctx->tsnb_y);
            copieTextureSurRender(gctx->renderer, gctx->assets_tiles, posX, 2, src, SDL_FLIP_NONE, 1);
            posX += TILE_SIZE + 1;
        }
        
        SDL_SetRenderTarget(gctx->renderer, NULL);
    }
    
    return 0;
}

int textDialogUpdate(game_context *gctx, wchar_t *wstr, int speed)
{
    const int max_len = 254;
    static wchar_t buf[255] = L"\0";
    static int len;
    static wchar_t showed_text[255] = L"\0"; //contains the part of the text showed on the screen 
    static int showed_len = 0;
    static bool render = true;
    
    static gTimer txt_timer;
    if(wstr != NULL)
    {
        render = true;
        len = wcslen(wstr);
        //avoid overflow
        if(len >= max_len)
        {
            len = max_len;
            buf[max_len] = L'\0';
        }
        wcsncpy(buf, wstr, len);
        buf[max_len] = L'\0';
        txt_timer.init_t = SDL_GetTicks64(); //timer init
        showed_text[max_len] = L'\0'; //secure the end of the wstring
        showed_text[0] = L'\0'; //clean the wstring
        showed_len = 0;
        if(speed == 0)
        {
            wcsncpy(showed_text, buf, max_len);
            showed_len = len;
        }
        
    }

    txt_timer.current_t = SDL_GetTicks64();
    while ((txt_timer.current_t - txt_timer.init_t >= speed * showed_len) && 
           (showed_len < len))
    {
        //wide characters
        showed_len++;
        wcsncat(showed_text, buf, 1);
        wmemmove(buf, (wchar_t*)&buf[1], len - showed_len);
    }
    
    if(!render)
        return S_SUCCESS_DONE;
    
    static SDL_Rect text_rect = {
        ((NB_TILES_X - 26) / 2) * TILE_SIZE,
        (NB_TILES_Y - 5) * TILE_SIZE - 4,
        26 * TILE_SIZE,
        5 * TILE_SIZE + 3
    };
    SDL_Texture *tmp = NULL;
    char c[255];
    c[max_len] = '\0';
    wcstombs(c, showed_text, max_len);
    tmp = createText(dialog_tf, c, gctx->renderer, text_rect.w - 6);
    if(tmp == NULL)
        return EXIT_FAILURE;
    
    SDL_SetRenderTarget(gctx->renderer, gctx->text_dialog);
    SDL_SetRenderDrawColor(gctx->renderer, 0,0,0,0);
    SDL_RenderClear(gctx->renderer);
    
    SDL_SetRenderDrawColor(gctx->renderer, 0, 0, 0, 176);

    SDL_RenderFillRect(gctx->renderer, &text_rect);
    SDL_SetRenderDrawColor(gctx->renderer, 255,255,255,255);
    SDL_RenderDrawRect(gctx->renderer, &text_rect);
    copieTextureSurRender(gctx->renderer, tmp, text_rect.x + 3, text_rect.y, RECT_NULL, SDL_FLIP_NONE, 1);
    SDL_SetRenderTarget(gctx->renderer, NULL);
    
    SDL_DestroyTexture(tmp);
    
    if(showed_len == len)
    {
        render = false;
        return S_SUCCESS_DONE;
    }
    
    return S_SUCCESS_IN_PROGRESS;
}

void textDialogClear(game_context *gctx)
{
    SDL_SetRenderTarget(gctx->renderer, gctx->text_dialog);
    SDL_SetRenderDrawColor(gctx->renderer, 0, 0, 0, 0);
    SDL_RenderClear(gctx->renderer);
    SDL_SetRenderTarget(gctx->renderer, NULL);
}

SDL_Texture *createText(text_format tf, const char *string, SDL_Renderer *ren, Uint32 wrap)
{
    TTF_Font *font = NULL;
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;
    font = TTF_OpenFont(tf.file, tf.size);
    if (NULL == font)
    {
        fprintf(stderr, "Erreur TTF_OpenFont : %s", TTF_GetError());
        return NULL;
    }
    surface = TTF_RenderUTF8_Solid_Wrapped(font, string, tf.color, wrap);
    TTF_CloseFont(font);
    if (NULL == surface)
    {
        fprintf(stderr, "Erreur TTF_RenderUTF8_Solid : %s", TTF_GetError());
        return NULL;
    }
    texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_FreeSurface(surface);
    if (NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
        return NULL;
    }
    return texture;
}

int loadLevelTiles(SDL_Texture **dest_text, SDL_Texture *assets_tiles, int *level_tiles_grid, 
                   int nbt_x, int nbt_y, SDL_Renderer *renderer, int ts_nb_x, int ts_nb_y)
//remplit la texture dest_text avec les bonnes tuiles
{
    SDL_Rect source;
    //TEXTURE_SHIFTSIZE_X sert à créer l'espace nécesssaire à la mise à jour de la texture hors écran
    //pour le scrolling
    SDL_SetTextureBlendMode(*dest_text, SDL_BLENDMODE_BLEND);
    if(NULL == *dest_text)
    {
        fprintf(stderr, "erreur SDL_CreateTexture in function loadLevelTiles : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetRenderTarget(renderer, *dest_text);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_RenderClear(renderer);
    
    for(int i=0; i<nbt_y; i++)
    {
        for(int j=0; j<(nbt_x/*NB_TILES_X + TEXTURE_TILES_SHIFTSIZE_X*/); j++)
        {
            if( 0 != level_tiles_grid[i * nbt_x + j] )
            {
                //if(i==7 && j==7)
                    //printf("test\n");
                if( 0!= selectLvlAssetsTile(level_tiles_grid[i * nbt_x + j], &source, ts_nb_x, ts_nb_y) )
                    return EXIT_FAILURE;
                if (0 != copieTextureSurRender(renderer, assets_tiles, j * TILE_SIZE, 
                    i * TILE_SIZE, source, SDL_FLIP_NONE, 1))
                    return EXIT_FAILURE;
            }
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
//    SDL_SetTextureAlphaMod(*dest_text, 100);
    return EXIT_SUCCESS;
}

int selectLvlAssetsTile(int sprite_ID, SDL_Rect *source, int ts_nb_x, int ts_nb_y)
{
    int status = EXIT_FAILURE;
    if (sprite_ID < 1 || sprite_ID > (ts_nb_x * ts_nb_y))
    {
        fprintf(stderr, "Erreur selectLvlAssetsTile() : sprite_ID valeur interdite (%d)\n", sprite_ID);
        return status;
    }
    //calcul de l'emplacement de la tile sur la texture
    int ligne = sprite_ID / ts_nb_x;
    int colonne = sprite_ID % ts_nb_x;
    colonne--;
    if(colonne < 0)
    {
        ligne--;
        colonne = ts_nb_x-1;
    }    
    source->x = colonne*TILE_SIZE;
    source->y = ligne*TILE_SIZE;
    source->w = TILE_SIZE;
    source->h = TILE_SIZE;
    
    status = EXIT_SUCCESS;
    return status;
}

int choseCharacterSprite(character charact, SDL_RendererFlip *flip)
//donne le bon identifiant de sprite en fonction des 3 paramètres
{
    int spriteID;
    *flip = SDL_FLIP_NONE;
    //indique si l'animation du mvt se fait en inversant l'image ou non
    //(faux pour les directions gauche et droite car leur animation se 
    //fait en alternant la sprite walking et la sprite stand)
    bool mouvement_inverse = true; 
    switch (charact.obj.direction)
    {
        case REQ_DIR_RIGHT:
            *flip = SDL_FLIP_HORIZONTAL; //inverse la side sprite pour la diriger à droite
        case REQ_DIR_LEFT:
            spriteID = 5; //va à la dernière ligne de la spritesheet (side)
            mouvement_inverse = false;
            break;
        case REQ_DIR_UP:
            spriteID = 1; //va à la première ligne de la spritesheet (up)
            break;
        case REQ_DIR_DOWN:
            spriteID = 3; //va à la deuxième ligne de la spritesheet (down)
            break;
    }
    
    if ((charact.state & CH_STATE_WALKING) && mouvement_inverse)
    {
        spriteID++; //choisit la sprite "walking"
        if ((charact.frame_walk/DEFAULT_ANIM_FRAMES) == 0)
            *flip = SDL_FLIP_HORIZONTAL;
    }
    if ((charact.state & CH_STATE_WALKING) && (!mouvement_inverse))
    {
        if((charact.frame_walk/DEFAULT_ANIM_FRAMES) == 0) //choisit la sprite walking ou bien garde la stand (alterné)
            spriteID++;
    }

    return spriteID;
}

int choseNPCSprite(character npc, SDL_RendererFlip *flip)
//donne le bon identifiant de sprite en fonction des 3 paramètres
{
    int spriteID;
    *flip = SDL_FLIP_NONE;
    //indique si l'animation du mvt se fait en inversant l'image ou non
    //(faux pour les directions gauche et droite car leur animation se 
    //fait en alternant la sprite walking et la sprite stand)
    if(npc.obj.direction == REQ_DIR_RIGHT)
        *flip = SDL_FLIP_HORIZONTAL;
    
    if(!(npc.state & CH_STATE_WALKING))
        return 1;
    spriteID = npc.frame_walk/NPC_ANIM_FRAME_LENGHT + 1;
    return spriteID;
}

int loadNPCSprite(game_context *gctx, character npc,
                  int pos_x, int pos_y, int sprite_ID, SDL_RendererFlip flip)
{
    int status = EXIT_FAILURE;
    if (sprite_ID < 1 || sprite_ID > 4)
    {
        fprintf(stderr, "Erreur loadNPCSprite() : sprite_ID valeur interdite (%d)\n", sprite_ID);
        return status;
    }
    //calcul de l'emplacement de la sprite sur la texture
    int ligne = sprite_ID / 2;
    int colonne = sprite_ID % 2;
    colonne--;
    if(colonne < 0)
    {
        ligne--;
        colonne = 1;
    }
    SDL_Rect source = {colonne*(SPRITE_SIZE+8), ligne*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    if (npc.obj.type == NPC_SANGLIER)
        source.w = SPRITE_SIZE+8;
    if (0 != copieTextureSurRender(gctx->renderer, gctx->sanglier_texture, pos_x, pos_y, source, flip, WIN_SCALE))
        return status;
    
    status = EXIT_SUCCESS;
    return status;
}

void black_stripes(game_context *gctx, int anim_value)
{
    SDL_SetRenderTarget(gctx->renderer, gctx->stripes_texture);
    SDL_SetRenderDrawColor(gctx->renderer, 0,0,0,0);
    SDL_RenderClear(gctx->renderer);
    SDL_SetRenderDrawColor(gctx->renderer, 0,0,0,255);
    SDL_Rect dst = {
        0,
        0,
        TILE_SIZE * NB_TILES_X,
        anim_value
    };
    SDL_RenderFillRect(gctx->renderer, &dst);
    dst.y = TILE_SIZE * NB_TILES_Y - anim_value;
    SDL_RenderFillRect(gctx->renderer, &dst);
    SDL_SetRenderTarget(gctx->renderer, NULL);
}
//
//void renderAll(game_context *gctx)
//{
//    //RENDER
//    //render level
//    SDL_SetRenderTarget(gctx->renderer, NULL);
//    copieTextureSurRender(gctx->renderer, gctx->level_main_layer, 0, 0, gctx->cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
//    copieTextureSurRender(gctx->renderer, gctx->level_overlay, 0, 0, gctx->cam.texLoadSrc, SDL_FLIP_NONE, WIN_SCALE);
//    //HUD
//    copieTextureSurRender(gctx->renderer, gctx->hud, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
//    copieTextureSurRender(gctx->renderer, gctx->stripes_texture, 0,0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
//    copieTextureSurRender(gctx->renderer, gctx->text_dialog, 0, 0, RECT_NULL, SDL_FLIP_NONE, WIN_SCALE);
//}
