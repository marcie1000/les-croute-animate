#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "pl_mouvements.h"
#include "enumerations.h"
#include "types_struct_defs.h"
//#include "textures_fx.h"

void initPlayer(character *ch, bool initmoney)
{
    ch->obj.type = BODY_TYPE_ANTOINE;
    
    ch->obj.position.x=0; 
    ch->obj.position.y=0;
    
    ch->obj.collider.x=PLAYER_COL_SHIFT; 
    ch->obj.collider.y=2; 
    ch->obj.collider.w = SPRITE_SIZE - PLAYER_COL_SHIFT*2; 
    ch->obj.collider.h = SPRITE_SIZE-2;
    
    ch->obj.pdv = 1;
    ch->puissance = 1;
    ch->walking = false;
    ch->jumping = false;
    ch->falling = false;
    ch->frame_walk = 0;
    ch->frame_jump = 0;
    ch->frame_fall = 0;
    ch->obj.direction = REQ_DIR_DOWN;
    ch->obj.enabled = true;
    if(initmoney)
        ch->money = 0;
}

bool CheckPlayerOnTheGround(int nb_objs, interobj *objs, character *player, int *main_tiles_grid, 
                            int nb_tiles_x, int nb_tiles_y)
//vérifie si le joueur a les pieds posés sur qqch
{
    //the sides of the rectangles
    int leftB, rightB, topB;
    int leftA, rightA, bottomA;
    
    //assignations aux valeurs des rectangles
    //rectangle obj_col
    leftA = player->obj.collider.x;
    rightA = player->obj.collider.x + player->obj.collider.w;
    bottomA = player->obj.collider.y + player->obj.collider.h;
    
    //coresponding ground tiles (subscripts of the main_tiles_grid array)
    int subscripts[3];
    subscripts[0] = ((player->obj.collider.y + player->obj.collider.h)/TILE_SIZE)*nb_tiles_x + (player->obj.collider.x/TILE_SIZE) -1;
    subscripts[1] = subscripts[0] + 1;
    subscripts[2] = subscripts[1] + 1;
    
    SDL_Rect tilebnd;
    tilebnd.w = TILE_SIZE;
    tilebnd.h = TILE_SIZE;

    for(int j = 0; j<3; j++)
    {
        //if the tile is one of the wall tiles
        if(subscripts[j] < nb_tiles_x*nb_tiles_y &&
           (main_tiles_grid[subscripts[j]] <= MAX_TILE_WALL) &&
           (main_tiles_grid[subscripts[j]] > 0))
        {
            tilebnd.x = subscripts[j]%nb_tiles_x * TILE_SIZE;
            tilebnd.y = subscripts[j]/nb_tiles_x * TILE_SIZE;
            
            topB = tilebnd.y;
            leftB = tilebnd.x;
            rightB = tilebnd.x + tilebnd.w;
            //si il y a collision sur l'axe x et que le joueur touche le haut du collider
            if( (leftA < rightB) && (rightA > leftB) && (bottomA == topB) )
                return true;                
        }     
    }

    //collisions with programmed objects
    //test of all colliders
    
    for(int i=0; i<nb_objs; i++)
    {
        //si objet activé
        if(objs[i].enabled)
        {
            if((objs[i].type == IT_WALL) || (objs[i].type == IT_ENDWALL))
            {
                topB = objs[i].collider.y;
                leftB = objs[i].collider.x;
                rightB = objs[i].collider.x + objs[i].collider.w;
                //si il y a collision sur l'axe x et que le joueur touche le haut du collider
                if( (leftA < rightB) && (rightA > leftB) && (bottomA == topB) )
                    return true;
            }
        }
    }
    return false;
}

