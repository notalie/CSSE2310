#include "utils.h"     
#include "sockets.h"                                 

/* 
    Global variable for use in signal handler functions
*/          
Depot depot;

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

/*
    Signal handler function that 
*/
void signal_handler(int signalNum) {
    if (signalNum == SIGHUP) {
        sem_wait(&depot.semaphore);
        print_depot_data();
        sem_post(&depot.semaphore);
    } 
} 

/**
    Prints the existing depot data. This method is only called when SIGHUP
    is received. First all the resources will be sorted using qsort 
    (see compare_resouce_names and compare_resource_names for proper 
    function description)
*/
void print_depot_data(void) {
    qsort((depot.resources), depot.amountOfResources, sizeof(Resource*), 
            compare_resource_names);
    qsort((depot.neighbours), depot.amountOfNeighbours, sizeof(Neighbour*), 
            compare_neighbour_names);
    printf("Goods:\n");
    fflush(stdout);
    for (int i = 0; i < depot.amountOfResources; i++) {
        if (depot.resources[i]->amount != 0) { // Don't print if amount if 0
            printf("%s %d\n", depot.resources[i]->name, 
                    depot.resources[i]->amount);
            fflush(stdout);
        }
    }
    printf("Neighbours:\n");
    fflush(stdout);
    for (int i = 0; i < depot.amountOfNeighbours; i++) {
        printf("%s\n", depot.neighbours[i]->name);
        fflush(stdout);
    }
}

/**
    Initialises the depot passed in using the arguments passed in.
    Also makes sure if there are no quantity/goods passed in
    Params: Depot pointer to initialise, the amount of args,
    the list of arg strings.

*/
void init_depot(Depot* depot, int argsNum, char** args) {
    depot->name = args[1];
    int counter = 0;
    if (argsNum > 1) { // Quantity and Amount is in here
    // Amount of resources is without the executable name and depot name
        depot->amountOfResources = (argsNum - 2) / 2;
        depot->resources = malloc(sizeof(Resource*) * 
                depot->amountOfResources);
        depot->neighbours = malloc(sizeof(Neighbour*));
        depot->deferredOrders = malloc(sizeof(DeferredOrder));

        for (int i = RESOURCE_INDEX; i < argsNum; i += 2) {
            depot->resources[counter] = malloc(sizeof(Resource));
            depot->resources[counter]->amount = atoi(args[i]);
            depot->resources[counter]->name = args[i - 1];
            counter++;
        }
    } else { // No resources
        depot->amountOfResources = 0;
    }
    depot->amountOfNeighbours = 0;
    depot->amountOfDefers = 0;
    sem_t semaphore;
    sem_init(&semaphore, 1, 1); 
    depot->semaphore = semaphore;
}

/**
    Changes the resouce amount. Used for withdraw, transfer and 
        deposit functions
    Params: Depot pointer to change, resource name to change the amount of 
        and the amount to change the resource name by
*/
void change_resouce_amount(Depot* depot, char* resourceName, int amount) {
    int resourceIndex = NO_INDEX;
    for (int i = 0; i < depot->amountOfResources; i++) {
        if (!strcmp(depot->resources[i]->name, resourceName)) {
            resourceIndex = i;
            break;
        }
    }
    // Check that the index has been initialised
    if (resourceIndex != NO_INDEX && !contains_banned_char(resourceName)) {
        depot->resources[resourceIndex]->amount += amount;
    } else if (!contains_banned_char(resourceName)) {
        (depot->amountOfResources)++;

        depot->resources = realloc(depot->resources, sizeof(Resource*) * 
                depot->amountOfResources);

        depot->resources[depot->amountOfResources - 1] = 
                malloc(sizeof(Resource));

        depot->resources[depot->amountOfResources - 1]->name = resourceName;
        depot->resources[depot->amountOfResources - 1]->amount = amount; 
    }
}   

