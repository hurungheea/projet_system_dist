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

int main(int argc, char *argv[])
{
  int socket_multicast, fin = 0, nbytes, list_size;
  char recv_buffer[MULTICAST_MSG_SZ];
  multicast_request_t *req;
  multicast_request_t tmp;
  struct sockaddr_in addr;
  socklen_t addrlen;

  list_size = 0;
  addrlen = sizeof(addr);
  socket_multicast = socket_udp_multicast_server(&addr);
  req = malloc(sizeof(multicast_request_t));

  if(bind(socket_multicast,(struct sockaddr*) &addr, sizeof(addr)) == -1)
  {
      perror("bind");
      exit(EXIT_FAILURE);
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
