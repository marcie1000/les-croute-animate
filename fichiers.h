#ifndef FICHIERS_H
#define FICHIERS_H

//standard includes
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

//project includes
#include "types_struct_defs.h"

extern int loadLevel(const char* level_filename, game_context *gctx);
extern int lireLigne(FILE *fp, char* buf, size_t taille);
extern int getLevelSize(char *text_line, size_t *taille_x, size_t *taille_y);
extern int getNbrFromChars(char *string);
//extern int getCharsFromNmbr(size_t int value, char *chaine);
extern void tilesReading(size_t taille_x, size_t taille_y, FILE *level_file, char *buf, size_t buf_len, int **tiles_array);
extern int objReading(FILE *level_file, char *buf, size_t buf_len, interobj **objs, size_t *nb_objs, character **npcs, size_t *nb_npcs);
extern int objArrayFill(FILE *level_file, char *buf, size_t buf_len, interobj **objs, size_t nb_objs);

#endif //FICHIERS_H
