CC=gcc
CFLAGS=-I.

client: client.c functions.c parson.c helpers.c requests.c buffer.c
	$(CC) -o client client.c functions.c parson.c helpers.c requests.c buffer.c -Wall

run: client
	./client

clean:
	rm -f *.o client