bool CheckPlayerJumpOnObj(interobj obj, character *player)
//check if player jumps on an object, for example jumps on a NPC
{    
    //the sides of the rectangles
    int leftB, rightB, topB;
    int leftA, rightA, bottomA;
    
    //assignations aux valeurs des rectangles
    //rectangle obj_col
    leftA = player->obj.collider.x;
    rightA = player->obj.collider.x + player->obj.collider.w;
    bottomA = player->obj.collider.y + player->obj.collider.h;
    //rectangle b
    //test de tous les colliders
    //si objet activé
    if(!obj.enabled)
        return false;
        
    topB = obj.collider.y;
    leftB = obj.collider.x;
    rightB = obj.collider.x + obj.collider.w;
    //si il y a collision sur l'axe x et que le joueur touche le haut du collider
    if( (leftA < rightB) && (rightA > leftB) && (bottomA - topB > 0) && (bottomA - topB <=5) )
        return true;
    
    return false;
}

bool playerFall(int nb_objs, interobj *objs, character *player, int frame_fall, bool *bump_soundflag, 
                int *main_tiles_grid, int nb_tiles_x, int nb_tiles_y)
//adds gravity to a character
{
    *bump_soundflag = false;
    bool on_the_ground = CheckPlayerOnTheGround(nb_objs, objs, player, main_tiles_grid, nb_tiles_x, nb_tiles_y);
    //update position si le joueur n'a pas les pieds sur le sol
    int vitesse;
    if(!on_the_ground)
    {
        vitesse = gravity * frame_fall;
        //limiter
        if(vitesse > 5)
            vitesse = 5;
    }
    //recheck pour toutes les positions comprises dans [+1;+vitesse]
    int i=1;
    while ( !on_the_ground && (i<=vitesse) )
    {
        player->obj.position.y ++;
        player->obj.collider.y ++;
        on_the_ground = CheckPlayerOnTheGround(nb_objs, objs, player, main_tiles_grid, nb_tiles_x, nb_tiles_y);
        //si vient de toucher le sol
        if(on_the_ground)
            *bump_soundflag = true;
        i++;
    }
    //renvoie faux si on ne tombe pas et vrai si on tombe
    return !on_the_ground;
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
    //the sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    
    //assignations aux valeurs des rectangles
    //rectangle a
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;
    //rectangle b
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    
    //si un des côtés est en dehors :
    if(leftA >= rightB)
        return false;
    if(rightA <= leftB)
        return false;
    if(topA >= bottomB)
        return false;
    if(bottomA <= topB)
        return false;
    
    return true;
}

bool checkCollisionX(SDL_Rect ch_coll, SDL_Rect b, int dir)
{
    //the sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    
    //assignations aux valeurs des rectangles
    //rectangle ch_coll
    leftA = ch_coll.x;
    rightA = ch_coll.x + ch_coll.w;
    topA = ch_coll.y;
    bottomA = ch_coll.y + ch_coll.h;
    //rectangle b
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    

    //si collision sur l'axe y
    if( (bottomA >= topB) && (topA <= bottomB) )
    {
        /*
        * SI :
        * 
        * /                                                                              \
        * | / bord gauche         le joueur   \     / bord droit           le joueur   \ |
        * | | du joueur      ET   regarde vers|  OU | du joueur      ET    regarde vers| |
        * | | touche bord         la gauche   |     | touche bord          la droite   | |
        * | \ droit du mur                    /     \ gauche du mur                    / |
        * \                                                                              /
        * 
        */
        if( ( (leftA == rightB) && (dir == -1) ) || ( (rightA == leftB) && (dir == 1) ) )
            return true;
    }
    
    return false;
}

bool checkCollisionEndwall(SDL_Rect a, SDL_Rect b)
//obsolete
{
    //the sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    
    //assignations aux valeurs des rectangles
    //rectangle a
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;
    //rectangle b
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //si collision sur l'axe y
    if( (bottomA >= topB) && (topA <= bottomB) )
    {
        //si collision sur l'axe x 
        if( (leftA < rightB) && (rightA > leftB) )
            return true;
    }
    
    return false;
}

