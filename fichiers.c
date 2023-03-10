#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#include "fichiers.h"

int getNbrFromChars(char *string)
//convertit une chaine en nombre.
//si aucun nombre trouvé, renvoie 0
{
    int length = strlen(string);
    int newlength = length;
    int value = 0;
    for(int i = 0; i < length; i++)
    {
        //si n'est pas un caractère chiffre (en commençant par la fin)
        if((string[length-i-1] < 48) || (string[length-i-1] > 57))
            newlength--;
    }
    length = newlength;
    for(int i = 0; i < length; i++)
    {
        value = value + (string[length-i-1]-48) * (pow(10, i));
    }
    return value;
    
}

//int getCharsFromNmbr(unsigned int value, char *string)
////Convertit un nombre en chaine.
////Retourne le nombre de chiffres.
//{
//    int tmp = value;
//    int len = 0;
//    //calcule le nombre de chiffres
//    while(tmp != 0)
//    {
//        tmp = tmp/10;
//        len++;
//    }
//    
//    //limites
//    if(len > sizeof(string))
//        len = sizeof(string);
//    
//    for(int i = 0; i<len; i++)
//    {
//        int divide = value / (pow(10, len-i-1));
//        string[i] = '0' + divide;
//        value = value - divide * pow(10, len-i-1);
//    }
//    string[len] = '\0';
//    return len;
//}

int getLevelSize(char *text_line, int *taille_x, int *taille_y)
{
    int status = EXIT_FAILURE;
    
    //divise la chaine par ","
    char *sous_chaine;
    sous_chaine = strtok(text_line, ",");
    if(NULL == sous_chaine)
        return status;
    
    //taille_x
    sous_chaine = strtok(NULL, ",");
    if(NULL == sous_chaine)
        return status;
    //conversion de la sous chaine en nombre
    *taille_x = getNbrFromChars(sous_chaine);
    
    //taille_y
    sous_chaine = strtok(NULL, ",");
    if(NULL == sous_chaine)
        return status;
    //conversion de la sous chaine en nombre
    *taille_y = getNbrFromChars(sous_chaine);
    
    status = EXIT_SUCCESS;
    return status;
}

void tilesReading(int taille_x, int taille_y, FILE *level_file, char *buf, int buf_len, int **tiles_array)
{
    //lecture de chaque ligne
    for(int i = 0; i < taille_y; i++)
    {
        //si erreur de lecture
        if(0 != lireLigne(level_file, buf, buf_len))
            break; //break for i
        char sous_chaine[buf_len];
        int pos_x = 0;
        //lecture de chaque char du buf
        for(int j = 0; j < buf_len; j++)
        {
            //si on a une case vide
            if( buf[j] == ',' )
            {
                //incrémente la position
                pos_x++;
                continue; //continue for j
            }
            //si on arrive en fin de ligne
            if( buf[j] == '\n')
                break; //break for j
            //si la taille d'une ligne est dépassée
            if(pos_x >= taille_x)
                break; //break for j
            
            int k = 0;
            strcpy(sous_chaine, "\0");
            char c[] = "\0";
            do
            {
                c[0] = buf[j+k];
                strcat(sous_chaine, c);
                k++;
            }while( ( buf[j+k] >= 48 ) && ( buf[j+k] <= 57 ) ); //tant que char est un chiffre
            //convertir la sous chaine en nombre et l'ajoute dans le tableau
            (*tiles_array)[i * (taille_x) + pos_x] = getNbrFromChars(sous_chaine);
            j = j+k-1;
        }
    }
}

