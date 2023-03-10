#ifndef FICHIERS_H
#define FICHIERS_H

//standard includes
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

//project includes
#include "types_struct_defs.h"

extern int loadLevel(const char* level_filename, int *taille_x, int *taille_y, int **main_tiles_array, 
                     int **overlay_tiles_array, interobj **objs, int *nb_objs, character **npcs, int *nb_npcs);
extern int lireLigne(FILE *fp, char* buf, int taille);
extern int getLevelSize(char *text_line, int *taille_x, int *taille_y);
extern int getNbrFromChars(char *string);
//extern int getCharsFromNmbr(unsigned int value, char *chaine);
extern void tilesReading(int taille_x, int taille_y, FILE *level_file, char *buf, int buf_len, int **tiles_array);
extern int objReading(FILE *level_file, char *buf, int buf_len, interobj **objs, int *nb_objs, character **npcs, int *nb_npcs);
extern int objArrayFill(FILE *level_file, char *buf, int buf_len, interobj **objs, int nb_objs);

#endif //FICHIERS_H