bool checkCollisionJump(SDL_Rect a, SDL_Rect b)
{
    //the sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA;
    int bottomB;
    
    //assignations aux valeurs des rectangles
    //rectangle a
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    //rectangle b
    leftB = b.x;
    rightB = (b.x + b.w);
    bottomB = (b.y + b.h);
    
    //si collision sur l'axe x ET le haut du joueur touche le bas d'un mur
    if( ( (leftA < rightB) && (rightA > leftB) ) && (topA == bottomB) )
        return true;
    
    return false;
}

bool checkAllCollisions(SDL_Rect a, int nb_objs, interobj *objs, int req)
//teste collision entre un rectangle a et tous les objets existants
//le parametre req permmet de différencier les cas (mur sur l'axe x ou mur au dessus)
{
    int dir;
    bool col;

    for(int i=0; i<nb_objs; i++)
    {
        if( (objs[i].enabled) && ( ( objs[i].type == IT_WALL) || (objs[i].type == IT_ENDWALL) ) )
        {
            switch(req)
            {
                case REQ_DIR_LEFT:
                    dir = -1;
                    col = checkCollisionX(a, objs[i].collider, dir);
                    break;
                case REQ_DIR_RIGHT:
                    dir = +1;
                    col = checkCollisionX(a, objs[i].collider, dir);
                    break;
                case REQ_JUMP:
                    col = checkCollisionJump(a, objs[i].collider);
            }
            if(col)
                return true;
        }
    }
    return false;
}

bool checkAllEndwallCollisions(SDL_Rect a, int nb_objs, interobj *objs, int cam_leftRight)
//obsolete
//teste collision entre la camera et les bords du monde
{
    bool col;

    for(int i=0; i<nb_objs; i++)
    {
        if(objs[i].type == IT_ENDWALL)
        {
            col = checkCollisionEndwall(a, objs[i].collider);
            if(col)
                return true;
        }
    }
    return false;
}

bool updatePositionJump(int nb_objs, interobj *objs, character *player, int frame_jump, bool *hurt_soundflag,
                        int *main_tiles_grid, int nb_tiles_x, int nb_tiles_y)
{
    *hurt_soundflag = false;
    flpoint pos_init = player->obj.position;
    SDL_Rect col_init = player->obj.collider;
    int vitesse;
    //calcul de la vitesse de saut qui diminue progressivement
    vitesse = -(gravity) * frame_jump + jump_init_speed;
    
    if(vitesse <= 0)
        return false;

    player->obj.position.y -= vitesse;
    //======= collision tests with the ID of the tiles only ============
    //coresponding ground tiles (subscripts of the main_tiles_grid array)
    int subscripts[3];
    SDL_Rect tilebnd;
    tilebnd.w = TILE_SIZE;
    tilebnd.h = TILE_SIZE;
    SDL_Rect pl_col = player->obj.collider;

    //test of all positions between 0 and vitesse
    for(int i=0; i<vitesse; i++)
    {
        pl_col.y = --player->obj.collider.y;
        
        //top left
        subscripts[0] = ((pl_col.y-1)/TILE_SIZE)*nb_tiles_x + (pl_col.x/TILE_SIZE);
        //top midle
        subscripts[1] = ((pl_col.y-1)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w/2)/TILE_SIZE);
        //top right
        subscripts[2] = ((pl_col.y-1)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w)/TILE_SIZE);
        
        
        for(int j = 0; j<3; j++)
        {
            //if the tile is one of the wall tiles
            if(subscripts[j] < nb_tiles_x*nb_tiles_y)
            {
                tilebnd.x = subscripts[j]%nb_tiles_x * TILE_SIZE;
                tilebnd.y = subscripts[j]/nb_tiles_x * TILE_SIZE;
                        
                if(main_tiles_grid[subscripts[j]] <= 0 || main_tiles_grid[subscripts[j]] > MAX_TILE_WALL)
                    continue;
                if (checkCollisionJump(pl_col, tilebnd))
                {
                    player->obj.collider.y = col_init.y - i;
                    player->obj.position.y = pos_init.y - i;
                    *hurt_soundflag = true;
                    return false;
                }
            }     
        }
              
        if( checkAllCollisions(player->obj.collider, nb_objs, objs, REQ_JUMP) )
        {
            player->obj.collider.y = col_init.y - i;
            player->obj.position.y = pos_init.y - i;
            *hurt_soundflag = true;
            return false;
        }
    }
    return true;
}

