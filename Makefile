CC = gcc
gflags = -Wall -Werror -D_GNU_SOURCE
version = -std=c89 -pedantic
exec = chouette-client.out
exes = chouette-server.out

all: $(exec) $(exes)

$(exes): chouette-server.c  chouette-des.o chouette-common.o chouette-joueurs.o
	$(CC) $(version) $(gflags) -g -o $(exes) chouette-server.c c/chouette-des.c c/chouette-common.c c/chouette-joueurs.c

$(exec): chouette-client.c  chouette-des.o chouette-common.o chouette-joueurs.o chouette-tools-client.o
	$(CC) $(version) $(gflags) -g -o $(exec) chouette-client.c c/chouette-des.c c/chouette-common.c c/chouette-joueurs.c c/chouette-tools-client.c

chouette-des.o: c/chouette-des.c
	$(CC) $(version) $(gflags) -g -c -o objets/chouette-des.o c/chouette-des.c

chouette-common.o: c/chouette-common.c
	$(CC) $(version) $(gflags) -g -c -o objets/chouette-common.o c/chouette-common.c

chouette-joueurs.o: c/chouette-joueurs.c
	$(CC) $(version) $(gflags) -g -c -o objets/chouette-joueurs.o c/chouette-joueurs.c

chouette-tools-client.o: c/chouette-tools-client.c
	$(CC) $(version) $(gflags) -g -c -o objets/chouette-tools-client.o c/chouette-tools-client.c

clean:
	rm -f objets/*.o

mrproper:
	rm -f $(exec) $(exes)
	rm -f objets/*.o
