#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <errno.h>

#include "fichiers.h"

int getLevelSize(char *text_line, size_t *taille_x, size_t *taille_y)
{
    int status = EXIT_FAILURE;
    
    //divise la chaine par ","
    char *sub_str;
    sub_str = strtok(text_line, ",");
    if(NULL == sub_str)
        return status;
    
    //taille_x
    sub_str = strtok(NULL, ",");
    if(NULL == sub_str)
        return status;
    //conversion de la sous chaine en nombre
    errno = 0;
    *taille_x = strtoul(sub_str, NULL, 10);
    if(errno != 0)
    {
        perror("getLevelSize");
        return status;
    }
    
    //taille_y
    sub_str = strtok(NULL, ",");
    if(NULL == sub_str)
        return status;
    //conversion de la sous chaine en nombre
    errno = 0;
    *taille_y = strtoul(sub_str, NULL, 10);
    if(errno != 0)
    {
        perror("getLevelSize");
        return status;
    }
    
    status = EXIT_SUCCESS;
    return status;
}

int tilesReading(size_t taille_x, size_t taille_y, FILE *level_file, char *buf, size_t buf_len, int **tiles_array)
{
    //lecture de chaque ligne
    for(size_t i = 0; i < taille_y; i++)
    {
        //si erreur de lecture
        if(0 != lireLigne(level_file, buf, buf_len))
            return EXIT_FAILURE;
        
        char sub_str[buf_len];
        size_t pos_x = 0;
        //lecture de chaque char du buf
        size_t j = 0;
        while(j < buf_len)
        {
            //si on a une case vide
            if( buf[j] == ',' )
            {
                //incrémente la position
                pos_x++;
                j++;
                continue; //continue for j
            }
            //si on arrive en fin de ligne
            if( buf[j] == '\n' || buf[j] == '\0')
                break; //break for j
            //si la taille d'une ligne est dépassée
            if(pos_x >= taille_x)
                break; //break for j

            //copy the buffer in sub_str starting from j
            memcpy(sub_str, (char*)&buf[j], buf_len-j);
            errno = 0;
            //converts chars in number
            long result = strtol(sub_str, NULL, 10);
            if(result > INT_MAX)
                errno = ERANGE;
            if(errno != 0)
            {
                perror("strtol");
                break;
            }
            //writes value in array
            (*tiles_array)[i * (taille_x) + pos_x] = (int)result;
            //measures the length of the sub_string for this number
            char tmp[50];
            snprintf(tmp, 50, "%ld", result);
            j += strlen(tmp);
        }
    }
    return EXIT_SUCCESS;
}

