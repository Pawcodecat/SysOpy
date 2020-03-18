CC = gcc
FLAGS = -std=c99 -Wall
LIB_NAME = library
LIB_PATH = ../zad1
TEST_PATH = ../zad2
TEST_SOURCE_NAME = main.c
REPORT_FILE = raport3b.txt
O = O1

static:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a

staticO:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -$O -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a

shared:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -I$(LIB_PATH) -L$(LIB_PATH) -library

sharedO:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -$O -I$(LIB_PATH) -L$(LIB_PATH) -library

dynamicMain: mainDyn.c
	make shared
	$(CC) $(FLAGS) mainDyn.c -ldl -o mainDyn -I

dynamicMainO:
	make CC = "gcc -$O" dynamicMain

cleanAll:
	make cleanAll
	rm -r mainDyn

test:
	LD_LIBRARY_PATH = $(LIB_PATH ) ./main $(shell cat args.txt)

testDyn:
	LD_LIBRARY_PATH = $(LIB_PATH) ./mainDyn $(shell cat args.txt)

testStatic:
	printf "Static library: \n" > report.txt
	make static
	make test
	mv report.txt $(REPORT_FILE)

testShared:
	printf "Shared library:\n" >> report.txt
	make shared
	make test
	mv report.txt $(REPORT_FILE)

testDynamic:
	printf "Dynamic library:\n" >> report.txt
	make dynamicMain
	make testDyn
	mv report.txt $(REPORT_FILE)

testAll:
	printf "Static library:\n" > report.txt
	make static
	make test
	printf "Shared library:\n" >> report.txt
	make shared
	make test
	printf "Dynamic library:\n" >> report.txt
	make dynamicMain
	make testDyn
	mv report.txt $(REPORT_FILE)

test_allO:
	printf "Static library O1:\n" > report.txt
	make staticO O=O1
	make test
	printf "Shared library: O1\n" >> report.txt
	make sharedO O=O1
	make test
	printf "Dynamic library: O1\n" >> report.txt
	make dynamicMainO O=O1
	make testDyn
	printf "Static library O2:\n" >> report.txt
	make staticO O=O2
	make test
	printf "Shared library: O2\n" >> report.txt
	make sharedO O=O2
	make test
	printf "Dynamic library: O2\n" >> report.txt
	make dynamicMainO O=O2
	make testDyn
	printf "Static library: Ofast\n" >> report.txt
	make staticO O=Ofast
	make test
	printf "Static library: Os\n" >> report.txt
	make staticO O=Os
	make test
	printf "Static library: O0\n" >> report.txt
	make staticO O=O0
	make test
	printf "Shared library: O0\n" >> report.txt
	make sharedO O=O0
	make test
	printf "Dynamic library: O0\n" >> report.txt
	make dynamicMainO O=O0
	make testDyn
	mv report.txt $(REPORT_FILE)