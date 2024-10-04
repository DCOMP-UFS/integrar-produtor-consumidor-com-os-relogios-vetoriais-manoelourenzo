.PHONY: all

all: build run

build: include/queue.h include/clock.h include/threads.h include/processes.h src/queue.c src/clock.c src/threads.c src/processes.c src/main.c
	mpicc -Wall -o bin/main.out src/main.c src/queue.c src/clock.c src/threads.c src/processes.c -lpthread -I./include

run: bin/main.out
	mpirun -np 3 bin/main.out

clean: bin/main.out
	rm bin/main.out

