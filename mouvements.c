#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "mouvements.h"
#include "enumerations.h"
#include "types_struct_defs.h"

void initPlayer(character *ch)
{
    ch->body_type = BODY_TYPE_DANIEL;
    
    ch->position.x=0; 
    ch->position.y=0;
    
    ch->collider.x=PLAYER_COL_SHIFT; 
    ch->collider.y=2; 
    ch->collider.w = SPRITE_SIZE - PLAYER_COL_SHIFT*2; 
    ch->collider.h = SPRITE_SIZE-2;
    
    ch->pt_de_vie = 1;
    ch->puissance = 1;
    ch->walking = false;
    ch->jumping = false;
    ch->falling = false;
    ch->direction = REQ_DIR_DOWN;
}

bool CheckPlayerOnTheGround(int nb_objs, interobj *objs, character *player)
//vérifie si le joueur a les pieds posés sur qqch
{
    //the sides of the rectangles
    int leftB, rightB, topB;
    int leftA, rightA, bottomA;
    
    //assignations aux valeurs des rectangles
    //rectangle obj_col
    leftA = player->collider.x;
    rightA = player->collider.x + player->collider.w;
    bottomA = player->collider.y + player->collider.h;
    //rectangle b
    //test de tous les colliders
    for(int i=0; i<nb_objs; i++)
    {
        //si le collider est un wall
        if( (objs[i].type == IT_WALL) || (objs[i].type == IT_ENDWALL) )
        {
            topB = objs[i].collider.y;
            leftB = objs[i].collider.x;
            rightB = objs[i].collider.x + objs[i].collider.w;
            //si il y a collision sur l'axe x et que le joueur touche le haut du collider
            if( (leftA < rightB) && (rightA > leftB) && (bottomA == topB) )
                return true;
        }
    }
    return false;
}

bool playerFall(int nb_objs, interobj *objs, character *player, int frame_fall, bool *bump_soundflag)
{
    *bump_soundflag = false;
    //arrête le saut si on est sorti de l'écran
    if(player->position.y > NATIVE_HEIGHT)
    {
        initPlayer(player);
        return false;
    }
    bool on_the_ground = CheckPlayerOnTheGround(nb_objs, objs, player);
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
    while ( (i<=vitesse) && !on_the_ground )
    {
        player->position.y ++;
        player->collider.y ++;
        on_the_ground = CheckPlayerOnTheGround(nb_objs, objs, player);
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

bool checkCollisionX(SDL_Rect a, SDL_Rect b, int dir)
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
    
    /*
    * SI :
    * 
    * /                                                                              \
    * | / bord gauche         le joueur   \     / bord droit           le joueur   \ |     /                       \
    * | | du joueur      ET   regarde vers|  OU | du joueur      ET    regarde vers| |  ET | collision sur l'axe y |
    * | | touche bord         la gauche   |     | touche bord          la droite   | |     \                       /
    * | \ droit du mur                    /     \ gauche du mur                    / |
    * \                                                                              /
    * 
    */
    if( ( ( (leftA == rightB) && (dir == -1) ) || ( (rightA == leftB) && (dir == 1) ) ) && ( (bottomA >= topB) && (topA <= bottomB) ) )
        return true;
    
    return false;
}

bool checkCollisionEndwall(SDL_Rect a, SDL_Rect b)
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
    
    /*
    * SI :
    * 
    * /                                                                              \
    * | / bord gauche         le joueur   \     / bord droit           le joueur   \ |     /                       \
    * | | du joueur      ET   regarde vers|  OU | du joueur      ET    regarde vers| |  ET | collision sur l'axe y |
    * | | touche bord         la gauche   |     | touche bord          la droite   | |     \                       /
    * | \ droit du mur                    /     \ gauche du mur                    / |
    * \                                                                              /
    * 
    */
    if( ( ( (leftA < rightB) /*&& (dir == -1)*/ ) && ( (rightA > leftB)/* && (dir == 1) */) ) && ( (bottomA >= topB) && (topA <= bottomB) ) )
        return true;
    
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
    return false;
}

bool checkAllEndwallCollisions(SDL_Rect a, int nb_objs, interobj *objs, int cam_leftRight)
//teste collision entre un rectangle a et tous les objets existants
//le parametre req permmet de différencier les cas (mur sur l'axe x ou mur au dessus)
{
    bool col;

    for(int i=0; i<nb_objs; i++)
    {
        if(objs[i].type != IT_ENDWALL)
            continue;
        col = checkCollisionEndwall(a, objs[i].collider);
        if(col)
            return true;
    }
    return false;
}

bool updatePositionJump(int nb_objs, interobj *objs, character *player, int frame_jump, bool *hurt_soundflag)
{
    *hurt_soundflag = false;
    flpoint pos_init = player->position;
    SDL_Rect col_init = player->collider;
    int vitesse;
    //calcul de la vitesse de saut qui diminue progressivement
    vitesse = -(gravity) * frame_jump + jump_init_speed;
    
    if(vitesse <= 0)
        return false;

    player->position.y -= vitesse;
    
    for(int i=0; i<vitesse; i++)
    {
        player->collider.y --;
        if( checkAllCollisions(player->collider, nb_objs, objs, REQ_JUMP) )
        {
            player->collider.y = col_init.y - i;
            player->position.y = pos_init.y - i;
            *hurt_soundflag = true;
            return false;
        }
    }
    return true;
}

int updatePositionCam(int nb_objs, interobj *objs, cam *camera, int cam_leftRight)
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

bool updatePositionWalk(int nb_objs, interobj *objs, character *player, int up_down, int left_right)
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
    
    SDL_Rect coll_tmp = player->collider; //ne change pas encore
    flpoint pos_tmp = player->position;
    pos_tmp.x += deplacement; //change ici
    
    //test de collision pour chaque position comprise dans [0; |deplacement|]
    for (int i=0; i <= abs(deplacement); i++)
    {
        coll_tmp.x += i*left_right;
        col = checkAllCollisions(coll_tmp, nb_objs, objs, requete);
        if (col)
        {
            //rabaisse à la dernière position possible
            pos_tmp.x = player->position.x + i*left_right;
            //on utilise position comme base car si player_speed n'est pas
            //entière cela causerait un décalage entre la position
            //du collider (entière) et la position flottante du joueur
            coll_tmp.x = player->position.x + i*left_right;
            break; //sort de la boucle
        }
    }
    
    //si il n'y a pas eu de collision
    if(!col)
        coll_tmp.x = player->position.x + deplacement;
    
    player->position.x = pos_tmp.x;
    player->collider.x = coll_tmp.x + PLAYER_COL_SHIFT;
    
    return !col;
    
}

