#include <stdio.h>
#include <stdlib.h>
#include "./headers/chouette-des.h"
#include "./headers/chouette-common.h"

int main(int argc, char **argv)
{
  des_t d[3];
  int socket_ecoute;

  jet_de_des(d);
  affiche_des(d);

  socket_ecoute = creer_socket_tcp(0);
  printf("socket_ecoute : %d\n",socket_ecoute);

  return 0;
}
