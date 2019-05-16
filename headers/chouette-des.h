#ifndef CHOUETTE_DES_H
#define CHOUETTE_DES_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define NB_DES 3
#define DES_CHOUETTE 2
#define DES_MIN 1
#define DES_MAX 6

typedef int des_t;

void affiche_des(des_t* des); /* affiche les dès */
void jet_chouette(des_t* des);/* Jet de la chouette */
void jet_cul_chouette(des_t* des);/* Jet du cul */
/* Combinaisons sans interaction */
int velute(des_t* des);
/* Combinaisons avec interaction */
int des_suite(des_t* des);/* Y a-t-il une suite ? 0 : non / 1 : oui */
int chouette_velute(des_t* des); /* y-a-t-il une chouette velute */

#endif
