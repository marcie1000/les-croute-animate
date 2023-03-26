#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

#include "collisions.h"
#include "enumerations.h"
#include "types_struct_defs.h"
#include "anim.h"

void initPlayer(character *ch, bool initmoney)
{
    ch->obj.type = BODY_TYPE_ANTOINE;
    
    ch->obj.position.x=0; 
    ch->obj.position.y=0;
    
    ch->obj.collider.x=PLAYER_COL_SHIFT; 
    ch->obj.collider.y=2; 
    ch->obj.collider.w = SPRITE_SIZE - PLAYER_COL_SHIFT*2; 
    ch->obj.collider.h = SPRITE_SIZE - ch->obj.collider.y;
    
    ch->obj.life = 1;
    ch->puissance = 1;
    
    ch->state = CH_STATE_NONE;
    
    ch->frame_walk = 0;
    ch->frame_jump = 0;
    ch->frame_fall = 0;
    ch->frame_hurt = 0;
    ch->obj.direction = REQ_DIR_DOWN;
    ch->obj.enabled = true;
    if(initmoney)
        ch->money = 0;
}

bool CheckCharacterOnTheGround(int nb_objs, interobj *objs, character *ch, int *main_tiles_array, 
                               int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y)
//vérifie si le joueur a les pieds posés sur qqch
{
    //the sides of the collsion rectangles
    int leftB, rightB, topB;
    int leftA, rightA, bottomA;
    
    //the sides of the ch collision rectangle
    leftA = ch->obj.collider.x;
    rightA = ch->obj.collider.x + ch->obj.collider.w;
    bottomA = ch->obj.collider.y + ch->obj.collider.h;
    
    //coresponding ground tiles (subscripts of the main_tiles_array array)
    //will look at the tiles at the bottom left, bottom mid and bottom right of the ch
    int subscripts[3];
    subscripts[0] = ((ch->obj.collider.y + ch->obj.collider.h)/TILE_SIZE)*nb_tiles_x + (ch->obj.collider.x/TILE_SIZE) -1;
    subscripts[1] = subscripts[0] + 1;
    subscripts[2] = subscripts[1] + 1;
    
    //tile bounds rectangle
    SDL_Rect tilebnd;
    tilebnd.w = TILE_SIZE;
    tilebnd.h = TILE_SIZE;

    //avoid useless checks if the subscripts are equals
    //(happens when ch is aligned with tiles)
    int start_count = 0, end_count = 3;
    if(subscripts[0] == subscripts[1])
        start_count++;
    if(subscripts[1] == subscripts[2])
        end_count--;
    
    for(int j = start_count; j<end_count; j++)
    {
        //if the tile exist and is one of the wall tiles
        if( subscripts[j] < nb_tiles_x*nb_tiles_y &&
            ( ( (main_tiles_array[subscripts[j]] <= MAX_TILE_WALL) &&
             (main_tiles_array[subscripts[j]] > 0) ) || 
             ((overlay_tiles_array[subscripts[j]] <= MAX_TILE_WALL) &&
             (overlay_tiles_array[subscripts[j]] > 0) ) ) )
        {
            //gets the tile coordinates for the current subscript
            tilebnd.x = subscripts[j]%nb_tiles_x * TILE_SIZE;
            tilebnd.y = subscripts[j]/nb_tiles_x * TILE_SIZE;
            
            //the sides of the tile collision rectangle
            topB = tilebnd.y;
            leftB = tilebnd.x;
            rightB = tilebnd.x + tilebnd.w;
            //if collision on X axis and ch hits the top of the collider
            int dif = topB - bottomA;
            if( (leftA < rightB) && (rightA > leftB) && ((dif <= 0)) )
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
            if(objs[i].type == IT_WALL)
            {
                topB = objs[i].collider.y;
                leftB = objs[i].collider.x;
                rightB = objs[i].collider.x + objs[i].collider.w;
                //if collision on X axis and ch hits the top of the collider
                if( (leftA < rightB) && (rightA > leftB) && ((topB == bottomA)) )
                    return true;
            }
        }
    }
    return false;
}