/**
    Delivery protocol, checks the string and makes sure that the amout
    to check is greater than 0 and contains 'Deliver' in the message,
        if the message is invalid, don't do anything, if it is,
        add the amount of the resouce to the depot
    Params: Depot pointer and message to check against
*/
void deliver_protocol(Depot* depot, char* message) {
    int arraySize;
    if (!strncmp(message, "Deliver:", strlen("Deliver:")) && 
            strlen(message) > strlen("Deliver:")) {
        char** result = string_split(message, ':', &arraySize);
        // Valid Amount
        if (atoi(result[RESOURCE_AMOUNT]) > 0 && arraySize == DELIVER_ARGS) {
            change_resouce_amount(depot, result[RESOURCE_NAME], 
                    atoi(result[RESOURCE_AMOUNT]));
        }
    }
}

/**
    Withdraw protocol, checks the string and makes sure that the amout
    to check is greater than 0 and contains 'Deliver' in the message,
        if the message is invalid, don't do anything, if it is,
        add the amount of the resouce to the depot
    Params: Depot pointer and message to check against
*/
void withdraw_protocol(Depot* depot, char* message) {
    int arraySize;
    if (!strncmp(message, "Withdraw:", strlen("Withdraw:")) && 
            strlen(message) > strlen("Withdraw:")) {
        char** result = string_split(message, ':', &arraySize);
        // Valid Amount
        if (atoi(result[RESOURCE_AMOUNT]) > 0 && arraySize == WITHDRAW_ARGS) {
            change_resouce_amount(depot, result[RESOURCE_NAME], 
                    atoi(result[RESOURCE_AMOUNT]) * -1);
                    // Want to make negative amount to take away
        }
    }
}

/**
    Defer protocol, adds a deferred message to the depot ready to be executed
    If the key or the message is invalid (not equal to "Defer:"" or key <= 0),
        do nothing
    Params: Depot pointer and message to check against
*/
void defer_protocol(Depot* depot, char* message) {
    if (!strncmp(message, "Defer:", strlen("Defer:")) 
            && strlen(message) > strlen("Defer:")) {
        // Check if the message is either a deliver/withdraw or 
        // transfer message and that the key is greater than 0 (unsigned)
        if (get_key(message) > 0) {
            (depot->amountOfDefers)++;
            depot->deferredOrders = realloc(depot->deferredOrders, 
                    sizeof(DeferredOrder) * depot->amountOfDefers);
            depot->deferredOrders[depot->amountOfDefers - 1].key = 
                    get_key(message);

            depot->deferredOrders[depot->amountOfDefers - 1].command =  
                    get_command(message);
        }
    }  
}

/**
    Function that executes all the orders with the corresponding key 
    by executing the string/command associated with each deferred order
        Once the 
    Params: Depot pointer and message to check against
*/
void execute_orders(Depot* depot, char* message) {
    int key = get_key(message);
    if (!strncmp(message, "Execute:", strlen("Execute:")) && 
            strlen(message) > strlen("Execute:")) {
        for (int i = 0; i < depot->amountOfDefers; i++) {
            if (depot->deferredOrders[i].key == key) {
                // Execute the order using the normal command
                sanitise_inputs(depot, depot->deferredOrders[i].command);
                depot->deferredOrders[i].key = USED_KEY;
            }
        }
    } 
}

/**
    Add the neighbours to the depot pointer if there is no existing neighbour.
    If there is an existing neighbour, do not add it to the 
    existing neighbours. Also does not add the neighbour if it is has
        invalid characters.
    Params: depot pointer, neighbour name string, port name string, 
        file descriptor to open
*/
void add_neighbour(Depot* depot, char* neighbourName, char* port, int fd) {
    // If neighbour not in list, add it
    if (!contains_banned_char(neighbourName)) {
        ++(depot->amountOfNeighbours);

        depot->neighbours = realloc(depot->neighbours, sizeof(Neighbour*) * 
                depot->amountOfNeighbours);

        depot->neighbours[depot->amountOfNeighbours - 1] = 
                (Neighbour*) malloc(sizeof(Neighbour));

        depot->neighbours[depot->amountOfNeighbours - 1]->name = neighbourName;
        depot->neighbours[depot->amountOfNeighbours - 1]->port = port;

        int newFd = dup(fd);
        depot->neighbours[depot->amountOfNeighbours - 1]->writeFile = 
                fdopen(newFd, "w");

        // Sends IM Message
        fprintf(depot->neighbours[depot->amountOfNeighbours - 1]->writeFile, 
                "IM:%u:%s\n", depot->port, depot->name);
        fflush(depot->neighbours[depot->amountOfNeighbours - 1]->writeFile);
    }
}

