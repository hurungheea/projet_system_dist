#ifndef CHOUETTE_COMMON_H
#define CHOUETTE_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

int creer_socket_tcp(int port);

#endif
