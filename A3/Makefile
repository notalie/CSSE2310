.PHONY: all clean
.DEFAULT_GOAL := all
CFLAGS = -Wall -pedantic -g -std=gnu99
all: 2310hub 2310alice 2310bob

2310hub: hub.c hub.o inputs.c inputs.o
	gcc hub.o inputs.o -o 2310hub

2310alice: inputs.o player.o alice.o
	gcc $(CFLAGS) player.o inputs.o alice.o -o 2310alice

2310bob: inputs.o player.o bob.o
	gcc $(CFLAGS) player.o inputs.o bob.o -o 2310bob

alice.o: alice.c player.h
	gcc $(CFLAGS) -c alice.c

bob.o: bob.c player.h
	gcc $(CFLAGS) -c bob.c

hub.o: hub.c hub.h structs.h
	gcc $(CFLAGS) -c hub.c

player.o: player.c player.h structs.h
	gcc $(CFLAGS) -c player.c

inputs.o: inputs.c
	gcc $(CFLAGS) -c inputs.c

clean:
	rm -rf *.o *.err *.out
