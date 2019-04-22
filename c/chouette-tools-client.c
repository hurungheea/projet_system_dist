#include "./../headers/chouette-tools-client.h"

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