int updatePositionCam(int nb_objs, interobj *objs, cam *camera, int cam_leftRight)
//obsolete
{
    //rectangle de test
    SDL_Rect test_coll = camera->absCoord;
    //incrémente position
    test_coll.x += cam_leftRight;
    if(!checkAllEndwallCollisions(test_coll, nb_objs, objs, cam_leftRight))
    {
        camera->absCoord.x += cam_leftRight;
        camera->texLoadSrc.x += cam_leftRight;
        //printf("%d,%d\n", camera->absCoord.x, camera->absCoord.x+camera->absCoord.w);
    }
    
    return 0;
}

bool updatePositionWalk(int nb_objs, interobj *objs, character *player, int up_down, int left_right, 
                        int *main_tiles_grid, int nb_tiles_x, int nb_tiles_y)
{
    int requete;
    if (left_right < 0)
        requete = REQ_DIR_LEFT;
    else if (left_right > 0)
        requete = REQ_DIR_RIGHT;
    else
        return 0;
    float deplacement = left_right * player_speed;
    bool col = false;
    //reach end of level
    bool reach_end = false;
    SDL_Rect coll_tmp = player->obj.collider; //ne change pas encore
    flpoint pos_tmp = player->obj.position;
    pos_tmp.x += deplacement; //change ici
    int x_max = nb_tiles_x * TILE_SIZE;

    for (int i=0; i <= fabs(deplacement); i++)
    {
        coll_tmp.x += i*left_right;

        /*  
         * here 0 *-=========
         *          |   |   |
         * here 1 *_|   |   |
         *          =========    <- the player sprite (squares represents tile size)
         *          |   |   |
         * here 2 *_|   |   |
         *          =========
         *   the four subscripts point 
         * 
         */
        //coresponding ground tiles
        int subscripts[3];
        int w=-1;
        if(requete == REQ_DIR_RIGHT) 
        {
            w=(coll_tmp.w/TILE_SIZE);
            if(w==0) w=1;
        }
        subscripts[0] = (coll_tmp.y/TILE_SIZE)*nb_tiles_x + ((coll_tmp.x+1)/TILE_SIZE) + w;
        subscripts[1] = ( (coll_tmp.y + TILE_SIZE -1) /TILE_SIZE )*nb_tiles_x + ((coll_tmp.x+1)/TILE_SIZE) + w;
        subscripts[2] = ( (coll_tmp.y + TILE_SIZE -1) /TILE_SIZE + 1)*nb_tiles_x + ((coll_tmp.x+1)/TILE_SIZE) + w;
        
        SDL_Rect tilebnd;
        tilebnd.w = TILE_SIZE;
        tilebnd.h = TILE_SIZE;

        for(int j = 0; j<3; j++)
        {
            //if the tile is one of the wall tiles
            if(subscripts[j] < nb_tiles_x*nb_tiles_y)
            {
                if(requete == REQ_DIR_LEFT)
                    tilebnd.x = coll_tmp.x - TILE_SIZE;
                else
                    tilebnd.x = (coll_tmp.x + coll_tmp.w);
                tilebnd.x -= tilebnd.x % TILE_SIZE;
                tilebnd.y = (subscripts[j] / nb_tiles_x + 1*(subscripts[j]!=0)) * TILE_SIZE;
                
                int dir;
                if(requete == REQ_DIR_LEFT) dir = -1;
                else dir = +1;
                if(main_tiles_grid[subscripts[j]] <= 0 || main_tiles_grid[subscripts[j]] > MAX_TILE_WALL)
                    continue;
                if (checkCollisionX(coll_tmp, tilebnd, dir))
                {
                    //rabaisse à la dernière position possible
                    pos_tmp.x = player->obj.position.x + i*left_right;
                    //on utilise position comme base car si player_speed n'est pas
                    //entière cela causerait un décalage entre la position
                    //du collider (entière) et la position flottante du joueur
                    coll_tmp.x = player->obj.position.x + i*left_right;
                    col = true;
                    break; //sort de la boucle
                }
            }     
        }
        
        if(col) break;

        col = checkAllCollisions(coll_tmp, nb_objs, objs, requete);
        reach_end = (coll_tmp.x <= 0 && left_right == -1) ||
                     (coll_tmp.x + coll_tmp.w >= x_max && left_right == +1);
        if (col || reach_end)
        {
            //rabaisse à la dernière position possible
            pos_tmp.x = player->obj.position.x + i*left_right;
            //on utilise position comme base car si player_speed n'est pas
            //entière cela causerait un décalage entre la position
            //du collider (entière) et la position flottante du joueur
            coll_tmp.x = player->obj.position.x + i*left_right;
            break; //sort de la boucle
        }
    }
    
    //si il n'y a pas eu de collision
    if(!col)
        coll_tmp.x = player->obj.position.x + deplacement;
    
    player->obj.position.x = pos_tmp.x;
    player->obj.collider.x = coll_tmp.x;
    
    return !col;
    
}

