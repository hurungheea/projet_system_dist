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

int reveive_id_list(int socket_ecoute, joueur_t* local_user, int* list_size,multicast_request_t *req);

int main(int argc, char *argv[])
{
  char opt;
  int socket_ecoute = 0,list_size;
  struct sockaddr_in addr_local_tcp;
  multicast_request_t* req;
  socklen_t lg_addr;
  joueur_t local_user;
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
    perror("erreur sending multicast  message");
  }

  if(reveive_id_list(socket_ecoute,&local_user, &list_size, req) == -1)
  {
    perror("erreur reveive id list");
  }

  close(socket_ecoute);
  exit(EXIT_SUCCESS);
 }

 int reveive_id_list(int socket_ecoute, joueur_t* local_user, int* list_size, multicast_request_t *req)
 {
   int socket_service, recv_tcp;
   struct sockaddr_in addr_server;
   socklen_t lg_addr;
   char buffer[BUFFER_TCP_MESSAGE];
   status_t tampon;

   lg_addr = sizeof(struct sockaddr_in);

   /* attende de réponse du serveur */
   if(listen(socket_ecoute,5) == -1)
   {
     perror("erreur listen\n");
     return -1;
   }

   if((socket_service = accept(socket_ecoute,(struct sockaddr*)&addr_server, &lg_addr)) == -1)
   {
     perror("error accept");
     return -1;
   }

   while(recv_tcp != BUFFER_TCP_MESSAGE)
   {
     if((recv_tcp = read(socket_service,buffer,BUFFER_TCP_MESSAGE)) == -1)
     {
       perror("error read");
       return -1;
     }
   }

   memcpy(&tampon,buffer,sizeof(int));

   switch(tampon)
   {
     case CONNECTED:
      printf("\033[32mVous êtes bien connecté.\033[0m\n");
      break;

     case NOPLACELEFT:
      printf("\033[91mImpossible de rejoindre la partie en cours car trop de joueurs sont déjà connecter.\033[0m\n");
      break;

     default:
      printf("default\n");
      break;
   }

   close(socket_service);
   return 0;
 }
