#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <wait.h>

#include "./headers/chouette-des.h"
#include "./headers/chouette-common.h"

int main(int argc, char const *argv[])
{

  static struct sockaddr_in addr_client;

  static struct sockaddr_in addr_serveur;
  int nombre_client = 0;
  socklen_t lg_addr;
  int port = 4000;
  int socket_ecoute,socket_service;
  char message[TAILLEBUF];
  char *chaine_recue;
  char *reponse = "bien recu";
  int nb_octets;

  socket_ecoute = creer_socket_tcp(port);
  bzero((char*)&addr_serveur,sizeof(addr_serveur));
  addr_serveur.sin_family = AF_INET;
  addr_serveur.sin_port = htons(port);
  addr_serveur.sin_addr.s_addr = htonl(INADDR_ANY);

  if(listen(socket_ecoute,5) == -1)
  {
    printf("erreur listen %d\n",errno);
    exit(1);
  }

  lg_addr = sizeof(struct sockaddr_in);

  while(1)
  {
    socket_service = accept(socket_ecoute,(struct sockaddr*)&addr_client,&lg_addr);
    if(socket_service == -1)
    {
      perror("erreur accept");
      exit(1);
    }
    else
    {
      nombre_client++;
    }

    if(fork() == 0)
    {
      sleep(5);
      close(socket_ecoute);
      printf("traitement à effectuer\n");
      nb_octets = read(socket_service,message,TAILLEBUF);
      chaine_recue = (char*)malloc(nb_octets * sizeof(char));
      memcpy(chaine_recue,message,nb_octets);
      if(write(socket_service,reponse,strlen(reponse)+1) == -1)
      {
        perror("error\n");
      }
      close(socket_service);
      exit(0);
    }
    wait(0);
  }

  return 0;
}