int objArrayFill(FILE *level_file, char *buf, size_t buf_len, interobj **objs, size_t nb_objs)
{
    int status = EXIT_FAILURE;
    char* sub_str;
    //============================================
    //boucle de remplissage du tableau objs
    for(size_t i=0; i<(nb_objs); i++)
    {
        lireLigne(level_file, buf, buf_len);
        if(NULL == buf)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu\n", i);
            return status;
        }
        //==============
        //*
        sub_str = strtok(buf, ",");
        if (NULL == strstr(sub_str, "*")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : *\n", i);
            return status;
        }
        
//        //==============
//        //ID
//        //ID=
//        sub_str = strtok(NULL, ",");
//        if (NULL == strstr(sub_str, "ID=")) 
//        {
//            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : ID=\n", i);
//            return status;
//        }
//        //value
//        sub_str = strtok(NULL, ",");
//        if (NULL == sub_str)
//        {
//            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : ID=[???]\n", i);
//            return status;
//        }
//        (*objs)[i].ID = atoi(sub_str);
        
        //==============
        //type=
        sub_str = strtok(NULL, ",");
        if (NULL == strstr(sub_str, "type=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : type=\n", i);
            return status;
        }
        sub_str = strtok(NULL, ",");
        
        if (NULL != strstr(sub_str, "wall"))
            (*objs)[i].type = IT_WALL;
        else if (NULL != strstr(sub_str, "trigger"))
            (*objs)[i].type = IT_TRIGGER;
        else if (NULL != strstr(sub_str, "sanglier"))
            (*objs)[i].type = NPC_SANGLIER;
        else
            (*objs)[i].type = IT_NONE;
        
        
        //==============
        //position=
        //position en TUILES et non en PIXELS
        sub_str = strtok(NULL, ",");
        if (NULL == strstr(sub_str, "position=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : position=\n", i);
            return status;
        }
        //position x
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : position.x=[???]\n", i);
            return status;
        }
        (*objs)[i].position.x = atoi(sub_str);
        //position y
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : position.y=[???]\n", i);
            return status;
        }
        (*objs)[i].position.y = atoi(sub_str);
        
        //==============
        //collider=
        //collider en PIXELS
        sub_str = strtok(NULL, ",");
        if (NULL == strstr(sub_str, "collider=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : collider=\n", i);
            return status;
        }
        //collider x
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : collider.x=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.x = TILE_SIZE * atoi(sub_str);
        //collider y
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : collider.y=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.y = TILE_SIZE * atoi(sub_str);
        //collider w
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : collider.w=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.w = TILE_SIZE * atoi(sub_str);
        //collider h
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : collider.h=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.h = TILE_SIZE * atoi(sub_str);
        
        //==============
        //life=
        sub_str = strtok(NULL, ",");
        if (NULL == strstr(sub_str, "life=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : life=\n", i);
            return status;
        }
        //value
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : life=[???]\n", i);
            return status;
        }
        if (NULL != strstr(sub_str, "inf")) 
            (*objs)[i].life = -1;
        else
            (*objs)[i].life = atoi(sub_str);
            
        //==============
        //direction=
        sub_str = strtok(NULL, ",");
        if (NULL == strstr(sub_str, "dir=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : dir=\n", i);
            return status;
        }
        //value
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : dir=[???]\n", i);
            return status;
        }
        (*objs)[i].direction = atoi(sub_str);
            
            
        //==============
        //enabled=
        sub_str = strtok(NULL, ",");
        if (NULL == strstr(sub_str, "enabled=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : enabled=\n", i);
            return status;
        }
        //value
        sub_str = strtok(NULL, ",");
        if (NULL == sub_str)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %lu : enabled=[???]\n", i);
            return status;
        }
        if (NULL != strstr(sub_str, "1")) 
            (*objs)[i].enabled = true;
        else
            (*objs)[i].enabled = false;

    } //fin boucle for
    status = EXIT_SUCCESS;
    return status;

}

