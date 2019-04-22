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
#include "./headers/chouette-tools-client.h"

int main(int argc, char *argv[])
{

  int finis = 0, socket_tcp;
  struct sockaddr_in addr_local_tcp;
  joueur_t local_user;
  socket_tcp = creer_socket_tcp(0,&addr_local_tcp);

  do
  {
    printf("Choisissez un pseudo : \n");
    scanf("%s",local_user.pseudo);
  }while((strlen(local_user.pseudo)+1) >= BUF_PSEUDO);

  if(send_multicast_msg(local_user.pseudo, addr_local_tcp) == -1)
  {
    perror("erreur sending multicast  message");
  }

  

  exit(EXIT_SUCCESS);
 }
