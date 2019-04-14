CC = gcc
gflags = -Wall -Werror -D_GNU_SOURCE
version = -std=c89 -pedantic
exec = chouette-client.out
exes = chouette-server.out

all: $(exec) $(exes)

$(exes): chouette-server.c  chouette-des.o
	$(CC) $(version) $(gflags) -g -o $(exec) chouette-server.c c/chouette-des.c

$(exec): chouette-client.c  chouette-des.o
	$(CC) $(version) $(gflags) -g -o $(exec) chouette-client.c c/chouette-des.c

chouette-des.o: c/chouette-des.c
	$(CC) $(version) $(gflags) -g -c -o objets/chouette-des.o c/chouette-des.c


clean:
	rm -f *.o

mrproper:
	rm -f toto.tok
	rm -f $(exec) $(exes)
	rm -f *.o
