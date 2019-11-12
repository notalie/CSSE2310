#ifndef DEPOT_H
#define DEPOT_H

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <signal.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h>
#include <stdbool.h>

#define DEPOT_NAME 0
#define RESOURCE_NAME 2
#define RESOURCE_AMOUNT 1
#define TRANSFER_MESSAGE_SIZE 5
#define DELIVER_OR_WITHDRAW_SIZE 3
#define THIRD_LETTER 2
#define NOTHING 0 
#define USED_KEY -1
#define NO_INDEX -1
#define WITHDRAW_ARGS 3
#define DELIVER_ARGS 3
#define RESOURCE_INDEX 3
#define IM_ARGS_NUM 3
#define PORT_INDEX 1
#define NEIGHBOUR_NAME 2
#define PORT_NUM 1
#define CONNECT_ARGS 2
#define MAX_COLONS 5
// Transfer
#define QUANTITY 1
#define TYPE_OF_GOOD 2
#define TRANSFER_DESTINATION 3
#define TRANSFER_ARGS_NUM 4

// Sanitise Inputs
#define DELIVER_ID 'l'
#define WITHDRAW_ID 't'
#define DEFER_ID 'f'
#define EXECUTE_ID 'e'
#define IM_ID ':'
#define CONNECT_ID 'n'
#define TRANSFER_ID 'a'


/**
    Struct for resouces, contains a name and amount
    Resouces are made each time a resource amount is added or subtracted
*/
typedef struct {
    char* name;
    int amount;
} Resource;

/**
    Struct for neighbours. Contains the port string to allow for duplicate
    checking. The FILE* for writing Deposit messages to and the name of the
    neighbour.
 */
typedef struct {
    char* name;
    FILE* writeFile;
    char* port;
} Neighbour; // Basically a depot where I didn't want to use struct everything


/**
    Struct for deferred orders. Contains a 
    Each time Defer is called, a new deferred order struct will be created
 */
typedef struct {
    int key; // Key set to deferred order
    char* command; // Command - String without Execute:
} DeferredOrder;

/**
    Struct for depots. contains a name of the depot, the amount of resources
    in it, the number of neighbours and the amount of defers which are tracked.
    As well as this, there is an array of resource and neighbour pointers
    which are as they are due to how qsort works.
    There is also a port number for IM messages as well as a list of deferred
    orders to execute each time defer/execute is received a message.
    Contained is also a semaphore as well as the recent file descriptor which
    is locally initialised in each thread.
 */
typedef struct {
    char* name;
    int amountOfResources;
    int amountOfNeighbours;
    int amountOfDefers;
    Resource** resources;
    Neighbour** neighbours;
    unsigned short port;
    DeferredOrder* deferredOrders; // List of deferred orders
    sem_t semaphore;
    FILE* recentWriteFile;
    FILE* recentReadFile;
} Depot;


/**
    Enums for the error statuses. 
 */ 
typedef enum {    
    OK = 0,
    BAD_ARGS = 1,
    INVALID_NAME = 2,
    INVALID_QUANTITY = 3,
} Status;  

/**
    Wrapper for passing into threads, stops the readFd from changing
    Contains a depot pointer and fd to be read from for thread fdopening
*/
typedef struct {
    Depot* depot;
    int fd;
} Param;

Status show_message(Status);
void print_depot_data(void);
void signal_handler(int);
void change_resouce_amount(Depot*, char*, int);
void init_depot(Depot*, int, char**); 
void change_resouce_amount(Depot*, char*, int);
void deliver_protocol(Depot*, char*);
void withdraw_protocol(Depot*, char*);
int get_key(char*);
char* get_command(char*);
void defer_protocol(Depot*, char*);
void execute_orders(Depot*, char*);
void sanitise_inputs_wrapper(Depot*, char*);
void sanitise_inputs(Depot*, char*);
int im_protocol(Depot*, char*, int);
void add_neighbour(Depot*, char*, char*, int);
void* new_connection_loop(void*);
#endif