int checkCollisionSpecialAction(int nb_objs, interobj **objs, int nb_npcs, character **npcs,
                                character *player, int **main_tiles_grid, int **overlay_tiles_grid,
                                int nb_tiles_x, int nb_tiles_y, int *type)
{
    unsigned sp_act = SP_AC_NONE;
    bool col = false;
    
    for(int i = 0; i<nb_npcs; i++)
    {
        if(CheckPlayerJumpOnObj((*npcs)[i].obj, player))
        {
            (*npcs)[i].obj.pdv -= player->puissance;
            if((*npcs)[i].obj.pdv == 0) (*npcs)[i].obj.enabled = false;
            sp_act |= SP_AC_NPC_HURT;
        }
        if( ( (*npcs)[i].obj.enabled ) && ( (*npcs)[i].obj.type == NPC_SANGLIER ) )
            col = checkCollision(player->obj.collider, (*npcs)[i].obj.collider);
        if(col)
        {
            *type = NPC_SANGLIER;
            player->obj.pdv -= (*npcs)[i].puissance;
            sp_act |= SP_AC_HURT;
        }
    }
    
    sp_act |= checkItemCollision(player, main_tiles_grid, overlay_tiles_grid, nb_tiles_x, nb_tiles_y);
    
//    for(int i = 0; i<nb_objs; i++)
//    {
//        if( ( (*objs)[i].enabled ) && ( (*objs)[i].type == IT_COIN ) )
//            col = checkCollision(player->obj.collider, (*objs)[i].collider);
//        if(col)
//        {
//            objCollisionSpecialActions(i, objs, player, main_tiles_grid, nb_tiles_x);
//            *type = IT_COIN;
//            sp_act |= SP_AC_EARN_COIN;
//        }
//    }
    return sp_act;
}

