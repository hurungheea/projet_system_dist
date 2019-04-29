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
      if(list_size < NB_CLIENT_MAX)
      {
        if(list_size == 0)
        {
          req = malloc(sizeof(multicast_request_t));
          set_multicast_request(&req[list_size],tmp.pseudo,tmp.addr_client);
          list_size++;
        }

        if((strncmp(req[list_size-1].pseudo,tmp.pseudo,BUF_PSEUDO) != 0)&&(list_size > 0))
        {
          req = realloc(req,((list_size + 1) * sizeof(multicast_request_t)));
          memcpy(&req[list_size],&tmp,sizeof(multicast_request_t));
          list_size++;
        }
        send_tcp_id_list(get_client_id(),list_size,req);
        printf("jouers %s Connecté avec succes\n", req[(list_size - 1)].pseudo);
      }
      else
      {
        send_tcp_id_list(-1,-1,NULL);
        printf("jouers %s n'a pus se connecter :--> PLUS DE PLACE\n",tmp.pseudo);
      }
      if(strcmp(tmp.pseudo,"-1") == 0)
      {
        fin = 1;
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
  char* buffy;
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

  if(list_size == -1)
  {
    status = NOPLACELEFT;
    buffy = (char*)malloc(sizeof(status_t));
    memcpy(buffy,&status,sizeof(status));
  }else
  {
    if(list_size > 1)
    {
      buffy = (char *)malloc((2 * sizeof(int)) + (list_size * sizeof(multicast_request_t)));
      status = CONNECTED;
      memcpy(buffy, &status,sizeof(status_t));
      memcpy(buffy + sizeof(status_t), &id,sizeof(int));
      memcpy((buffy + sizeof(status_t) + sizeof(int)),&list_size,sizeof(int));
      memcpy(buffy + sizeof(status_t) + (2 * sizeof(int)),req,sizeof(*req));
    }
    else
    {
      status = CONNECTED;
      buffy = malloc(sizeof(int) + sizeof(int));
      memcpy(buffy, &status,sizeof(status_t));
      memcpy(buffy + sizeof(status_t),&id,sizeof(int));
      memcpy(buffy + sizeof(status_t) + sizeof(int),&no_client,sizeof(int));
    }
  }

  write(socket_tcp,buffy,BUFFER_TCP_MESSAGE);

  return 0;
}

int get_client_id()
{
  static int id = 0;
  return (++id);
}