bool CheckCharacterJumpOnObj(interobj obj, character *ch)
//check if ch jumps on an object, for example jumps on a NPC
{    
    if(!obj.enabled)
        return false;
    
    //the sides of the collsion rectangles
    int leftB, rightB, topB;
    int leftA, rightA, bottomA;
    
    //the sides of the ch collision rectangle
    leftA = ch->obj.collider.x;
    rightA = ch->obj.collider.x + ch->obj.collider.w;
    bottomA = ch->obj.collider.y + ch->obj.collider.h;

    //the side of the object/npc rectangle
    topB = obj.collider.y;
    leftB = obj.collider.x;
    rightB = obj.collider.x + obj.collider.w;
    //if collision on X axis and ch hits the top of the collider
    if( (leftA < rightB) && (rightA > leftB) && (bottomA - topB > 0) && (bottomA - topB <=5) )
        return true;
    
    return false;
}

bool CharacterFall(int nb_objs, interobj *objs, character *ch, 
                int *main_tiles_array, int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y)
//adds gravity to a character
{
    bool on_the_ground = CheckCharacterOnTheGround(nb_objs, objs, ch, main_tiles_array, 
                                                   overlay_tiles_array, nb_tiles_x, nb_tiles_y);
    //update position si le joueur n'a pas les pieds sur le sol
    int vitesse;
    if(!on_the_ground)
    {
        vitesse = gravity * ch->frame_fall;
        //limiter
        if(vitesse > 5)
            vitesse = 5;
    }
    //recheck pour toutes les positions comprises dans [+1;+vitesse]
    int i=1;
    while ( !on_the_ground && (i<=vitesse) )
    {
        ch->obj.position.y ++;
        ch->obj.collider.y ++;
        on_the_ground = CheckCharacterOnTheGround(nb_objs, objs, ch, main_tiles_array, 
                                                  overlay_tiles_array, nb_tiles_x, nb_tiles_y);
        //si vient de toucher le sol
        i++;
    }
    
//    //avoid glitch : sometimes the player breaks in a wall
//    //if the position Y of the player, when they are on the ground,
//    //is not aligned with the tiles grid, the position is re aligned
//    //on the next top grid line
//    int flaw = (ch->obj.collider.y + ch->obj.collider.h) % TILE_SIZE;
//    if(flaw > 0 && on_the_ground)
//    {
//        ch->obj.collider.y --;
//        ch->obj.position.y --;
//    }
        
    //return false if we are not falling and true if we are falling
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

bool checkCollisionX(SDL_Rect ch_coll, SDL_Rect b, int dir/*, bool debug*/)
//checks collision on X axis, left or right
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
    
    
    //if(debug)printf("CheckCollisionX :\nleftA = %d, rightA = %d, topA = %d, bottomA = %d\n", leftA, rightA, topA, bottomA);
    //if(debug)printf("leftB = %d, rightB = %d, topB = %d, bottomB = %d\n", leftB, rightB, topB, bottomB);

    //si collision sur l'axe y
    if( (bottomA >= topB) && (topA <= bottomB) )
    {
        //if(debug)printf("Enter condition 1\n");
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
//        int dif1 = abs(leftA-rightB);
//        int dif2 = abs(leftB-rightA);
        
        if( ( (leftA == rightB) && (dir == -1) ) || ( (leftB == rightA) && (dir == 1) ) )
        {
            //if(debug)printf("CheckCollisionX : true\n");
            return true;
        }
    }
    
    //if(debug)printf("CheckCollisionX : false\n\n");
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
        if( (objs[i].enabled) && (objs[i].type == IT_WALL) )
        {
            switch(req)
            {
                case REQ_DIR_LEFT:
                    dir = -1;
                    col = checkCollisionX(a, objs[i].collider, dir/*, false*/);
                    break;
                case REQ_DIR_RIGHT:
                    dir = +1;
                    col = checkCollisionX(a, objs[i].collider, dir/*, false*/);
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

bool updatePositionJump(int nb_objs, interobj *objs, character *ch, int frame_jump, bool *hurt_soundflag,
                        int *main_tiles_array, int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y)
{
    *hurt_soundflag = false;
    flpoint pos_init = ch->obj.position;
    SDL_Rect col_init = ch->obj.collider;
    int vitesse;
    //calcul de la vitesse de saut qui diminue progressivement
    vitesse = -(gravity) * frame_jump + jump_init_speed;
    
    if(vitesse <= 0)
        return false;

    ch->obj.position.y -= vitesse;
    //======= collision tests with the ID of the tiles only ============
    //coresponding ground tiles (subscripts of the main_tiles_array array)
    int subscripts[3];
    SDL_Rect tilebnd;
    tilebnd.w = TILE_SIZE;
    tilebnd.h = TILE_SIZE;
    SDL_Rect pl_col = ch->obj.collider;

    //test of all positions between 0 and vitesse
    for(int i=0; i<vitesse; i++)
    {
        pl_col.y = --ch->obj.collider.y;
        
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
                        
                if((main_tiles_array[subscripts[j]] <= 0 || main_tiles_array[subscripts[j]] > MAX_TILE_WALL) &&
                    (overlay_tiles_array[subscripts[j]] <= 0 || overlay_tiles_array[subscripts[j]] > MAX_TILE_WALL))
                    continue;
                if (checkCollisionJump(pl_col, tilebnd))
                {
                    ch->obj.collider.y = col_init.y - i;
                    ch->obj.position.y = pos_init.y - i;
                    *hurt_soundflag = true;
                    return false;
                }
            }     
        }
              
        if( checkAllCollisions(ch->obj.collider, nb_objs, objs, REQ_JUMP) )
        {
            ch->obj.collider.y = col_init.y - i;
            ch->obj.position.y = pos_init.y - i;
            *hurt_soundflag = true;
            return false;
        }
    }
    return true;
}

bool updatePositionWalk(int nb_objs, interobj *objs, character *ch, int up_down, int left_right, 
                        int *main_tiles_array, int *overlay_tiles_array, int nb_tiles_x, int nb_tiles_y/*, SDL_Renderer *renderer, bool debug*/)
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
    SDL_Rect coll_tmp = ch->obj.collider; //ne change pas encore
    flpoint pos_tmp = ch->obj.position;
    pos_tmp.x += deplacement; //change ici
    int x_max = nb_tiles_x * TILE_SIZE;

    for (int i=0; i <= fabs(deplacement); i++)
    {
        coll_tmp.x += i*left_right;

        /*  
         * here 0 *-=========
         *          |   |   |
         * here 1 *_|   |   |
         *          =========    <- the ch sprite (squares represents tile size)
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
        subscripts[0] = (  coll_tmp.y                 /TILE_SIZE) * nb_tiles_x + ((coll_tmp.x)/TILE_SIZE) + w;
        subscripts[1] = ( (coll_tmp.y + TILE_SIZE -1) /TILE_SIZE) * nb_tiles_x + ((coll_tmp.x)/TILE_SIZE) + w;
        subscripts[2] = ( (coll_tmp.y + coll_tmp.h-1) /TILE_SIZE) * nb_tiles_x + ((coll_tmp.x)/TILE_SIZE) + w;
        
        SDL_Rect tilebnd;
        tilebnd.w = TILE_SIZE;
        tilebnd.h = TILE_SIZE;

        for(int j = 0; j<3; j++)
        {
            //if the tile is one of the wall tiles
            if(subscripts[j] < nb_tiles_x*nb_tiles_y)
            {
                tilebnd.x = subscripts[j] % nb_tiles_x * TILE_SIZE;
                tilebnd.y = (subscripts[j] / nb_tiles_x /*+ 1*(subscripts[j]!=0))*/ * TILE_SIZE);
                
//                SDL_Rect tmprct;
//                tmprct.x = tilebnd.x*WIN_SCALE;
//                tmprct.y = tilebnd.y*WIN_SCALE;
//                tmprct.w = tilebnd.w*WIN_SCALE;
//                tmprct.h = tilebnd.h*WIN_SCALE;
//                
//                SDL_SetRenderTarget(renderer, NULL);
//                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
//                SDL_RenderFillRect(renderer, &tmprct);
                
                int dir;
                if(requete == REQ_DIR_LEFT) dir = -1;
                else dir = +1;
                if((main_tiles_array[subscripts[j]] <= 0 || main_tiles_array[subscripts[j]] > MAX_TILE_WALL) &&
                   (overlay_tiles_array[subscripts[j]] <= 0 || overlay_tiles_array[subscripts[j]] > MAX_TILE_WALL))
                    continue;
                
                //if (debug) printf("UpdatePositionWalk : subscript = %d, x = %d, y = %d, val = %d\n", subscripts[j], subscripts[j] % 127, 
                //subscripts[j] / 127, main_tiles_array[subscripts[j]]);
                if (checkCollisionX(coll_tmp, tilebnd, dir/*, debug*/))
                {
                    //rabaisse à la dernière position possible
                    pos_tmp.x = ch->obj.position.x + i*left_right;
                    //on utilise position comme base car si ch_speed n'est pas
                    //entière cela causerait un décalage entre la position
                    //du collider (entière) et la position flottante du joueur
                    coll_tmp.x = ch->obj.position.x + i*left_right;
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
            pos_tmp.x = ch->obj.position.x + i*left_right;
            //on utilise position comme base car si ch_speed n'est pas
            //entière cela causerait un décalage entre la position
            //du collider (entière) et la position flottante du joueur
            coll_tmp.x = ch->obj.position.x + i*left_right;
            break; //sort de la boucle
        }
    }
    
    //si il n'y a pas eu de collision
    if(!col)
        coll_tmp.x = ch->obj.position.x + deplacement;
    
    ch->obj.position.x = pos_tmp.x;
    ch->obj.collider.x = coll_tmp.x;
    
    return !col;
    
}

