#include "./headers/chouette-des.h"

int main(int argc, char **argv)
{
  des_t d[3];
  jet_de_des(d);
  affiche_des(d);
  return 0;
}
