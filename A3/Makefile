.PHONY: all clean
.DEFAULT_GOAL := all
CFLAGS = -Wall -pedantic -g -std=gnu99
all: 2310hub 2310alice 2310bob

2310hub: hub.c hub.o inputs.c inputs.o
	gcc hub.o inputs.o -o 2310hub

2310alice: inputs.o player.o
	gcc player.o inputs.o -o 2310alice

2310bob: inputs.o player.o
	gcc player.o inputs.o -o 2310bob

hub.o: hub.c hub.h structs.h
	gcc -c hub.c

player.o: player.c player.h structs.h
	gcc -c player.c

inputs.o: inputs.c
	gcc -c inputs.c

clean:
	rm -rf *.o *.err *.out
