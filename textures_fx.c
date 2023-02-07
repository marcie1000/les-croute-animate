#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "textures_fx.h"
#include "enumerations.h"

int initTextures(SDL_Renderer *renderer, SDL_Texture **croute_texture, SDL_Texture **assets_tiles)
{
    int status = EXIT_FAILURE;
    
    *croute_texture = IMG_LoadTexture(renderer, CROUTE_PNG_FILENAME);
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
    
    status = EXIT_SUCCESS;
    return status;
}

int destroyTextures(SDL_Texture **croute_texture, SDL_Texture **assets_tiles, SDL_Texture **level_main)
{
    int status = EXIT_FAILURE;
    
    if(NULL != *croute_texture)
        SDL_DestroyTexture(*croute_texture);
    if(NULL != *assets_tiles)
        SDL_DestroyTexture(*assets_tiles);
    if(NULL != *level_main)
        SDL_DestroyTexture(*level_main);
    
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

int initLevelTextures(SDL_Texture **level_main, SDL_Renderer *renderer, int nb_tuiles_x, int nb_tuiles_y)
{
    int largeur_texture = nb_tuiles_x*TILE_SIZE;
    *level_main = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
                                    largeur_texture, nb_tuiles_y * TILE_SIZE);
    if(NULL == *level_main)
    {
        fprintf(stderr, "erreur SDL_CreateTexture level_main : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int loadLevelSprites(SDL_Texture **dest_text, SDL_Texture *assets_tiles, int *level_tiles_grid, 
                              int nb_tuiles_x, int nb_tuiles_y, SDL_Renderer *renderer)
{
    SDL_Rect source;
    //TEXTURE_SHIFTSIZE_X sert à créer l'espace nécesssaire à la mise à jour de la texture hors écran
    //pour le scrolling
    SDL_SetTextureBlendMode(*dest_text, SDL_BLENDMODE_BLEND);
    if(NULL == *dest_text)
    {
        fprintf(stderr, "erreur SDL_CreateTexture in function loadLevelSprites : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetRenderTarget(renderer, *dest_text);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_RenderClear(renderer);
    
    for(int i=0; i<nb_tuiles_y; i++)
    {
        for(int j=0; j<(nb_tuiles_x/*NB_TILES_X + TEXTURE_TILES_SHIFTSIZE_X*/); j++)
        {
            if( 0 != level_tiles_grid[i * nb_tuiles_x + j] )
            {
                if( 0!= selectLvlAssetsSprite(level_tiles_grid[i * nb_tuiles_x + j], &source) )
                    return EXIT_FAILURE;
                if (0 != copieTextureSurRender(renderer, assets_tiles, j * TILE_SIZE, 
                    i * TILE_SIZE, source, SDL_FLIP_NONE, 1))
                    return EXIT_FAILURE;
            }
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
    return EXIT_SUCCESS;
}

int selectLvlAssetsSprite(int sprite_ID, SDL_Rect *source)
{
    int status = EXIT_FAILURE;
    if (sprite_ID < 1 || sprite_ID > 64)
    {
        fprintf(stderr, "Erreur selectLvlAssetsSprite() : sprite_ID valeur interdite (%d)\n", sprite_ID);
        return status;
    }
    //calcul de l'emplacement de la sprite sur la texture
    int ligne = sprite_ID / 8;
    int colonne = sprite_ID % 8;
    colonne--;
    if(colonne < 0)
    {
        ligne--;
        colonne = 7;
    }    
    source->x = colonne*TILE_SIZE;
    source->y = ligne*TILE_SIZE;
    source->w = TILE_SIZE;
    source->h = TILE_SIZE;
    
    status = EXIT_SUCCESS;
    return status;
}

int chosePlayerSprite(int direction, bool player_moving, bool playerID, int rapport_frame, SDL_RendererFlip *flip)
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