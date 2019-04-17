#include "./../headers/chouette-common.h"

int creer_socket_tcp(int port)
{
  struct sockaddr_in addr_local;
  int sock;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
  {
    perror("erreur création de socket");
    return -1;
  }
  bzero(&addr_local, sizeof(struct sockaddr_in));
  addr_local.sin_family = AF_INET;
  addr_local.sin_port = htons(port);
  addr_local.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind (sock, (struct sockaddr*) &addr_local, sizeof(addr_local))== -1)
  {
    printf("erreur bind socket %d\n",errno);
    return -1;
  }
  return sock;
}