/**
    Protocol for checking the string of IM, if the IM message is invalid,
    exit the thread. 
    Params: depot pointer, message to be passed in, file descriptor number
    if the input is valid, it will add the neighbour to the Depot* 
    (see add_neighbour for more)
    Returns 1 if there is an error and 0 if there is not
*/
int im_protocol(Depot* depot, char* message, int fd) {
    int arraySize = 0;
    char** splitString = string_split(message, ':', &arraySize);
    if (arraySize == IM_ARGS_NUM && 
            !contains_banned_char(splitString[NEIGHBOUR_NAME]) && 
            !strcmp(splitString[0], "IM")) {
        for (int i = 0; i < strlen(splitString[PORT_INDEX]); i++) {
            // Exit if port number is invalid
            if (splitString[PORT_INDEX][i] < '0' || 
                    splitString[PORT_INDEX][i] > '9') {
                return 1;
            }
        }        
        add_neighbour(depot, splitString[NEIGHBOUR_NAME], 
                splitString[PORT_INDEX], fd);
        return 0;
    }
    return 1;
}

/**
    Loop for a new connection which is executed when a thread is created
    makes a local version of the most recent file descriptor, prints the
    IM message and then continues to read from the file descriptor until
    it closes/it gets EOF.
    Params: void* containing a pointer to the main depot
    Does not return anything as the loop is inifinite and does not end uwu
*/
void* new_connection_loop(void* args) {
    Param* param = (Param*)args;
    Depot* depot = param->depot;

    FILE* receivedFile = fdopen(param->fd, "r");

    char* receivedMessage;
    while(1) {
        receivedMessage = read_line(receivedFile);
        if (receivedMessage[0] == 0) {
            continue;
        }
        sanitise_inputs_wrapper(depot, receivedMessage);
    }
}

/**
    Connects to the port string passed into it. If the port can be connected
    to, make a new thread on the new connected fd and continue to read
    messages from it. If the port cannot be connected to, do not make a
    new thread and keep reading messages. 
    Also checks if the port to connect
        to already exists before connecting.
    Params: depot pointer to change, message to parse.     
*/
void connect_protocol(Depot* depot, char* message) {
    int connectedFd, arraySize;
    int containsPort = 0;

    pthread_t id;
    
    if (!strncmp(message, "Connect:", strlen("Connect:")) && 
            strlen(message) > strlen("Connect:")) {
        char** splitString = string_split(message, ':', &arraySize);

        for (int i = 0; i < depot->amountOfNeighbours; i++) {
            if (!strcmp(splitString[PORT_NUM], depot->neighbours[i]->port)) {
                containsPort = 1;
                break;
            }
        } 
        // Connect to port passed in and make sure that the split string 
        // is valid
        if (!containsPort && !connect_to_port(splitString[PORT_NUM], 
                &connectedFd) && arraySize == CONNECT_ARGS) {

            int writeFd = dup(connectedFd);

            depot->recentWriteFile = fdopen(writeFd, "w");
            fprintf(depot->recentWriteFile, "IM:%u:%s\n", depot->port, 
                    depot->name);
            fflush(depot->recentWriteFile);

            Param* param = malloc(sizeof(Param));
            param->depot = depot;
            param->fd = connectedFd;

            char* message = read_fd_line(connectedFd);
            if (!im_protocol(depot, message, connectedFd)) {
                pthread_create(&id, NULL, new_connection_loop, param);
            }
        }
    }
}