int checkCollisionSpecialAction(int nb_objs, interobj **objs, int nb_npcs, character **npcs,
                                character *ch, int **main_tiles_array, int **overlay_tiles_array,
                                int nb_tiles_x, int nb_tiles_y, gameLevel *level)
{
    unsigned sp_act = SP_AC_NONE;
    //bool col = false;
    
    for(int i = 0; i<nb_npcs; i++)
    {
        //KILL/HURT A NPC
        if((ch->state & CH_STATE_FALLING) && (CheckCharacterJumpOnObj((*npcs)[i].obj, ch)))
        {
            (*npcs)[i].obj.life -= ch->puissance;
            if((*npcs)[i].obj.life == 0) (*npcs)[i].obj.enabled = false;
            sp_act |= SP_AC_NPC_HURT;
        }
        //NPC HURTS THE PLAYER
        if( ( (*npcs)[i].obj.enabled ) && ( (*npcs)[i].obj.type == NPC_SANGLIER ) &&
           !(ch->state & CH_STATE_HURT) )
        {
            if(checkCollision(ch->obj.collider, (*npcs)[i].obj.collider))
            {
                ch->obj.life -= (*npcs)[i].puissance;
                sp_act |= SP_AC_HURT;
                ch->state |= CH_STATE_HURT;
            }
        }
    }
    
    //PICK AN ITEM (coin or heart)
    sp_act |= checkItemCollision(ch, main_tiles_array, overlay_tiles_array, nb_tiles_x, nb_tiles_y);
    
    //ACTIVE A TRIGGER
    for(int i = 0; i<nb_objs; i++)
    {
        bool col = false;
        if( ( (*objs)[i].enabled ) && ( (*objs)[i].type == IT_TRIGGER ) )
            col = checkCollision(ch->obj.collider, (*objs)[i].collider);
        if(col)
        {
            flag_assign(
                &level->active_scripts,
                (1U << (level->script_link[i])),
                true
            );
            (*objs)[i].enabled = false;
        }
    }
    return sp_act;
}