int objArrayFill(FILE *level_file, char *buf, int buf_len, interobj **objs, int nb_objs)
{
    int status = EXIT_FAILURE;
    char* sous_chaine;
    //============================================
    //boucle de remplissage du tableau objs
    for(int i=0; i<(nb_objs); i++)
    {
        lireLigne(level_file, buf, buf_len);
        if(NULL == buf)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d\n", i);
            return status;
        }
        //==============
        //*
        sous_chaine = strtok(buf, ",");
        if (NULL == strstr(sous_chaine, "*")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : *\n", i);
            return status;
        }
        
        //==============
        //type=
        sous_chaine = strtok(NULL, ",");
        if (NULL == strstr(sous_chaine, "type=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : type=\n", i);
            return status;
        }
        sous_chaine = strtok(NULL, ",");
        
        if (NULL != strstr(sous_chaine, "endwall"))
            (*objs)[i].type = IT_ENDWALL;
        else if (NULL != strstr(sous_chaine, "wall"))
            (*objs)[i].type = IT_WALL;
        else if (NULL != strstr(sous_chaine, "coin"))
            (*objs)[i].type = IT_COIN;
        else if (NULL != strstr(sous_chaine, "sanglier"))
            (*objs)[i].type = NPC_SANGLIER;
        else
            (*objs)[i].type = IT_NONE;
        
        
        //==============
        //position=
        //position en TUILES et non en PIXELS
        sous_chaine = strtok(NULL, ",");
        if (NULL == strstr(sous_chaine, "position=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : position=\n", i);
            return status;
        }
        //position x
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : position.x=[???]\n", i);
            return status;
        }
        (*objs)[i].position.x = getNbrFromChars(sous_chaine);
        //position y
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : position.y=[???]\n", i);
            return status;
        }
        (*objs)[i].position.y = getNbrFromChars(sous_chaine);
        
        //==============
        //collider=
        //collider en PIXELS
        sous_chaine = strtok(NULL, ",");
        if (NULL == strstr(sous_chaine, "collider=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : collider=\n", i);
            return status;
        }
        //collider x
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : collider.x=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.x = TILE_SIZE * getNbrFromChars(sous_chaine);
        //collider y
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : collider.y=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.y = TILE_SIZE * getNbrFromChars(sous_chaine);
        //collider w
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : collider.w=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.w = TILE_SIZE * getNbrFromChars(sous_chaine);
        //collider h
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : collider.h=[???]\n", i);
            return status;
        }
        (*objs)[i].collider.h = TILE_SIZE * getNbrFromChars(sous_chaine);
        
        //==============
        //pdv=
        sous_chaine = strtok(NULL, ",");
        if (NULL == strstr(sous_chaine, "pdv=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : pdv=\n", i);
            return status;
        }
        //value
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : pdv=[???]\n", i);
            return status;
        }
        if (NULL != strstr(sous_chaine, "inf")) 
            (*objs)[i].pdv = -1;
        else
            (*objs)[i].pdv = getNbrFromChars(sous_chaine);
            
        //==============
        //direction=
        sous_chaine = strtok(NULL, ",");
        if (NULL == strstr(sous_chaine, "dir=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : dir=\n", i);
            return status;
        }
        //value
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : dir=[???]\n", i);
            return status;
        }
        (*objs)[i].direction = getNbrFromChars(sous_chaine);
            
            
        //==============
        //enabled=
        sous_chaine = strtok(NULL, ",");
        if (NULL == strstr(sous_chaine, "enabled=")) 
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : enabled=\n", i);
            return status;
        }
        //value
        sous_chaine = strtok(NULL, ",");
        if (NULL == sous_chaine)
        {
            fprintf(stderr, "fichier corrompu : lecture interobject ligne %d : enabled=[???]\n", i);
            return status;
        }
        if (NULL != strstr(sous_chaine, "1")) 
            (*objs)[i].enabled = true;
        else
            (*objs)[i].enabled = false;

    } //fin boucle for
    status = EXIT_SUCCESS;
    return status;

}

