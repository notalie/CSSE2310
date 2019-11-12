#include "sockets.h"

/**
    Listens on a port
    Params: Takes in an int pointer which will be the fd for the file socket
    Returns 1 if there is an error and 0 if there is no error
*/
int listen_on_port(int* sockFd, Depot* depot) {
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;  // IPv6  for generic could use AF_UNSPEC
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  // Because we want to bind with it
    // no particular port but on address localhost
    if (getaddrinfo("127.0.0.1", 0, &hints, &ai)) { 
        freeaddrinfo(ai);
        return 1;   // could not work out the address
    }
    // create a socket and bind it to a port
    *sockFd = socket(AF_INET, SOCK_STREAM, 0); // 0 == use default protocol
    if (bind(*sockFd, (struct sockaddr*)ai->ai_addr, 
            sizeof(struct sockaddr))) {
        return 1;
    }
    // Which port did we get?
    struct sockaddr_in ad;
    memset(&ad, 0, sizeof(struct sockaddr_in));
    socklen_t lengthOfSockaddr = sizeof(struct sockaddr_in);
    if (getsockname(*sockFd, (struct sockaddr*)&ad, &lengthOfSockaddr)) {
        return 1;
    }
    
    // allow up to 10 connection requests to queue
    if (listen(*sockFd, MAX_CONNECTIONS)) { 
        return 1;
    }
    printf("%u\n", ntohs(ad.sin_port)); // Prints port that it is listening to
    fflush(stdout);
    depot->port = ntohs(ad.sin_port);
    return 0;
}

/**
    Connects to the port specified by the passed in string. 
    If it is successful, return 0, if it is not, return 1.
    Params: string of port to connect to, int pointer to set the socket fd to
    Returns 1 if there is an error and 0 is there is no error
*/
int connect_to_port(char* port, int* fd) {
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;      
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo("127.0.0.1", port, &hints, &ai)) {
        freeaddrinfo(ai);
        return 1;   // could not work out the address
    }
    *fd = socket(AF_INET, SOCK_STREAM, 0); // 0 == use default protocol
    if (connect(*fd, (struct sockaddr*)ai->ai_addr, sizeof(struct sockaddr))) {
        return 1;
    }
    return 0;
}
