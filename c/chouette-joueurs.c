#include "./../headers/chouette-joueurs.h"

int joueur_set_process_num(joueur_t* joueur, int id)
{
  if(id <= 0 || sizeof(id) != sizeof(int))
    return -1;
  joueur -> id = id;
  return 0;
}

int joueur_set_des(joueur_t* joueur, int i, int val)
{
  if(val > DES_MAX || val < DES_MIN)
    return -1;
  if(i > NB_DES || i < 0)
    return -1;
  joueur -> des[i] = val;
  return 0;
}

int joueur_set_pseudo(joueur_t* joueur, char* pseudo)
{
  if(strlen(pseudo) > TAILLEBUF)
    return -1;
  strcpy(joueur -> pseudo,pseudo);
  return 0;
}

int joueur_set_score(joueur_t* joueur, char op, int score)
{
  switch (op)
  {
    case '+':
      joueur -> score += score;
      break;

    case '-':
      joueur -> score -= score;
      break;

    case 'i':
      joueur -> score = 0;
      break;

    default:
      return -1;
  }
  return 0;
}
