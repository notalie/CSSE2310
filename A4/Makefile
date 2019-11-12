.PHONY: all clean
.DEFAULT_GOAL := all
CFLAGS = -Wall -pedantic -g -std=gnu99 -pthread
all: 2310depot depot.o

2310depot: depot.o utils.o sockets.o
	gcc depot.o utils.o sockets.o $(CFLAGS) -o 2310depot

sockets.o: sockets.c sockets.h
	gcc $(CFLAGS) -c sockets.c

depot.o: depot.c depot.h
	gcc $(CFLAGS) -c depot.c

utils.o: utils.c utils.h
	gcc $(CFLAGS) -c utils.c

clean:
	rm -rf *.o *.err *.out
