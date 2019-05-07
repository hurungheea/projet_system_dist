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

int wait_client_tcp(joueur_t** j_list, int socket_ecoute, int list_size);
int connect_all_client(joueur_t** j_list, multicast_request_t** req, joueur_t local_user,int list_size);

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
  printf("pseudo addr av recv : %p\n",(void*)&local_user);
  if(reveive_id_list(socket_ecoute,&local_user, &list_size, &req) == -1)
  {
    display_error(NULL,argv[0],__FILE__,__LINE__);
    exit(EXIT_FAILURE);
  }
  printf("pseudo.id ap recv : %d\n",local_user.id);

  if(list_size < 0)
  {
    if(wait_client_tcp(&j_list,socket_ecoute, list_size) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    printf("pseudo.id main : %d\n",local_user.id);
    if(connect_all_client(&j_list, &req, local_user, list_size) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
    if(wait_client_tcp(&j_list,socket_ecoute, list_size) == -1) /* attend les connexion TCP des autres clients */
    {
      exit(EXIT_FAILURE);
    }
  }

  close(socket_ecoute);
  free(req);
  free(j_list);
  exit(EXIT_SUCCESS);
 }

int wait_client_tcp(joueur_t** j_list_ptr, int socket_ecoute, int list_size)
{
  int i = 0;
  char tampon[BUFFER_TCP_MESSAGE];
  struct sockaddr_in addr_client;
  socklen_t size_addr;
  int id_test;
  char pseudo[BUF_PSEUDO];

  bzero(&tampon,BUFFER_TCP_MESSAGE);

  size_addr = sizeof(struct sockaddr_in);

  if(list_size == -1)
    list_size = 1;

  while(i < ((NB_CLIENT_MAX -1) - (list_size -1)))
  {
   j_list_ptr[i] = calloc(1,sizeof(joueur_t));
   j_list_ptr[i] -> socket_recv = accept(socket_ecoute,(struct sockaddr*) &addr_client, &size_addr);
   if(j_list_ptr[i] -> socket_recv == -1)
   {
     perror("error accept");
   }
   read(j_list_ptr[i]->socket_recv,&tampon,BUFFER_TCP_MESSAGE);
   memcpy(&id_test,tampon,sizeof(int));
   memcpy(pseudo,tampon + sizeof(int),BUF_PSEUDO);

   i++;
  }
  printf("3 clients sont connectés\n");
  return 0;
}

int connect_all_client(joueur_t** j_list_ptr, multicast_request_t** req, joueur_t local_user,int list_size)
{
  int i;
  socklen_t lg_addr;
  char* buffy;

  printf("pseudo.id connect : %d\n",local_user.id);
  buffy = (char*)malloc(sizeof(int) + (BUF_PSEUDO * sizeof(char)));
  lg_addr = sizeof(struct sockaddr);

  memcpy(buffy,&local_user.id,sizeof(int));
  memcpy(buffy + sizeof(int),&local_user.pseudo,BUF_PSEUDO);

  for(i = 0; i < (list_size-1); i++)
  {
    printf("pseudo : %s\n",req[i]->pseudo);
    j_list_ptr[i] = calloc(1,sizeof(joueur_t));
    j_list_ptr[i] -> socket_send = creer_socket_tcp(0);
    if(connect(j_list_ptr[i] -> socket_send,(struct sockaddr*)&req[i]->addr_client,lg_addr))
    {
      perror("error connect");
    }
    write(j_list_ptr[i] -> socket_send,buffy,BUFFER_TCP_MESSAGE);
  }
  return 0;
}
