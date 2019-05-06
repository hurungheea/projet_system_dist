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
#include "./headers/chouette-des.h"
#include "./headers/chouette-common.h"
#include "./headers/chouette-joueurs.h"

int wait_client_tcp(joueur_t** j_list, int socket_ecoute);

int main(int argc, char *argv[])
{
  char opt;
  joueur_t* j_list;
  int socket_ecoute = 0,list_size = 0;
  struct sockaddr_in addr_local_tcp;
  multicast_request_t* req;
  socklen_t lg_addr;
  joueur_t local_user;
  req = NULL;
  lg_addr = sizeof(struct sockaddr_in);
  socket_ecoute = creer_socket_tcp(0);

  j_list = calloc((NB_CLIENT_MAX -1),sizeof(joueur_t*));

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

  if(reveive_id_list(socket_ecoute,&local_user, &list_size, &req) == -1)
  {
    display_error(NULL,argv[0],__FILE__,__LINE__);
    exit(EXIT_FAILURE);
  }
  printf("req[0].pseudo : %s\n",req[0].pseudo);
  if(list_size < 0)
    wait_client_tcp(&j_list,socket_ecoute); /* attend les connexion TCP des autres clients */
  else
  {
    if(connect(socket_ecoute,(struct sockaddr*)&req[0].addr_client,lg_addr))
    {
      printf("error connect %d\n",errno);
    }
    write(socket_ecoute,"ok",strlen("ok"));
  }

  close(socket_ecoute);
  free(req);
  free(j_list);
  exit(EXIT_SUCCESS);
 }

 int wait_client_tcp(joueur_t** j_list, int socket_ecoute)
 {
   int socket_service[NB_CLIENT_MAX-1], nb;
   char tampon[BUFFER_TCP_MESSAGE];
   struct sockaddr_in addr_client;
   socklen_t sz;
   nb = 0;
   sz = sizeof(struct sockaddr_in);

   while(nb < (NB_CLIENT_MAX-1))
   {
     socket_service[nb] = accept(socket_ecoute,(struct sockaddr*)&addr_client,&sz);
     if(socket_service[nb] == -1)
     {
       perror("error accept");
     }
     read(socket_service[nb],&tampon,BUFFER_TCP_MESSAGE);
     nb++;
   }
   printf("tampon : %s\n",tampon);
   return 0;
 }
