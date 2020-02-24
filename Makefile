CC=gcc
CFLAGS=-I.

client: client.c helpers.c parson.c
		$(CC) -o client client.c helpers.c parson.c -Wall
		
run: client
		./client

clean:
		rm -f *.o client
