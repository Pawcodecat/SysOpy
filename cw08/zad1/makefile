
all:
	make clean
	gcc -o main main.c -lpthread -lm
	make test_sign
	make test_block
	make test_interleaved

test_sign:
	./main 1 sign feep.ascii.pgm output.txt
	./main 2 sign feep.ascii.pgm output.txt
	./main 4 sign feep.ascii.pgm output.txt
	./main 8 sign feep.ascii.pgm output.txt

test_block:
	./main 1 block feep.ascii.pgm output.txt
	./main 2 block feep.ascii.pgm output.txt
	./main 4 block feep.ascii.pgm output.txt
	./main 8 block feep.ascii.pgm output.txt
test_interleaved:
	./main 1 interleaved feep.ascii.pgm output.txt
	./main 2 interleaved feep.ascii.pgm output.txt
	./main 4 interleaved feep.ascii.pgm output.txt
	./main 8 interleaved feep.ascii.pgm output.txt
clean:
	rm -f main output.txt Times.txt