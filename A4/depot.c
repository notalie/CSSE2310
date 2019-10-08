#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <signal.h>

#define DEPOT_NAME 1
#define CARRIAGE_RETURN_ASCII 13
#define NEWLINE_ASCII 10
#define SPACE_ASCII 32
#define COLON_ASCII 58

/*
	Only three possible modes for executing orders
*/
#define WITHDRAW 0
#define TRANSFER 1
#define DELIVER 2
#define NONE -1 

typedef struct {
	char* name;
	int amount;
} Resource;

typedef struct {
	char* name;
	int port;
	int socket;
} Neighbour; // Basically a depot where I didn't want to use struct everything

typedef struct {
	int key; // Key set to deferred order
	Resource resourceToDefer;
	int mode;
	int amount;
	int location; // Not always needed
} DeferredOrder;

typedef struct {
	char* name;
	Resource** resources; // Has to be an array of Resource pointers for qsort to work
	int amountOfResources;
	int amountOfNeighours;
	int port;
	Neighbour** neighbours;
	DeferredOrder* deferredOrders; // List of deferred orders
} Depot;

typedef enum {    
	OK = 0,                                                                            
    BAD_ARGS = 1,                                      
    INVALID_NAME = 2,                                     
    INVALID_QUANTITY = 3,                                                                           
} Status;                                               
                                                        
/**
    Output error message for status and return status 
*/   
Status show_message(Status s) {                         
    const char* messages[] = {"",                       
            "Usage: 2310depot name {goods qty}\n",
            "Invalid name(s)\n",
            "Invalid quantity\n"
};                              
    fputs(messages[s], stderr);                         
    return s;                                           
} 

Depot depot; // Global Depot for signal handling and thread access

int compare_resource_names(const void* s1, const void* s2) { 
	const Resource* firstResource = *((Resource**)s1);
	const Resource* secondResource = *((Resource**)s2);
    return strcmp(firstResource->name, secondResource->name); 
} 

int compare_neighbour_names(const void* s1, const void* s2) { 
	const Neighbour* firstNeighbour = *((Neighbour**)s1);
	const Neighbour* secondNeighbour = *((Neighbour**)s2);
    return strcmp(firstNeighbour->name, secondNeighbour->name); 
} 

void print_depot_data(Depot depot) {
	qsort((depot.resources), depot.amountOfResources, sizeof(Resource*), 
			compare_resource_names);
	qsort((depot.neighbours), depot.amountOfNeighours, sizeof(Neighbour*), 
			compare_neighbour_names);
	printf("Goods:\n");
	for (int i = 0; i < depot.amountOfResources; i++) {
		if (depot.resources[i]->amount != 0) {
			printf("%s: %d\n", depot.resources[i]->name, depot.resources[i]->amount);
		}
	}
	printf("Neighbours:\n");
	for (int i = 0; i < depot.amountOfNeighours; i++) {
		printf("%s\n", depot.neighbours[i]->name);
	}
}

void signal_handler(int signalNum) {
    if (signalNum == SIGHUP) {
        print_depot_data(depot);
    } 
} 

int contains_banned_char(char* stringToCheck) {
	if (stringToCheck[0] == 0) {
		return INVALID_NAME;
	}
	for (int i = 0; i < strlen(stringToCheck); i++) {
		if (stringToCheck[i] == CARRIAGE_RETURN_ASCII || 
			stringToCheck[i] == NEWLINE_ASCII || 
			stringToCheck[i] == SPACE_ASCII ||
			stringToCheck[i] == COLON_ASCII) {
			return INVALID_NAME;
		}
	}
	return OK;
}

int check_args(int argsNum, char** argsList) {
	char* depotName = argsList[DEPOT_NAME];
	if (argsNum < 2) {
		return BAD_ARGS;
	} else if (contains_banned_char(depotName)) {
		return INVALID_NAME;
	}
	for (int i = 2; i < argsNum; i++) {
		if (i % 2) { // Odd Args - Quantity
			for (int j = 0; j < strlen(argsList[i]); j++) {
				if (argsList[i][j] < '0' || argsList[i][j] > '9') {
					return INVALID_QUANTITY;
				}
			}
			if (atoi(argsList[i]) <= 0) {
				return INVALID_QUANTITY;
			}
		} else { // Even Args - Goods
			if (contains_banned_char(argsList[i])) {
				return INVALID_NAME;
			}
		}
	}

	if (argsNum % 2) { // Odd number of goods to quantity
		return INVALID_QUANTITY;
	}
	return OK;
}

int main(int argc, char** argv) {
	struct sigaction signalHandler;
    signalHandler.sa_handler = signal_handler;   
    signalHandler.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &signalHandler, 0);
    if (check_args(argc, argv)) {
    	return show_message(check_args(argc, argv));
    }
}
