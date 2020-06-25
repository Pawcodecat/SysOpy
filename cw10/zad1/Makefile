CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -pthread

all:
	# make clean
	make server
	make client
	./server 8080 socket

client:
	$(CC) $(CFLAGS) -o client client.c

server:
	$(CC) $(CFLAGS) -o server server.c

client1:
	./client Pawel local socket

client2:
	./client Piotr local socket

client1_host:
	./client Pawel xxx 8080

client2_host:
	./client Tomek xxx 8080

clean:
	rm -f $(obj) client server socket