int checkItemCollision(character *ch, int **main_tiles_array, int **overlay_tiles_array, 
                       int nb_tiles_x, int nb_tiles_y)
{
    SDL_Rect pl_col = ch->obj.collider;
    
    //coresponding ground tiles (subscripts of the main_tiles_array array)
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
           ( ( (*overlay_tiles_array)[subscripts[j]] == 0) &&
           ( (*main_tiles_array)[subscripts[j]] == 0) ) )
                continue;
        
        switch (checkItemInLayer(main_tiles_array, subscripts[j], pl_col, &tilebnd, nb_tiles_x, nb_tiles_y))
        {
            case ITEM_COIN:
                sp_act |= SP_AC_EARN_COIN;
                ch->money++;
                break;
            case ITEM_HEART:
                sp_act |= SP_AC_EARN_HEART;
                ch->obj.life++;
                break;
        }
        
        switch (checkItemInLayer(overlay_tiles_array, subscripts[j], pl_col, &tilebnd, nb_tiles_x, nb_tiles_y))
        {
            case ITEM_COIN:
                sp_act |= SP_AC_EARN_COIN;
                ch->money++;
                break;
            case ITEM_HEART:
                sp_act |= SP_AC_EARN_HEART;
                ch->obj.life++;
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

//void objCollisionSpecialActions(int id_obj, interobj **objs, character *ch, int **main_tiles_array, int nb_tiles_x)
////obsolete
////collision test with a programmed collider
//{
//    int posX = (*objs)[id_obj].position.x;
//    int posY = (*objs)[id_obj].position.y;
//    ch->money++;
//    (*main_tiles_array)[posY * nb_tiles_x + posX] = 0;
//    (*objs)[id_obj].enabled = false;
//    
//    printf("ch.money = %d\n", ch->money);
//}

//bool checkCollisionEndwall(SDL_Rect a, SDL_Rect b)
////obsolete
//{
//    //the sides of the rectangles
//    int leftA, leftB;
//    int rightA, rightB;
//    int topA, topB;
//    int bottomA, bottomB;
//    
//    //assignations aux valeurs des rectangles
//    //rectangle a
//    leftA = a.x;
//    rightA = a.x + a.w;
//    topA = a.y;
//    bottomA = a.y + a.h;
//    //rectangle b
//    leftB = b.x;
//    rightB = b.x + b.w;
//    topB = b.y;
//    bottomB = b.y + b.h;
//
//    //si collision sur l'axe y
//    if( (bottomA >= topB) && (topA <= bottomB) )
//    {
//        //si collision sur l'axe x 
//        if( (leftA < rightB) && (rightA > leftB) )
//            return true;
//    }
//    
//    return false;
//}

//int updatePositionCam(int nb_objs, interobj *objs, camera *cam, int cam_leftRight)
////obsolete
//{
//    //rectangle de test
//    SDL_Rect test_coll = cam->texLoadSrc;
//    //incrémente position
//    test_coll.x += cam_leftRight;
//    if(!checkAllEndwallCollisions(test_coll, nb_objs, objs, cam_leftRight))
//    {
//        //cam->absCoord.x += cam_leftRight;
//        cam->texLoadSrc.x += cam_leftRight;
//        //printf("%d,%d\n", cam->absCoord.x, cam->absCoord.x+cam->absCoord.w);
//    }
//    
//    return 0;
//}

//bool checkAllEndwallCollisions(SDL_Rect a, int nb_objs, interobj *objs, int cam_leftRight)
////obsolete
////teste collision entre la camera et les bords du monde
//{
//    bool col;
//
//    for(int i=0; i<nb_objs; i++)
//    {
//        if(objs[i].type == IT_ENDWALL)
//        {
//            col = checkCollisionEndwall(a, objs[i].collider);
//            if(col)
//                return true;
//        }
//    }
//    return false;
//}

