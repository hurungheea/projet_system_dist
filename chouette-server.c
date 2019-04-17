#include <wait.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "./headers/chouette-des.h"
#include "./headers/chouette-common.h"

int envoie_id();

int main(int argc, char const *argv[])
{
  static struct sockaddr_in addr_client;
  static struct sockaddr_in addr_serveur;
  int nombre_client = 0, canPlay = 0, client_pid = 0;
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

  if(listen(socket_ecoute,4) == -1)
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
    }
    nombre_client++;
    if(nombre_client > NB_CLIENT_MAX)
    {
      canPlay = 0;
      nombre_client--;
    }
    else
      canPlay = 1;

    if(fork() == 0)
    {
      if(canPlay)
      {
        close(socket_ecoute);
        printf("traitement à effectuer : %d ,%d\n",(int)getpid(),nombre_client);
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
      else
      {
        reponse = "TROP DE MONDE";
        write(socket_service,reponse,strlen(reponse)+1);
        close(socket_ecoute);
        close(socket_service);
        exit(0);
      }
    }
    wait(0);
  }

  return 0;
}