int checkItemCollision(character *player, int **main_tiles_grid, int **overlay_tiles_grid, 
                        int nb_tiles_x, int nb_tiles_y)
{
    SDL_Rect pl_col = player->obj.collider;
    
    //coresponding ground tiles (subscripts of the main_tiles_grid array)
    int subscripts[9];
    //bottom left
    subscripts[0] = ((pl_col.y + pl_col.h)/TILE_SIZE)*nb_tiles_x + (pl_col.x/TILE_SIZE);
    //midle left
    subscripts[1] = ((pl_col.y + pl_col.h/2)/TILE_SIZE)*nb_tiles_x + (pl_col.x/TILE_SIZE);
    //top left
    subscripts[2] = ((pl_col.y)/TILE_SIZE)*nb_tiles_x + (pl_col.x/TILE_SIZE);
    
    //bottom midle
    subscripts[3] = ((pl_col.y + pl_col.h)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w/2)/TILE_SIZE);
    //midle midle
    subscripts[4] = ((pl_col.y + pl_col.h/2)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w/2)/TILE_SIZE);
    //top midle
    subscripts[5] = ((pl_col.y)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w/2)/TILE_SIZE);

    //bottom right
    subscripts[6] = ((pl_col.y + pl_col.h)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w)/TILE_SIZE);
    //midle right
    subscripts[7] = ((pl_col.y + pl_col.h/2)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w)/TILE_SIZE);
    //top right
    subscripts[8] = ((pl_col.y)/TILE_SIZE)*nb_tiles_x + ((pl_col.x + pl_col.w)/TILE_SIZE);


    SDL_Rect tilebnd;
    tilebnd.w = TILE_SIZE;
    tilebnd.h = TILE_SIZE;
    
    unsigned sp_act = SP_AC_NONE;

    for(int j = 0; j<9; j++)
    {
        //if the tile is one of the wall tiles
        if ( subscripts[j] >= nb_tiles_x*nb_tiles_y ||
           ( ( (*overlay_tiles_grid)[subscripts[j]] == 0) &&
           ( (*main_tiles_grid)[subscripts[j]] == 0) ) )
                continue;
        
        switch (checkItemInLayer(main_tiles_grid, subscripts[j], pl_col, &tilebnd, nb_tiles_x, nb_tiles_y))
        {
            case ITEM_COIN:
                sp_act |= SP_AC_EARN_COIN;
                player->money++;
                break;
            case ITEM_HEART:
                sp_act |= SP_AC_EARN_HEART;
                player->obj.pdv++;
                break;
        }
        
        switch (checkItemInLayer(overlay_tiles_grid, subscripts[j], pl_col, &tilebnd, nb_tiles_x, nb_tiles_y))
        {
            case ITEM_COIN:
                sp_act |= SP_AC_EARN_COIN;
                player->money++;
                break;
            case ITEM_HEART:
                sp_act |= SP_AC_EARN_HEART;
                player->obj.pdv++;
                break;
        }
    }
    
    return sp_act;
}

int checkItemInLayer(int **tiles_grid, int subscript, SDL_Rect pl_col, SDL_Rect *tilebnd,
                    int nb_tiles_x, int nb_tiles_y)
//checks for item collision in a specific layer
{
    int tileID = 0;
    bool item = false;
    switch( (*tiles_grid)[subscript])
    {
        case ITEM_COIN:
        case ITEM_HEART:
            item = true;
            break;
        default:
            item = false;
    }
    
    if(item)
    {
        tilebnd->x = subscript%nb_tiles_x * TILE_SIZE;
        tilebnd->y = subscript/nb_tiles_x * TILE_SIZE;
        if(checkCollision(pl_col, *tilebnd))
        {
            tileID =  (*tiles_grid)[subscript];
            (*tiles_grid)[subscript] = 0;
        }
    }
    return tileID;
}

void objCollisionSpecialActions(int id_obj, interobj **objs, character *player, int **main_tiles_grid, int nb_tiles_x)
//obsolete
//collision test with a programmed collider
{
    int posX = (*objs)[id_obj].position.x;
    int posY = (*objs)[id_obj].position.y;
    player->money++;
    (*main_tiles_grid)[posY * nb_tiles_x + posX] = 0;
    (*objs)[id_obj].enabled = false;
    
    printf("player.money = %d\n", player->money);
}