/**
    Protocol for transferring resources to another depot. 
    Check if the destination is the same as one of the 
    neighbour names, if it is, remove x amount of the type of 
    goods from the depot and deliver that amount of goods to
    the destination port. 
    Params: depot pointer to change, message to split and check
*/
void transfer_protocol(Depot* depot, char* message) {
    int arraySize;
    if (!strncmp(message, "Transfer:", strlen("Transfer:")) && 
            strlen(message) > strlen("Transfer:")) {
	char** splitString = string_split(message, ':', &arraySize);

        // Checks if the amount to transfer is greater than 0 and there are 
        // the right amount of args
        if (atoi(splitString[QUANTITY]) > 0 && 
                arraySize == TRANSFER_ARGS_NUM) {

            for (int i = 0; i < depot->amountOfNeighbours; i++) {
                // Destination is a neighbour
                if (!strcmp(splitString[TRANSFER_DESTINATION], 
                        depot->neighbours[i]->name)) {
                    change_resouce_amount(depot, splitString[TYPE_OF_GOOD], 
                            atoi(splitString[QUANTITY]) * -1);

                    // Deliver to neighbour
                    fprintf(depot->neighbours[i]->writeFile, "Deliver:%s:%s\n",
                            splitString[QUANTITY], 
                            splitString[TYPE_OF_GOOD]);
                    fflush(depot->neighbours[i]->writeFile);
                    break;
                } 
            }
        }
    } 
}

/**
    Sanitises the input using the third letter of the message. The third
    letter was chosen as it is unique in each message making it easy to 
    identify each message so that they can be further checked in their 
    corresponding functions. Each ID was #defined so it is easier to identify.
        Before checking, the string is also checked to see if the max amount 
        of colons is in the message
    Params: Depot pointer, message to check against, file descriptor for IM
        messages so that the port can be connected and sent messages to
*/
void sanitise_inputs(Depot* depot, char* message) {
    if (amount_of_colons(message) <= MAX_COLONS && 
            amount_of_colons(message) > 0) {
        switch (message[THIRD_LETTER]) {
            case DELIVER_ID: 
                deliver_protocol(depot, message);
                break;
            case WITHDRAW_ID:
                withdraw_protocol(depot, message);
                break;
            case DEFER_ID:
                defer_protocol(depot, message); 
                break;
            case EXECUTE_ID:
                execute_orders(depot, message);
                break;
            case CONNECT_ID:
                connect_protocol(depot, message);
                break;
            case TRANSFER_ID:
                transfer_protocol(depot, message);
                break;
        }
    } 
}

/**
    Wraps the sanitise_inputs function within a semaphore, this was done
    as I wanted to access the sanitise inputs function in my execute orders
    function and it would get deadlocked if called again.
    Params: Depot pointer, message to check against, file descriptor for IM
        messages so that the port can be connected and sent messages to
*/
void sanitise_inputs_wrapper(Depot* depot, char* message) {
    sem_wait(&depot->semaphore);
    sanitise_inputs(depot, message);
    sem_post(&depot->semaphore);
}

int main(int argc, char** argv) {
    int fd;
    struct sigaction signalHandler;
    signalHandler.sa_handler = signal_handler;   
    signalHandler.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &signalHandler, 0);
    sigaction(SIGPIPE, &signalHandler, 0);
    if (check_args(argc, argv)) {
        return show_message(check_args(argc, argv));
    }
    init_depot(&depot, argc, argv);
    listen_on_port(&fd, &depot);
    int receivedFd = 1;
    pthread_t id;
    
    while (receivedFd = accept(fd, 0, 0), receivedFd >= 0) {
        Param* param = malloc(sizeof(Param));
        param->depot = &depot;
        param->fd = receivedFd;
        int writeFd = dup(receivedFd);

        char* message = read_fd_line(receivedFd);

        if (!im_protocol(&depot, message, writeFd)) {
            pthread_create(&id, NULL, new_connection_loop, param);
        } 
    }
}
