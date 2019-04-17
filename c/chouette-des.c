#include "./../headers/chouette-des.h"

int jet_de_des(des_t* des)
{
  int i;
  srand(time(NULL));
  for(i = 0; i < NB_DES; i++)
  {
    des[i] = ((rand() % (DES_MAX - DES_MIN + 1)) + DES_MIN);
  }
  return 0;
}

void affiche_des(des_t* des)
{
  int i;
  for(i = 0; i < NB_DES; i++)
    printf("dés n°%d : %hd\n",i+1,des[i]);
}
