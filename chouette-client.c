#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "./headers/chouette-des.h"
#include "./headers/chouette-common.h"
#include "./headers/chouette-joueurs.h"

int get_score_max();
void* scan_thread(char* caractere);
int get_score(des_t des[]);
void trie_bulle(int tab[], int taille);
int get_my_order(joueur_t* j, int my_id);
char* get_pseudo(joueur_t* j_list_ptr,int master_id);
void start_game(joueur_t* j_list_ptr,int socket_ecoute, joueur_t* local_user);
void partie(joueur_t* j_list_ptr,int ordre[],joueur_t* local_user,int master_id, int im_master,int score_max);

int main(int argc, char *argv[])
{
  char opt;
  joueur_t j_list[(NB_CLIENT_MAX-1)];
  int socket_ecoute = 0,list_size = 0, id = 0,nb_j_list = 0;
  struct sockaddr_in addr_local_tcp;
  multicast_request_t* req;
  socklen_t lg_addr;
  joueur_t local_user;

  bzero(&local_user,sizeof(joueur_t));
  bzero(j_list,sizeof(*j_list));

  req = NULL;
  lg_addr = sizeof(struct sockaddr_in);
  socket_ecoute = creer_socket_tcp(0);

  if(getsockname(socket_ecoute,(struct sockaddr*)&addr_local_tcp,&lg_addr) == -1)
  {
    perror("get sock name");
  }

  if(argc == 1)
  {
    do
    {
      printf("Choisissez un pseudo : \n");
      scanf("%s",local_user.pseudo);
    }while((strlen(local_user.pseudo)+1) >= BUF_PSEUDO);
  }
  else
  {
    strcpy(&opt,argv[1]);
    strcpy(local_user.pseudo,&opt);
  }

  if(send_multicast_msg(local_user.pseudo, addr_local_tcp) == -1)
  {
    display_error(NULL,argv[0],__FILE__,__LINE__);
    exit(EXIT_FAILURE);
  }

  if(receive_id_list(socket_ecoute,&id, &list_size, &req) == -1)
  {
    display_error(NULL,argv[0],__FILE__,__LINE__);
    exit(EXIT_FAILURE);
  }
  local_user.id = id;

  if(list_size < 0)
  {
    if(wait_client_tcp(j_list,&nb_j_list,socket_ecoute, list_size, local_user) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    if(connect_all_client(j_list,&nb_j_list,&req, local_user, list_size, socket_ecoute) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
    if(wait_client_tcp(j_list,&nb_j_list,socket_ecoute,list_size, local_user) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
  }

  start_game(j_list,socket_ecoute,&local_user);

  close(socket_ecoute);
  free(req);
  exit(EXIT_SUCCESS);
}

void start_game(joueur_t* j_list_ptr,int socket_ecoute, joueur_t* local_user)
{
  char *msg;
  char car = 0;
  status_t st;
  pthread_t sca_thread;
  int ordre[NB_CLIENT_MAX] = {0};
  char tmp[BUFFER_TCP_MESSAGE];
  bzero(tmp,BUFFER_TCP_MESSAGE);
  int i,j=0, res_read = 0, master_id = -1, partie_fini = 0, im_master = 0, score_max = 343;

  ordre[0]=ordre[0];

  if(pthread_create(&sca_thread,NULL,(void*)scan_thread,&car) == -1)
  {
    perror("error thread");
  }

  i=0;

  st = MASTER;
  msg = malloc(sizeof(status_t) + sizeof(int));
  memcpy(msg,&st,sizeof(status_t));
  memcpy(msg + sizeof(status_t),&local_user->id,sizeof(int));

  while(1)
  {
    res_read = read(j_list_ptr[i].socket_client,tmp,BUFFER_TCP_MESSAGE);
    if(res_read == BUFFER_TCP_MESSAGE)
    {
      memcpy(&master_id,tmp + sizeof(status_t),sizeof(int));
      break;
    }

    if((car == 'o')||(car == 'O'))
    {
      for(j = 0;j < (NB_CLIENT_MAX-1); j++)
      {
        write(j_list_ptr[j].socket_client,msg,BUFFER_TCP_MESSAGE);
      }
      im_master = 1;
      break;
    }

    i++;
    if(i == (NB_CLIENT_MAX-1))
      i = 0;
  }
  if(!im_master)
    printf("\033[92mLe joueur %s à lancé une partie.\033[0m\n",get_pseudo(j_list_ptr,master_id));
  else
    printf("\033[92mVous avez décidé de lancer une partie.\033[0m\n");

  while(!partie_fini)
  {
    if(im_master)
    {
      score_max = get_score_max();
      printf("Vous avez choisie : %d.\n",score_max);
      for(i=0;i < (NB_CLIENT_MAX - 1);i++)
      {
        write(j_list_ptr[i].socket_client,&score_max,BUFFER_TCP_MESSAGE);
      }
    }
    else
    {
      bzero(tmp,BUFFER_TCP_MESSAGE);
      i = 0;
      while(read(j_list_ptr[i].socket_client,tmp,BUFFER_TCP_MESSAGE) == -1)
      {
        if(i == NB_CLIENT_MAX)
          i = 0;
        i++;
      }
      memcpy(&score_max,tmp,sizeof(int));
      printf("Le score max choisie par %s est : %d\n",get_pseudo(j_list_ptr,master_id),score_max);
    }
    /* On initialise l'ordre des joueurs */
    ordre[0] = local_user -> id;
    for(i=0;i<(NB_CLIENT_MAX);i++)
    {
      ordre[(i+1)] = j_list_ptr[i].id;
    }
    trie_bulle(ordre,NB_CLIENT_MAX);
    /* Debut de partie */
    partie(j_list_ptr,ordre,local_user,master_id,im_master,score_max);
    sleep(10);
  }

}

void partie(joueur_t* j_list_ptr,int ordre[],joueur_t* local_user,int master_id, int im_master, int score_max)
{
  char* msg;
  int tmp_des[NB_DES];
  char tmp[BUFFER_TCP_MESSAGE];
  bzero((char*)tmp,BUFFER_TCP_MESSAGE);
  int fin = 0,i,j,tour,a_jouer,score_tour;

  while(!fin)
  {
    if(im_master)
    {
      for(i = 0;i < NB_CLIENT_MAX;i++)
      {
        tour = ordre[i];
        jet_chouette(j_list_ptr[tour].des);
        jet_cul_chouette(j_list_ptr[tour].des);
        trie_bulle(j_list_ptr[ordre[i]].des,NB_DES);

        score_tour = get_score(j_list_ptr[ordre[i]].des);

        msg = malloc((5 * sizeof(int)));
        memcpy(msg,&local_user->id,sizeof(int));
        for(j = 0;j < NB_DES; j++)
        {
          memcpy(msg + ((j+1) * sizeof(int)),&j_list_ptr[tour].des[j],sizeof(int));
        }
        memcpy(msg + (4 * sizeof(int)),&score_tour,sizeof(int));

        for(j = 0;j < NB_DES; j++)
        {
          write(j_list_ptr[j].socket_client,msg,BUFFER_TCP_MESSAGE);
        }
      }
    }
    else
    {
      i = 0;
      while(read(j_list_ptr[i].socket_client,tmp,BUFFER_TCP_MESSAGE) == -1)
      {
        if(i == NB_CLIENT_MAX)
          i = 0;
        i++;
      }
      memcpy(&a_jouer,tmp,sizeof(int));
      printf("Le joueur %s à joué : ",get_pseudo(j_list_ptr,a_jouer));

      for(i = 0;i < NB_DES; i++)
      {
        memcpy(&tmp_des[i],tmp + ((i+1) * sizeof(int)),sizeof(int));
      }
      affiche_des(tmp_des);
    }
    tour++;
    if(tour>NB_CLIENT_MAX)
      tour = 0;
    if(score_tour == score_max)
      fin = 1;

    sleep(2);
  }
}

int get_score(des_t des[])
{
  int score = 0;
  if((des[0]+des[1])==(des[2]))
    score += (des[2] * des[2]);
  else if (des[0]==des[1])
    score += (des[0]*des[0]);
  else if ((des[0]==des[1])&&(des[1])==des[2])
  {
    if (des[0]==1)
      score += 50;
    if (des[0]==2)
      score += 60;
    if (des[0]==3)
      score += 70;
    if (des[0]==4)
      score += 80;
    if (des[0]==5)
      score += 90;
    if (des[0]==6)
      score += 100;
  }
  else
    score+=0;
  return score;
}

int get_my_order(joueur_t* j, int my_id)
{
  int i = 0;
  while(j->id != my_id) {i++;}
  return i;
}

void trie_bulle(int tab[],int taille)
{
  int i,j,aux = 0;
  for(i = 0;i < taille;i++)
    for(j = 0;j < taille-1;j++)
      if(tab[j+1] < tab[j])
      {
        aux = tab[j+1];
        tab[j+1] = tab[j];
        tab[j] = aux;
      }
}

int get_score_max()
{
  int val;
  char* buf;
  char** endptr = NULL;
  char* entrer = NULL;
  buf = malloc(4 * sizeof(char));
  do
  {
    printf("Choisissez un score MAX(343) à atteindre : \n");
    if(fgets(buf,5,stdin) != NULL)
    {
      entrer = strchr(buf,'\n');
      if(entrer != NULL)
        *entrer = '\0';
      val = strtol(buf,endptr,10);
    }
    if(val > 343 || val < 0)
      printf("\033[31mLe nombre doit être inferieur à 343 et supérieur à 0.\033[0m\n");
  }while((val > 343)&&(val < 0));
  return (val);
}

char* get_pseudo(joueur_t* j_list_ptr,int master_id)
{
  int i = 0;
  while((j_list_ptr[i].id != master_id)&&(i < NB_CLIENT_MAX))
  {
    i++;
  }
  return (j_list_ptr[i].pseudo);
}

void* scan_thread(char* caractere)
{
  do
  {
    printf("Appuyer sur [ENTRER] pour commencer une nouvelle partie.\n");
  }while(getchar()!=0x0A);
  *caractere = 'o';
  pthread_exit(NULL);
}
