#ifndef SOCKETS_H
#define SOCKETS_H

#include "depot.h"
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int listen_on_port(int*, Depot*);
int connect_to_port(char*, int*);

#define MAX_CONNECTIONS 10

#endif


