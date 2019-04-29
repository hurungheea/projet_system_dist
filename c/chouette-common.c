#include "./../headers/chouette-common.h"

int creer_socket_tcp(int port)
{
  int sock;
  struct sockaddr_in addr_local_tcp;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
  {
    perror("erreur création de socket");
    return -1;
  }

  bzero((char*)&addr_local_tcp, sizeof(struct sockaddr_in));
  addr_local_tcp.sin_family = AF_INET;
  addr_local_tcp.sin_port = htons(port);
  addr_local_tcp.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind (sock, (struct sockaddr*) &addr_local_tcp, sizeof(addr_local_tcp))== -1)
  {
    printf("erreur bind socket %d\n",errno);
    return -1;
  }
  return sock;
}

void set_multicast_request(multicast_request_t *request,char *pseudo, struct sockaddr_in user)
{
  strncpy(request -> pseudo, pseudo,BUF_PSEUDO);
  request -> addr_client.sin_family = user.sin_family;
  request -> addr_client.sin_port = user.sin_port;
  request -> addr_client.sin_addr.s_addr = user.sin_addr.s_addr;
}

int socket_udp_multicast_client(struct sockaddr_in* addr)
{
  int sock;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1)
  {
    perror("socket");
    return -1;
  }

  bzero((char *)addr, sizeof(addr));
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
  addr->sin_port = htons(MULTICAST_PORT);

  return sock;
}

int socket_udp_multicast_server(struct sockaddr_in* addr)
{
  int sock, reuse;
  struct ip_mreq mreq;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1)
  {
    perror("socket");
    return -1;
  }

  reuse = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse)) == -1)
  {
     perror("Reusing ADDR failed");
     return 1;
  }

  bzero((char *)addr, sizeof(*addr));

  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
  addr->sin_port = htons(MULTICAST_PORT);

  mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) == -1)
  {
      perror("setsockopt");
      return 1;
  }

  return sock;
}

int send_multicast_msg(char *pseudo, struct sockaddr_in addr_local_tcp)
{
  int socket_multicast, res = 0, i = 0;
  struct sockaddr_in addr_local_multicast;
  multicast_request_t req;

  socket_multicast = socket_udp_multicast_client(&addr_local_multicast);

  set_multicast_request(&req,pseudo,addr_local_tcp);

  for(i = 0; i < NB_MULTICAST_TRY; i++)
  {
    res = sendto(socket_multicast,&req,MULTICAST_MSG_SZ,0,(struct sockaddr*) &addr_local_multicast,sizeof(addr_local_multicast));
    if (res == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
  }
  close(socket_multicast);
  return 0;
}
