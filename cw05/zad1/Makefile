CC=gcc -Wall


all: build test clean

build: main.c
	$(CC) -o main main.c

test: main 
		./main testFile

clean:
	rm -f main