int objReading(FILE *level_file, char *buf, size_t buf_len, interobj **objs, size_t *nb_objs, character **npcs, size_t *nb_npcs)
{
    int status = EXIT_FAILURE;
    
    //lecture du nb d'interobjects
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "interobjects=,"))
    {
        fprintf(stderr, "Fichier corrompu (interobjects=).\n");
        return status;
    }
    char *sub_str;
    sub_str = strtok(buf, ",");
    sub_str = strtok(NULL, ",");    
    if(NULL == sub_str)
    {
        fprintf(stderr, "Fichier corrompu ( interobjects=[aucune valeur] ).\n");
        return status;
    }
    *nb_objs = atoi(sub_str);
    if(*nb_objs != 0)
    {
        //Réallocation du tableau objs maintenant que l'on connait le nombre d'interobjects
        interobj *tmp = realloc( *objs, sizeof(interobj) * (*nb_objs) );
        if (NULL == tmp)
        {
            fprintf(stderr, "erreur realloc objs\n");
            return status;
        }
        *objs = tmp;
        //remplissage
        if (0 != objArrayFill(level_file, buf, buf_len, objs, *nb_objs))
            return status;
    }
    
    //lecture du nb de NPCS
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "npcs=,"))
    {
        fprintf(stderr, "Fichier corrompu (npcs=).\n");
        return status;
    }
    sub_str = strtok(buf, ",");
    sub_str = strtok(NULL, ",");    
    if(NULL == sub_str)
    {
        fprintf(stderr, "Fichier corrompu ( npcs=[aucune valeur] ).\n");
        return status;
    }
    *nb_npcs = atoi(sub_str);
    
    //arrête la fonction ici si il n'y a pas de npcs
    if(*nb_npcs == 0)
        return EXIT_SUCCESS;
    else if(*nb_npcs > 1)
    {
        //Réallocation du tableau npcs maintenant que l'on connait le nombre d'interobjects
        character *tmp2 = realloc( *npcs, sizeof(character) * (*nb_npcs) );
        if (NULL == tmp2)
        {
            fprintf(stderr, "erreur realloc npcs\n");
            return status;
        }
        *npcs = tmp2;
    }
    
    //tableau temporaire
    interobj *npc_objs_tmp = malloc(sizeof(interobj) * (*nb_npcs));
    //remplissage
    if (0 != objArrayFill(level_file, buf, buf_len, &npc_objs_tmp, *nb_npcs))
    {
        free(npc_objs_tmp);
        return status;
    }
    for(size_t i=0; i<(*nb_npcs); i++)
    {
        //(*npcs)[i].obj.ID = npc_objs_tmp[i].ID;
        (*npcs)[i].obj.type = npc_objs_tmp[i].type;
        
        (*npcs)[i].obj.position.x = npc_objs_tmp[i].position.x;
        (*npcs)[i].obj.position.y = npc_objs_tmp[i].position.y;
        
        (*npcs)[i].obj.collider.x = npc_objs_tmp[i].collider.x;
        (*npcs)[i].obj.collider.y = npc_objs_tmp[i].collider.y;
        (*npcs)[i].obj.collider.w = npc_objs_tmp[i].collider.w;
        (*npcs)[i].obj.collider.h = npc_objs_tmp[i].collider.h;
        
        (*npcs)[i].obj.life = npc_objs_tmp[i].life;
        
        (*npcs)[i].obj.direction = npc_objs_tmp[i].direction;
        
        (*npcs)[i].obj.enabled = npc_objs_tmp[i].enabled;

    }
    
    free(npc_objs_tmp);
    status = EXIT_SUCCESS;
    return status;
}

