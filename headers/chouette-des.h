#ifndef CHOUETTE_DES_H
#define CHOUETTE_DES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_DES 3
#define DES_MIN 1
#define DES_MAX 6

typedef short int des_t;

int jet_de_des(des_t* des);
void affiche_des(des_t* des);

#endif
