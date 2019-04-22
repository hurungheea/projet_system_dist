#ifndef CHOUETTE_TOOLS_CLIENT
#define CHOUETTE_TOOLS_CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

#include "./chouette-common.h"

int send_multicast_msg(char *pseudo, struct sockaddr_in addr_local_tcp);

#endif
