CC = gcc -Wall -ggdb

all: send catch

send:
	$(CC) sender.c -o sender

catch:
	$(CC) catcher.c -o catcher

test:
	@echo "Test nr.1 sigqueue"
	@./catcher sigqueue&
	@echo "Catcher work"
	@./sender `catcher's pd` 30 sigqueue

	@echo "Test nr.2 sigrt"
	@./catcher sigrt&
	@echo "Catcher work"
	@./sender `catcher's pd` 30 sigrt

	@echo "Test nr.1 kill"
	@./catcher kill&
	@echo "Catcher work"
	@./sender `catcher's pd` 30 kill

clean:
	rm sender catcher