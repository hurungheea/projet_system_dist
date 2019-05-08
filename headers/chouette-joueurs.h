#ifndef CHOUETTE_JOUEUR_H
#define CHOUETTE_JOUEUR_H

#include <string.h>
#include "./chouette-des.h"

#define BUF_PSEUDO 16

typedef enum
{
  CONNECTED,NOTCONNECTED,NOPLACELEFT,MASTER
}  status_t;

typedef struct
{
  int id;
  int score;
  int socket_send;
  int socket_recv;
  des_t des[NB_DES];
  status_t status;
  char pseudo[BUF_PSEUDO];
} joueur_t;

int joueur_set_process_num(joueur_t* joueur, int id);
int joueur_set_des(joueur_t* joueur, int i, int val);
int joueur_set_pseudo(joueur_t* joueur, char* pseudo);
int joueur_set_score(joueur_t* joueur, char op, int score);

#endif
