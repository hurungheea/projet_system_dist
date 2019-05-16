#ifndef CHOUETTE_COMMON_H
#define CHOUETTE_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include "./chouette-joueurs.h"

#define NB_CLIENT_MAX 4
/* Tailles des différents buffers */
#define BUF_PSEUDO 16
#define BUFFER_TCP_MESSAGE 1024
/* Constantes liées au multicast */
#define MULTICAST_PORT 10119
#define MULTICAST_ADDR "226.224.223.38"
#define MULTICAST_MSG_SZ 128
#define NB_MULTICAST_TRY 1

typedef struct
{
  char pseudo[BUF_PSEUDO];
  struct sockaddr_in addr_client;
} multicast_request_t;

void display_error(void* t, ...);

int get_client_id();
int creer_socket_tcp(int port);
void affiche_joueur_connecter(multicast_request_t* req,int list_size);

struct in_addr get_ip_addr();

int wait_client_tcp(joueur_t* j_list, int* nb_j_list,int socket_ecoute, int list_size, joueur_t local_user);
int connect_all_client(joueur_t* j_list, int* nb_j_list,multicast_request_t** req, joueur_t local_user,int list_size, int socket_ecoute);

int socket_udp_multicast_client(struct sockaddr_in* addr);
int socket_udp_multicast_server(struct sockaddr_in* addr);

int send_multicast_msg(char *pseudo, struct sockaddr_in addr_local_tcp);
void set_multicast_request(multicast_request_t *request,char *pseudo, struct sockaddr_in user);

int send_tcp_id_list(int id, int list_size , multicast_request_t *req);
int receive_id_list(int socket_ecoute, int *id, int* list_size,multicast_request_t **req_ptr);
#endif