int objReading(FILE *level_file, char *buf, int buf_len, interobj **objs, int *nb_objs, character **npcs, int *nb_npcs)
{
    int status = EXIT_FAILURE;
    
    //lecture du nb d'interobjects
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "interobjects=,"))
    {
        fprintf(stderr, "Fichier corrompu (interobjects=).\n");
        return status;
    }
    char *sous_chaine;
    sous_chaine = strtok(buf, ",");
    sous_chaine = strtok(NULL, ",");    
    if(NULL == sous_chaine)
    {
        fprintf(stderr, "Fichier corrompu ( interobjects=[aucune valeur] ).\n");
        return status;
    }
    *nb_objs = getNbrFromChars(sous_chaine);
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
    sous_chaine = strtok(buf, ",");
    sous_chaine = strtok(NULL, ",");    
    if(NULL == sous_chaine)
    {
        fprintf(stderr, "Fichier corrompu ( npcs=[aucune valeur] ).\n");
        return status;
    }
    *nb_npcs = getNbrFromChars(sous_chaine);
    
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
    for(int i=0; i<(*nb_npcs); i++)
    {
        (*npcs)[i].obj.type = npc_objs_tmp[i].type;
        
        (*npcs)[i].obj.position.x = npc_objs_tmp[i].position.x;
        (*npcs)[i].obj.position.y = npc_objs_tmp[i].position.y;
        
        (*npcs)[i].obj.collider.x = npc_objs_tmp[i].collider.x;
        (*npcs)[i].obj.collider.y = npc_objs_tmp[i].collider.y;
        (*npcs)[i].obj.collider.w = npc_objs_tmp[i].collider.w;
        (*npcs)[i].obj.collider.h = npc_objs_tmp[i].collider.h;
        
        (*npcs)[i].obj.pdv = npc_objs_tmp[i].pdv;
        
        (*npcs)[i].obj.direction = npc_objs_tmp[i].direction;
        
        (*npcs)[i].obj.enabled = npc_objs_tmp[i].enabled;

    }
    
    free(npc_objs_tmp);
    status = EXIT_SUCCESS;
    return status;
}

int loadLevel(const char* level_filename, int *taille_x, int *taille_y, int **main_tiles_array, 
              int **overlay_tiles_array, interobj **objs, int *nb_objs, character **npcs, int *nb_npcs)
//fonction principale pour le chargement d'un fichier niveau .csv
{
    int status = EXIT_FAILURE;
    int buf_len = 100;
    
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
    *taille_x = 0;
    *taille_y = 0; //variables contenant les tailles du niveau
    if(0 != getLevelSize(buf, taille_x, taille_y))
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
    //(si taille_x dépasse cette valeur alors cela pourrait engendrer des problèmes de 
    //lecture du fichier au niveau de fgets).
    if( *taille_x > (INT_MAX / 4) )
    {
        fprintf(stderr, "Erreur : la taille du niveau dépasse les valeurs admissibles ( *taille_x > (INT_MAX / 4) )\n");
        goto Cleanup_all;
    }
    
    //réalloc de buf car on connait maintenant la taille des lignes
    if( (*taille_x) * 4 > buf_len )
    {
        buf_len = (*taille_x) * 4 + 1; //+le caractere nul
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
    int *tmp = realloc( *main_tiles_array, sizeof( int[*taille_y][*taille_x] ) );
    if (NULL == tmp)
    {
        fprintf(stderr, "erreur realloc *main_tiles_array in fct loadLevel\n");
        goto Cleanup_all;
    }
    *main_tiles_array = tmp;
    
    tmp = realloc( *overlay_tiles_array, sizeof( int[*taille_y][*taille_x] ) );
    if (NULL == tmp)
    {
        fprintf(stderr, "erreur realloc *overlay_tiles_array in fct loadLevel\n");
        goto Cleanup_all;
    }
    *overlay_tiles_array = tmp;
    
    //init a 0
    memset(*main_tiles_array, 0, sizeof( int[*taille_y][*taille_x] ) );
    memset(*overlay_tiles_array, 0, sizeof( int[*taille_y][*taille_x] ) );
    
    //lecture des tuiles du décor et remplissage du tableau main_tiles_array
    tilesReading(*taille_x, *taille_y, level_file, buf, buf_len, main_tiles_array);
    
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
    tilesReading(*taille_x, *taille_y, level_file, buf, buf_len, overlay_tiles_array);
    
    //ligne end_overlay_grid
    lireLigne(level_file, buf, buf_len);
    if(NULL == strstr(buf, "end_overlay_grid"))
    {
        fprintf(stderr, "Fichier corrompu (end_overlay_grid).\n");
        goto Cleanup_all;
    }
    
    //lecture des interobjects
    if ( 0 != objReading(level_file, buf, buf_len, objs, nb_objs, npcs, nb_npcs) )
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

int lireLigne(FILE *fp, char* buf, int taille)
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
