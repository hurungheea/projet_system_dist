#include "./../headers/chouette-common.h"

void display_error(void* t, ...)
{
  char* toPrint = NULL;
  va_list args;
  switch(errno)
  {
    case 501:
      toPrint = "%s:%s:%d: \033[91m  \033[0m\n";
      break;

    case 502:
      toPrint = "%s:%s:%d: \033[91mImpossible de lire dans la socket TCP.\033[0m.\n";
      break;

    case 503:
      toPrint = "%s:%s:%d: \033[91mErreur lors de la fonction accept.\033[0m.\n";
      break;

    case 504:
      toPrint = "%s:%s:%d: \033[91mErreur lors de la fonction listen.\033[0m.\n";
      break;

    case 505:
      toPrint = "%s:%s:%d: \033[91mErreur rencontré lors de l'envoie multicast.\033[0m.\n";
      break;

    default:
      fprintf(stderr,"Unknown error : %d.\n",errno);
      break;
  }
  va_start(args, t);
  vfprintf(stderr, toPrint,args);
  va_end(args);
}

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
    if(res == -1)
    {
        errno = 505;
        return -1;
    }
  }

  close(socket_multicast);
  if((strcmp(pseudo,"shutdown") == 0) || (strcmp(pseudo,"-1") == 0))
    exit(EXIT_SUCCESS); /* Comportement Attendu */
  return 0;
}

int reveive_id_list(int socket_ecoute,int *id, int* list_size, multicast_request_t **req_ptr)
{
  int socket_service, recv_tcp;
  int i;
  struct sockaddr_in addr_server;
  socklen_t lg_addr;
  char buffer[BUFFER_TCP_MESSAGE];
  status_t tampon;

  bzero(&buffer,BUFFER_TCP_MESSAGE);

  lg_addr = sizeof(struct sockaddr_in);

  /* attende de réponse du serveur */
  if(listen(socket_ecoute,5) == -1)
  {
    errno = 504;
    return -1;
  }

  if((socket_service = accept(socket_ecoute,(struct sockaddr*)&addr_server, &lg_addr)) == -1)
  {
    errno = 503;
    return -1;
  }

  while(recv_tcp != BUFFER_TCP_MESSAGE)
  {
    if((recv_tcp = read(socket_service,buffer,BUFFER_TCP_MESSAGE)) == -1)
    {
      errno = 502;
      return -1;
    }
  }

  memcpy(&tampon,buffer,sizeof(status_t));

  switch(tampon)
  {
    case CONNECTED:
     printf("\033[32mVous êtes bien connecté.\033[0m\n");
     memcpy(id, buffer + sizeof(status_t),sizeof(int));
     printf("received addr id : %d\n",*id);
     memcpy(list_size, buffer + sizeof(status_t) + sizeof(int),sizeof(int));

     if(*list_size > 0)
     {
       *req_ptr = calloc((*list_size),sizeof(multicast_request_t));
       for(i = 0;i < (*list_size);i++)
       {
         req_ptr[i] = calloc(1,sizeof(multicast_request_t));
         memcpy(req_ptr[i],buffer + (sizeof(status_t) + (2 * sizeof(int)) + (i * sizeof(multicast_request_t))),sizeof(multicast_request_t));
       }
     }
     else
       *req_ptr = NULL;
     break;

    case NOPLACELEFT:
      printf("\033[91mImpossible de rejoindre la partie en cours car trop de joueurs sont déjà connectés.\033[0m\n");
      exit(EXIT_FAILURE);

    default:
      printf("Default switch case\n");
      break;
  }

  close(socket_service);
  return 0;
}

int send_tcp_id_list(int id, int list_size , multicast_request_t *req)
{
  int socket_tcp, no_client = -1;
  struct sockaddr_in addr_client;
  char buffy[BUFFER_TCP_MESSAGE];
  status_t status;
  int i = 0;

  bzero(buffy,BUFFER_TCP_MESSAGE);

  /* Ajout d'un nouveau client à la liste */
  bzero((char*)&addr_client,sizeof(struct sockaddr_in));
  addr_client.sin_family = AF_INET;
  addr_client.sin_port = req[(list_size - 1)].addr_client.sin_port;
  memcpy(&addr_client.sin_addr.s_addr,&req[list_size - 1].addr_client.sin_addr.s_addr,sizeof(int));

  socket_tcp = creer_socket_tcp(0);
  /* Connexion au client */
  if(connect(socket_tcp,(struct sockaddr*)&addr_client,sizeof(struct sockaddr_in)) == -1)
  {
    perror("error connect to client");
  }

  /* Si il y a plus de 4 joueurs connectés */
  if(list_size > NB_CLIENT_MAX)
  {
    printf("\033[91mImpossible de connecter le joueur %s, 4 joueurs max en partie.\033[0m\n",req[(list_size - 1)].pseudo);
    status = NOPLACELEFT;
    memcpy(buffy,&status,sizeof(status));
  }
  else
  {
    if(list_size > 1)
    {
      status = CONNECTED;
      memcpy(buffy, &status,sizeof(status_t));
      memcpy(buffy + sizeof(status_t), &id,sizeof(int));
      memcpy((buffy + sizeof(status_t) + sizeof(int)),&list_size,sizeof(int));
      for(i = 0;i < list_size; i++)
      {
        memcpy(buffy + sizeof(status_t) + (2 * sizeof(int)) + (i * sizeof(multicast_request_t)),&req[i],sizeof(multicast_request_t));
      }
    }
    else
    {
      status = CONNECTED;
      memcpy(buffy, &status,sizeof(status_t));
      memcpy(buffy + sizeof(status_t),&id,sizeof(int));
      memcpy(buffy + sizeof(status_t) + sizeof(int),&no_client,sizeof(int));
    }
    affiche_joueur_connecter(req,list_size);
  }

  write(socket_tcp,buffy,BUFFER_TCP_MESSAGE);

  return 0;
}

void affiche_joueur_connecter(multicast_request_t* req,int list_size)
{
  int i;
  system("clear");
  for(i = 0; i < list_size; i++)
  {
    printf("\033[32mLe joueurs %s s'est connecté.\033[0m\n",req[i].pseudo);
  }
}

int get_client_id()
{
  static int id = 0;
  return (++id);
}
