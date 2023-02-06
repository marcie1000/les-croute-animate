#ifndef FICHIERS_H
#define FICHIERS_H

//standard includes
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

//project includes
#include "types_struct_defs.h"

extern int loadLevel(char*, int *, int *, int **, interobj **, int *);
extern int lireLigne(FILE *, char*, int);
extern int getLevelSize(char *, int *, int *);
extern int getNbrFromChars(char *);
extern void tilesReading(int, int, FILE *, char *, int, int **);
extern int objReading(FILE *, char *, int, interobj **, int *);

#endif //FICHIERS_H