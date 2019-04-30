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

int get_client_id();
void affiche_joueur_connecter(multicast_request_t* req,int list_size);
int send_tcp_id_list(int id, int list_size , multicast_request_t *req);

int main(int argc, char *argv[])
{
  int socket_multicast, fin = 0;
  struct sockaddr_in addr;
  char recv_buffer[MULTICAST_MSG_SZ];
  multicast_request_t *req;
  multicast_request_t tmp;
  int list_size;
  socklen_t addrlen;
  int nbytes;

  list_size = 0;
  addrlen = sizeof(addr);
  socket_multicast = socket_udp_multicast_server(&addr);
  req = malloc(sizeof(multicast_request_t));

  if(bind(socket_multicast,(struct sockaddr*) &addr, sizeof(addr)) == -1)
  {
      perror("bind");
      return 1;
  }

  while (!fin)
  {
    nbytes = recvfrom(socket_multicast,recv_buffer,MULTICAST_MSG_SZ,0,(struct sockaddr *) &addr,&addrlen);
    if(nbytes == -1)
    {
        perror("recvfrom");
        return 1;
    }

    if(nbytes == MULTICAST_MSG_SZ)
    {
      memcpy(&tmp,recv_buffer,sizeof(multicast_request_t));

      if((strcmp(tmp.pseudo,"shutdown") == 0)||(strcmp(tmp.pseudo,"-1") == 0))
      {
        fin = 1;
      }
      else if(strncmp(req[list_size-1].pseudo,tmp.pseudo,BUF_PSEUDO) != 0)
      {
        req = realloc(req,((list_size + 1) * sizeof(multicast_request_t)));
        memcpy(&req[list_size],&tmp,sizeof(multicast_request_t));
        list_size++;
        send_tcp_id_list(get_client_id(),list_size,req);
      }
    }
  }
  free(req);
  return 0;
}

int send_tcp_id_list(int id, int list_size , multicast_request_t *req)
{
  int socket_tcp, no_client = -1;
  struct sockaddr_in addr_client;
  char buffy[BUFFER_TCP_MESSAGE];
  status_t status;
  /* Ajout d'un nouveau client à la liste */
  bzero((char*)&addr_client,sizeof(struct sockaddr_in));
  addr_client.sin_family = AF_INET;
  addr_client.sin_port = req[(list_size - 1)].addr_client.sin_port;
  memcpy(&addr_client.sin_addr.s_addr,&req[list_size - 1].addr_client.sin_addr.s_addr,sizeof(int));

  socket_tcp = creer_socket_tcp(0);
  /* Connexion au client */
  if(connect(socket_tcp,(struct sockaddr*)&addr_client,sizeof(struct sockaddr_in)) == -1)
  {
    perror("error connect to client");
  }

  /* Si il y a plus de 4 joueurs connectés */
  if(list_size > NB_CLIENT_MAX)
  {
    printf("\033[91mImpossible de connecter le joueur %s, 4 joueurs max en partie.\033[0m\n",req[(list_size - 1)].pseudo);
    status = NOPLACELEFT;
    memcpy(buffy,&status,sizeof(status));
  }
  else
  {
    if(list_size > 1)
    {
      status = CONNECTED;
      memcpy(buffy, &status,sizeof(status_t));
      memcpy(buffy + sizeof(status_t), &id,sizeof(int));
      memcpy((buffy + sizeof(status_t) + sizeof(int)),&list_size,sizeof(int));
      memcpy(buffy + sizeof(status_t) + (2 * sizeof(int)),req,sizeof(*req));
    }
    else
    {
      status = CONNECTED;
      memcpy(buffy, &status,sizeof(status_t));
      memcpy(buffy + sizeof(status_t),&id,sizeof(int));
      memcpy(buffy + sizeof(status_t) + sizeof(int),&no_client,sizeof(int));
    }
    affiche_joueur_connecter(req,list_size);
  }

  write(socket_tcp,buffy,BUFFER_TCP_MESSAGE);

  return 0;
}

void affiche_joueur_connecter(multicast_request_t* req,int list_size)
{
  int i;
  system("clear");
  for(i = 0; i < list_size; i++)
  {
    printf("\033[32mLe joueurs %s s'est connecté.\033[0m\n",req[i].pseudo);
  }
}

int get_client_id()
{
  static int id = 0;
  return (++id);
}
