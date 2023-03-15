#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "textures_fx.h"
#include "enumerations.h"
#include "types_struct_defs.h"

static text_format hud_tf = {
    TTF_FONT_FILENAME,
    16,
    (SDL_Color){0,0,0,255}
};

int initTextures(SDL_Renderer *renderer, SDL_Texture **croute_texture, 
                 SDL_Texture **assets_tiles, SDL_Texture **npc_texture,
                 SDL_Texture **hud, int *ts_nb_x, int *ts_nb_y)
{
    int status = EXIT_FAILURE;
    
    *croute_texture = IMG_LoadTexture(renderer, CROUTE_SPRITES_PNG);
    if(NULL == *croute_texture)
    {
        fprintf(stderr, "erreur IMG_LoadTexture() croute_texture : %s\n", IMG_GetError());
        return status;
    }
    
    *assets_tiles = IMG_LoadTexture(renderer, ASSETS_TILES_PNG);
    if(NULL == *assets_tiles)
    {
        fprintf(stderr, "erreur IMG_LoadTexture() assets_tiles : %s\n", IMG_GetError());
        return status;
    }
    SDL_Point bounds;
    if(0 != SDL_QueryTexture(*assets_tiles, NULL, NULL, &bounds.x, &bounds.y))
    {
        fprintf(stderr, "error SDL_QueryTexture : %s\n", SDL_GetError());
        return status;
    }
    *ts_nb_x = bounds.x / 8;
    *ts_nb_y = bounds.y / 8;
    
    *npc_texture = IMG_LoadTexture(renderer, NPC_SPRITES_PNG);
    if(NULL == *npc_texture)
    {
        fprintf(stderr, "erreur IMG_LoadTexture() npc_texture : %s\n", IMG_GetError());
        return status;
    }
    
    *hud = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                             NB_TILES_X * TILE_SIZE, 2 * TILE_SIZE);
    if(NULL == *hud)
    {
        fprintf(stderr, "error SDL_CreateTexture in function HUD_iinit: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetTextureBlendMode(*hud, SDL_BLENDMODE_BLEND);

    
    status = EXIT_SUCCESS;
    return status;
}

int destroyTextures(SDL_Texture **croute_texture, SDL_Texture **assets_tiles, 
                    SDL_Texture **level_main, SDL_Texture **npc_texture, 
                    SDL_Texture **level_overlay, SDL_Texture **hud)
{
    int status = EXIT_FAILURE;
    
    if(NULL != *npc_texture)
        SDL_DestroyTexture(*npc_texture);
    if(NULL != *croute_texture)
        SDL_DestroyTexture(*croute_texture);
    if(NULL != *assets_tiles)
        SDL_DestroyTexture(*assets_tiles);
    if(NULL != *level_main)
        SDL_DestroyTexture(*level_main);
    if(NULL != *level_overlay)
        SDL_DestroyTexture(*level_overlay);
    if(NULL != *hud)
        SDL_DestroyTexture(*hud);
    
    status = EXIT_SUCCESS;
    return status;
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



int loadPlayerSprite(SDL_Renderer *renderer, SDL_Texture *croute_texture,
                     int pos_x, int pos_y, int sprite_ID, SDL_RendererFlip flip)
{
    int status = EXIT_FAILURE;
    if (sprite_ID < 1 || sprite_ID > 12)
    {
        fprintf(stderr, "Erreur loadPlayerSprite() : sprite_ID valeur interdite (%d)\n", sprite_ID);
        return status;
    }
    //calcul de l'emplacement de la sprite sur la texture
    int ligne = sprite_ID / 4;
    int colonne = sprite_ID % 4;
    colonne--;
    if(colonne < 0)
    {
        ligne--;
        colonne = 3;
    }
    SDL_Rect source = {colonne*SPRITE_SIZE, ligne*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    if (0 != copieTextureSurRender(renderer, croute_texture, pos_x, pos_y, source, flip, WIN_SCALE))
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


int HUD_update(SDL_Renderer *renderer, SDL_Texture **hud, SDL_Texture *assets_tiles,
               int ts_nb_x, int ts_nb_y, character player)
{
    static int money;
    static int life;
    bool changes = true;
    
    if(money != player.money)
    {
        changes = true;
        money = player.money;
    }
    else if(life != player.obj.pdv)
    {
        changes = true;
        life = player.obj.pdv;
    }
    else 
        changes = false;
    
    if(changes)
    {
        SDL_SetRenderTarget(renderer, *hud);
        SDL_SetRenderDrawColor(renderer, 0,0,0,0);
        SDL_RenderClear(renderer);
        
        int posX = 1; 
        
        //coin
        SDL_Rect src;
        selectLvlAssetsTile(ITEM_COIN, &src, ts_nb_x, ts_nb_y);
        copieTextureSurRender(renderer, assets_tiles, posX, 2, src, SDL_FLIP_NONE, 1);
        //SDL_SetRenderTarget(renderer, NULL);
        
        char buf[50];
        sprintf(buf, "%d", player.money);
        SDL_Texture *tmp = createText(hud_tf, buf, renderer);
        if(NULL == tmp)
            return EXIT_FAILURE;
        posX += 3+TILE_SIZE;
        copieTextureSurRender(renderer, tmp, posX, -1, RECT_NULL, SDL_FLIP_NONE, 1);
        SDL_DestroyTexture(tmp);
        
        //life
        posX += 2*TILE_SIZE;
        for(int i = 1; i<=PLAYER_MAX_LIFE; i++)
        {
            if(player.obj.pdv >= i)
                selectLvlAssetsTile(ITEM_HEART, &src, ts_nb_x, ts_nb_y);
            else
                selectLvlAssetsTile(ITEM_EMPTY_HEART, &src, ts_nb_x, ts_nb_y);
            copieTextureSurRender(renderer, assets_tiles, posX, 2, src, SDL_FLIP_NONE, 1);
            posX += TILE_SIZE + 1;
        }
        
        SDL_SetRenderTarget(renderer, NULL);
    }
    
    return 0;
}

SDL_Texture *createText(text_format tf, const char *string, SDL_Renderer *ren)
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
    surface = TTF_RenderUTF8_Blended(font, string, tf.color);
    TTF_CloseFont(font);
    if (NULL == surface)
    {
        fprintf(stderr, "Erreur TTF_RenderUTF8_Blended : %s", TTF_GetError());
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

int chosePlayerSprite(int direction, bool player_moving, bool playerID, 
                      int rapport_frame, SDL_RendererFlip *flip)
//donne le bon identifiant de sprite en fonction des 3 paramètres
{
    int spriteID;
    *flip = SDL_FLIP_NONE;
    //indique si l'animation du mvt se fait en inversant l'image ou non
    //(faux pour les directions gauche et droite car leur animation se 
    //fait en alternant la sprite walking et la sprite stand)
    bool mouvement_inverse = true; 
    switch (direction)
    {
        case REQ_DIR_RIGHT:
            *flip = SDL_FLIP_HORIZONTAL; //inverse la side sprite pour la diriger à droite
        case REQ_DIR_LEFT:
            spriteID = 9; //va à la dernière ligne de la spritesheet (side)
            mouvement_inverse = false;
            break;
        case REQ_DIR_UP:
            spriteID = 1; //va à la première ligne de la spritesheet (up)
            break;
        case REQ_DIR_DOWN:
            spriteID = 5; //va à la deuxième ligne de la spritesheet (down)
            break;
    }
    
    if (player_moving && mouvement_inverse)
    {
        spriteID++; //choisit la sprite "walking"
        if (rapport_frame == 0)
            *flip = SDL_FLIP_HORIZONTAL;
    }
    if (player_moving && (!mouvement_inverse))
    {
        if(rapport_frame == 0) //choisit la sprite walking ou bien garde la stand (alterné)
            spriteID++;
    }
    if (playerID)
        spriteID+=2; //choisit une sprite de l'autre personnage
    return spriteID;
}

int choseNPCSprite(int direction, bool moving, int rapport_frame, SDL_RendererFlip *flip)
//donne le bon identifiant de sprite en fonction des 3 paramètres
{
    int spriteID;
    *flip = SDL_FLIP_NONE;
    //indique si l'animation du mvt se fait en inversant l'image ou non
    //(faux pour les directions gauche et droite car leur animation se 
    //fait en alternant la sprite walking et la sprite stand)
    if(direction == REQ_DIR_RIGHT)
        *flip = SDL_FLIP_HORIZONTAL;
    
    if(!moving)
        return 1;
    switch(rapport_frame)
    {
        case 0:
            spriteID = 1;
            break;
        case 1:
            spriteID = 2;
            break;
        case 2:
            spriteID = 3;
            break;
        case 3:
            spriteID = 4;
            break;
    }
    return spriteID;
}

int loadNPCSprite(SDL_Renderer *renderer, SDL_Texture *npc_texture,
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
    SDL_Rect source = {colonne*(SPRITE_SIZE+8), ligne*SPRITE_SIZE, SPRITE_SIZE+8, SPRITE_SIZE};
    if (0 != copieTextureSurRender(renderer, npc_texture, pos_x, pos_y, source, flip, WIN_SCALE))
        return status;
    
    status = EXIT_SUCCESS;
    return status;
}

