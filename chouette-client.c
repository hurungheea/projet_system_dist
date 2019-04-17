#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include "./headers/chouette-des.h"
#include "./headers/chouette-joueurs.h"
#include "./headers/chouette-common.h"

int main(int argc, char *argv[])
{
  static struct sockaddr_in addr_serveur;
  struct hostent* host_serveur;
  char* name_tmp;
  joueur_t player;
  int sock, port = 4000;
  char* hostname = "localhost";
  char *msg = "bonjour";
  char buffer[TAILLEBUF];
  int nb_octets;

  sock = creer_socket_tcp(0);
  if(sock == -1)
  {
    perror("erreur création socket");
    exit(1);
  }

  host_serveur = gethostbyname(hostname);
  if (host_serveur == NULL)
  {
    perror("erreur adresse serveur");
    exit(1);
  }

  bzero((char*)&addr_serveur, sizeof(struct sockaddr_in));
  addr_serveur.sin_family = AF_INET;
  addr_serveur.sin_port = htons(port);
  memcpy(&addr_serveur.sin_addr.s_addr, host_serveur -> h_addr, host_serveur -> h_length);

  if(connect(sock,(struct sockaddr*)&addr_serveur,sizeof(struct sockaddr_in)) == -1)
  {
    printf("erreur connexion serveur %d\n",errno);
    exit(1);
  }

  do
  {
    printf("Quel pseudo voulez-vous ? : ");
    scanf("%s",name_tmp);
  }while(joueur_set_pseudo(&player,name_tmp) == 0);

  nb_octets = write(sock,msg,strlen(msg)+1);
  if(nb_octets == -1)
  {
    printf("erreur write\n");
    exit(1);
  }

  nb_octets = read(sock,buffer,TAILLEBUF);
  if(nb_octets == -1)
  {
    printf("erreur read\n");
    exit(1);
  }

  printf("reponse reçu : %s\n",buffer);

  close(sock);

  return 0;
}
