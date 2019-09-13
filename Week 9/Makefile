.PHONY: all clean
.DEFAULT_GOAL := all
CFLAGS =  -g -pedantic -Wall
all: alice

alice: fakeAlice.c fakeAlice.h
	gcc $(CFLAGS) fakeAlice.c -o alice

clean:
	rm -rf *.o *.in *.out
