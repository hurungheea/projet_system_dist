#include "./../headers/chouette-des.h"

void jet_chouette(des_t* des)
{
  srand(time(NULL));
  des[0] = rand()%(6)+1;
  des[1] = rand()%(6)+1;
}

void jet_cul_chouette(des_t* des)
{
  des[2] = rand()%(6)+1;
}

void affiche_des(des_t* des)
{
  printf("(%d,%d,%d)\n",des[0],des[1],des[2]);
}

int des_suite(des_t* des)
{
  return((des[1] == des[0]+1)&&((des[2]==(des[1]+1))));
}

int chouette_velute(des_t* des)
{
  return ((des[0] == des[1])&&(des[2] == (des[0] * 2)));
}
