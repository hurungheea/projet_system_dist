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
#include "./headers/chouette-joueurs.h"
#include "./headers/chouette-common.h"

int main(int argc, char *argv[])
{
  int socket_multicast;
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

  while (1)
  {
    nbytes = recvfrom(socket_multicast,recv_buffer,MULTICAST_MSG_SZ,0,(struct sockaddr *) &addr,&addrlen);
    if (nbytes == -1)
    {
        perror("recvfrom");
        return 1;
    }
    if(nbytes == MULTICAST_MSG_SZ)
    {
      memcpy(&tmp,recv_buffer,sizeof(multicast_request_t));
      if(list_size == 0)
      {
        req = malloc(sizeof(multicast_request_t));
        memcpy(&req[list_size],&tmp,sizeof(multicast_request_t));
        list_size++;
      }
      if((strncmp(req[list_size-1].pseudo,tmp.pseudo,BUF_PSEUDO) != 0)&&(list_size>0))
      {
        printf("list_size : %d , ",list_size);
        printf("%d\n",list_size+1);
        req = realloc(req,((list_size + 1) * sizeof(multicast_request_t)));
        memcpy(&req[list_size],&tmp,sizeof(multicast_request_t));
        list_size++;
      }
      if(strcmp(tmp.pseudo,"-1") == 0)
      {
        break;
      }
      printf("list_size %d\n", list_size);
    }
  }
  free(req);
  return 0;
}
