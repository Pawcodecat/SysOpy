CC=gcc -Wall

all: build test clean

build:
	$(CC) -o main main.c 
	$(CC) -o worker1 worker1.c 
	$(CC) -o worker2 worker2.c 
	$(CC) -o worker3 worker3.c 

test:
	./main

clean:
	rm -f main worker1 worker2 worker3