int loadLevel(const char* level_filename, game_context *gctx)
//fonction principale pour le chargement d'un fichier niveau .csv
{
    int status = EXIT_FAILURE;
    size_t buf_len = 100;
    
    FILE *level_file = fopen(level_filename, "r");
    if (NULL == level_file)
    {
        fprintf(stderr, "Le fichier n'a pas pu être ouvert.\n");
        return status;
    }
    
    //allocation dynamique du buffer
    char *buf = malloc(sizeof(char)*100);
    
    //level size (taille en tuiles du niveau)
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "level_size,"))
    {
        fprintf(stderr, "Fichier corrompu 1.\n");
        goto Cleanup_all;
    }
    gctx->nbTiles_x = 0;
    gctx->nbTiles_y = 0; //variables contenant les tailles du niveau
    if(0 != getLevelSize(buf, &gctx->nbTiles_x, &gctx->nbTiles_y))
    {
        fprintf(stderr, "Fichier corrompu 2.\n");
        goto Cleanup_all;
    }
    
    //ligne start_main_grid
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "start_main_grid"))
    {
        fprintf(stderr, "Fichier corrompu (start_main_grid).\n");
        goto Cleanup_all;
    }
    
    //vérification de la taille max
    //(si nbTiles_x dépasse cette valeur alors cela pourrait engendrer des problèmes de 
    //lecture du fichier au niveau de fgets).
    if( gctx->nbTiles_x > (INT_MAX / 4) )
    {
        fprintf(stderr, "Erreur : la taille du niveau dépasse les valeurs admissibles ( gctx->nbTiles_x > (INT_MAX / 4) )\n");
        goto Cleanup_all;
    }
    
    //réalloc de buf car on connait maintenant la taille des lignes
    if( (gctx->nbTiles_x) * 4 > buf_len )
    {
        buf_len = (gctx->nbTiles_x) * 4 + 1; //+le caractere nul
        char *tmp = realloc(buf, buf_len);
        if (NULL == tmp)
        {
            fprintf(stderr, "erreur realloc.\n");
            goto Cleanup_all;
        }
        buf = tmp;
    }
    
    //reallocation dynamique du tableau de numéro des tuiles
    //maintenant que l'on connait sa taille
    int *tmp = realloc( gctx->main_tiles_array, sizeof( int[gctx->nbTiles_y][gctx->nbTiles_x] ) );
    if (NULL == tmp)
    {
        fprintf(stderr, "erreur realloc gctx->main_tiles_array in fct loadLevel\n");
        goto Cleanup_all;
    }
    gctx->main_tiles_array = tmp;
    
    tmp = realloc( gctx->overlay_tiles_array, sizeof( int[gctx->nbTiles_y][gctx->nbTiles_x] ) );
    if (NULL == tmp)
    {
        fprintf(stderr, "erreur realloc gctx->overlay_tiles_array in fct loadLevel\n");
        goto Cleanup_all;
    }
    gctx->overlay_tiles_array = tmp;
    
    //init a 0
    memset(gctx->main_tiles_array, 0, sizeof( int[gctx->nbTiles_y][gctx->nbTiles_x] ) );
    memset(gctx->overlay_tiles_array, 0, sizeof( int[gctx->nbTiles_y][gctx->nbTiles_x] ) );
    
    //lecture des tuiles du décor et remplissage du tableau main_tiles_array
    tilesReading(gctx->nbTiles_x, gctx->nbTiles_y, level_file, buf, buf_len, &gctx->main_tiles_array);
    
    //ligne end_main_grid
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "end_main_grid"))
    {
        fprintf(stderr, "Fichier corrompu (end_main_grid).\n");
        goto Cleanup_all;
    }
    
    //ligne start_overlay_grid
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "start_overlay_grid"))
    {
        fprintf(stderr, "Fichier corrompu (start_overlay_grid).\n");
        goto Cleanup_all;
    }
    
    //lecture des tuiles du décor et remplissage du tableau overlay_tiles_array
    tilesReading(gctx->nbTiles_x, gctx->nbTiles_y, level_file, buf, buf_len, &gctx->overlay_tiles_array);
    
    //ligne end_overlay_grid
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "end_overlay_grid"))
    {
        fprintf(stderr, "Fichier corrompu (end_overlay_grid).\n");
        goto Cleanup_all;
    }
    
    //lecture des interobjects
    if ( 0 != objReading(level_file, buf, buf_len, &gctx->objs, &gctx->nb_objs, &gctx->npcs, &gctx->nb_npcs) )
    {
        goto Cleanup_all;
    }
    
    status = EXIT_SUCCESS;
    
Cleanup_all:
    
    free(buf);
    
//Close_file:
    
    //fermeture du fichier
    if (fclose(level_file) == EOF)
    {
        fprintf(stderr, "Erreur fermeture du fichier fclose()\n");
        free(buf);
        status = EXIT_FAILURE;
    }
    
    return status;
}

int lireLigne(FILE *fp, char* buf, size_t taille)
{
    int status = EXIT_FAILURE;
    
    if(NULL == fgets(buf, taille, fp))
    {
        if(ferror(fp))
        {
            fprintf(stderr, "Erreur fgets\n");
            return EXIT_FAILURE;
        }
    }
    status = EXIT_SUCCESS;
    return status;
}
