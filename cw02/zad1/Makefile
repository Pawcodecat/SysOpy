CC=gcc -Wall

all: build test clean

build: main.c
	$(CC) -o main main.c

test: main
	mkdir data
	@echo '----------------------------------------Number of RECORDS: 20000  SIZE: 1 byte ----------------------------------------'
	@./main generate data/t1 20000 1
	@./main copy data/t1 data/t1_1 20000 1 sys
	@./main copy data/t1 data/t1_2 20000 1 lib
	@./main sort data/t1_1 20000 1 sys
	@./main sort data/t1_2 20000 1 lib
	@echo '----------------------------------------Number of RECORDS: 30000  SIZE: 1 byte ----------------------------------------'
	@./main generate data/t2 30000 1
	@./main copy data/t2 data/t2_1 30000 1 sys
	@./main copy data/t2 data/t2_2 30000 1 lib
	@./main sort data/t2_1 30000 1 sys
	@./main sort data/t2_2 30000 1 lib
	@echo '----------------------------------------Number of RECORDS: 50000  SIZE: 4 bytes ----------------------------------------'
	@./main generate data/t3 50000 4
	@./main copy data/t3 data/t3_1 50000 4 sys
	@./main copy data/t3 data/t3_2 50000 4 lib
	@./main sort data/t3_1 50000 4 sys
	@./main sort data/t3_2 50000 4 lib
	@echo '---------------------------------------- Number of RECORDS:100000  SIZE: 4 bytes ----------------------------------------'
	@./main generate data/t4 100000 4
	@./main copy data/t4 data/t4_1 100000 4 sys
	@./main copy data/t4 data/t4_2 100000 4 lib
	@./main sort data/t4_1 100000 4 sys
	@./main sort data/t4_2 100000 4 lib
	@echo '----------------------------------------Number of RECORDS: 50000  SIZE: 512 bytes ----------------------------------------'
	@./main generate data/t5 50000 512
	@./main copy data/t5 data/t5_1 50000 512 sys
	@./main copy data/t5 data/t5_2 50000 512 lib
	@./main sort data/t5_1 50000 512 sys
	@./main sort data/t5_2 50000 512 lib
	@echo '---------------------------------------- Number of RECORDS:100000  SIZE: 512 bytes ----------------------------------------'
	@./main generate data/t6 100000 512
	@./main copy data/t6 data/t6_1 100000 512 sys
	@./main copy data/t6 data/t6_2 100000 512 lib
	@./main sort data/t6_1 100000 512 sys
	@./main sort data/t6_2 100000 512 lib
	@echo '----------------------------------------Number of RECORDS: 50000  SIZE: 1024 bytes ----------------------------------------'
	@./main generate data/t7 50000 1024
	@./main copy data/t7 data/t7_1 50000 1024 sys
	@./main copy data/t7 data/t7_2 50000 1024 lib
	@./main sort data/t7_1 50000 1024 sys
	@./main sort data/t7_2 50000 1024 lib
	@echo '---------------------------------------- Number of RECORDS:100000  SIZE: 1024 bytes ----------------------------------------'
	@./main generate data/t8 100000 1024
	@./main copy data/t8 data/t8_1 100000 1024 sys
	@./main copy data/t8 data/t8_2 100000 1024 lib
	@./main sort data/t8_1 100000 1024 sys
	@./main sort data/t8_2 100000 1024 lib
	@echo '----------------------------------------Number of RECORDS: 50000  SIZE: 4096 bytes ----------------------------------------'
	@./main generate data/t9 50000 4096
	@./main copy data/t9 data/t9_1 50000 4096 sys
	@./main copy data/t9 data/t9_2 50000 4096 lib
	@./main sort data/t9_1 50000 4096 sys
	@./main sort data/t9_2 50000 4096 lib
	@echo '---------------------------------------- Number of RECORDS:100000  SIZE: 4096 bytes ----------------------------------------'
	@./main generate data/t10 100000 4096
	@./main copy data/t10 data/t10_1 100000 4096 sys
	@./main copy data/t10 data/t10_2 100000 4096 lib
	@./main sort data/t10_1 100000 4096 sys
	@./main sort data/t10_2 100000 4096 lib
	@echo '----------------------------------------Number of RECORDS: 50000  SIZE: 8192 bytes ----------------------------------------'
	@./main generate data/t11 50000 8192
	@./main copy data/t11 data/t11_1 50000 8192 sys
	@./main copy data/t11 data/t11_2 50000 8192 lib
	@./main sort data/t11_1 50000 8192 sys
	@./main sort data/t11_2 50000 8192 lib
	@echo '---------------------------------------- Number of RECORDS:100000  SIZE: 8192 bytes ----------------------------------------'
	@./main generate data/t12 100000 8192
	@./main copy data/t12 data/t12_1 100000 8192 sys
	@./main copy data/t12 data/t12_2 100000 8192 lib
	@./main sort data/t12_1 100000 8192 sys
	@./main sort data/t12_2 100000 8192 lib

clean:
	rm -rf main.o main data

cleanAll:
	rm -rf main.o data wyniki.txt