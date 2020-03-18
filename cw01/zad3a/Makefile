CC = gcc
FLAGS = -std=c99 -Wall
LIB_NAME = library
LIB_PATH = ../zad1
TEST_PATH = ../zad2
TEST_SOURCE_NAME = main.c
REPORT_FILE = raport3a.txt

static:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a

shared:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -I$(LIB_PATH) -L$(LIB_PATH) -library

dynamicMain: mainDyn.c
	make shared
	$(CC) $(FLAGS) mainDyn.c -ldl -o mainDyn -I

test: 
	LD_LIBRARY_PATH = $(LIB_PATH) ./main $(shell cat args.txt)

testDyn:
	LD_LIBRARY_PATH = $(LIB_PATH) ./mainDyn $(shell cat args.txt)

testStatic:
	printf "Static library: \n" > report.txt
	make static
	make test
	mv report.txt $(REPORT_FILE)

testShared:
	printf "Shared library:\n >> report.txt
	make shared
	make test
	mv report.txt $(REPORT_FILE)

testDynamic:
	printf "Dynamic library:\n" >> report.txt
	make dynamicMain
	mv report.txt $(REPORT_FILE)

testAll:
	printf "Static library: \n" > report.txt
	make static
	make test
	printf "Shared library: \n" >> report.txt
	make shared
	make test
	printf "Dynamic library: \n" >> report.txt
	make dynamicMain
	make testDyn
	mv report.txt $(REPORT_FILE)

cleanAll:
	make clean
	rm -f main
	rm -f mainDyn
	