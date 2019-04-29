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
#include "./chouette-joueurs.h"

#define BUF_PSEUDO 16

typedef struct
{
  char pseudo[BUF_PSEUDO];
  struct sockaddr_in addr_client;
} multicast_request_t;

#define NB_CLIENT_MAX 4
#define MULTICAST_PORT 10119
#define MULTICAST_ADDR "226.1.2.3"
#define MULTICAST_MSG_SZ 128
#define NB_MULTICAST_TRY 1
#define BUFFER_TCP_MESSAGE 128


int creer_socket_tcp(int port);

int socket_udp_multicast_client(struct sockaddr_in* addr);
int socket_udp_multicast_server(struct sockaddr_in* addr);

int send_multicast_msg(char *pseudo, struct sockaddr_in addr_local_tcp);
void set_multicast_request(multicast_request_t *request,char *pseudo, struct sockaddr_in user);
#endif
