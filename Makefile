.PHONY: all

all: build run

build: include/queue.h include/clock.h include/queue.c include/clock.c include/threads.h include/threads.c include/processes.h include/processes.c src/main.c
	mpicc -Wall -o bin/main.out src/main.c include/queue.c include/clock.c include/threads.c include/processes.c -lpthread

run: bin/main.out
	mpirun -np 3 bin/main.out

clean: bin/main.out
	rm bin/main.out

