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

void *scan_thread(char* caractere);
void start_game(joueur_t** j_list_ptr,int socket_ecoute, joueur_t* local_user);

int main(int argc, char *argv[])
{
  char opt;
  joueur_t* j_list;
  int socket_ecoute = 0,list_size = 0, id = 0;
  struct sockaddr_in addr_local_tcp;
  multicast_request_t* req;
  socklen_t lg_addr;
  joueur_t local_user;

  bzero(&local_user,sizeof(joueur_t));

  req = NULL;
  lg_addr = sizeof(struct sockaddr_in);
  socket_ecoute = creer_socket_tcp(0);
  j_list = calloc((NB_CLIENT_MAX - 1),sizeof(joueur_t*));

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
    if(wait_client_tcp(&j_list,socket_ecoute, list_size) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    if(connect_all_client(&j_list, &req, local_user, list_size) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
    if(wait_client_tcp(&j_list,socket_ecoute, list_size) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
  }

  start_game(&j_list,socket_ecoute,&local_user);

  close(socket_ecoute);
  free(req);
  free(j_list);
  exit(EXIT_SUCCESS);
 }

 void start_game(joueur_t** j_list_ptr,int socket_ecoute, joueur_t* local_user)
 {
   int horloge_local[NB_CLIENT_MAX] = {0}, i,res = 0;
   status_t status;
   pthread_t sca_thread;
   char tmp[BUFFER_TCP_MESSAGE];
   bzero(&tmp,BUFFER_TCP_MESSAGE);
   char car='\0';
   char *msg = NULL;

   horloge_local[0] = horloge_local[2];

   if(pthread_create(&sca_thread,NULL,(void*)scan_thread,&car) == -1)
   {
     perror("error thread");
   }

  if(pthread_join(sca_thread, NULL))
  {
    perror("pthread_join");
  }

  while((res = (read(socket_ecoute,tmp,BUFFER_TCP_MESSAGE)) < BUFFER_TCP_MESSAGE)||(car == 'o')||(car == 'O')) {}

  if((car == 'o')||(car == 'O'))
  {
    status = MASTER;
    msg = malloc(sizeof(status) + sizeof(int));
    memcpy(msg,&local_user -> id,sizeof(int));
    for(i = 0;i < (NB_CLIENT_MAX -1);i++)
    {
      write(socket_ecoute,msg,sizeof(int));
    }
  }
/*
  memcpy(&status,tmp,sizeof(status_t));
  if(status == MASTER)
  if(pthread_cancel(sca_thread) == -1)
  {
    perror("error killing scan thread");
  }
  */
}

void* scan_thread(char* caractere)
{
  char car = '\0';
  do
  {
    printf("Commencer une nouvelle partie ? (o|N)\n");
    scanf("%c%*c",&car);
  }while((car != 'o')&&(car != 'O')&&(car != 'n')&&(car != 'N'));
  *caractere = car;
  pthread_exit(EXIT_SUCCESS);
}
