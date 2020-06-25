CC = gcc -Wall -ggdb

all: exec

exec: 
	$(CC) main.c -o main
	$(CC) compare.c -o compare
	$(CC) helper.c -o helper

clean:
	rm helper compare main

test: all
	./main 