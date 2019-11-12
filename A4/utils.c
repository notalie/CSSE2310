#include "depot.h"
#include "utils.h"

/**
    Gets the key from a passed in message
    Params: message to get the key from
    Returns: the key found in the message if it is not a float and 0 if it is
*/
int get_key(char* message) {
    int firstColon = 0, counter = 0;
    char* buff = malloc(sizeof(char) * BUFFER_DEFAULT);
    for (int i = 0; i < strlen(message); i++) {
        if (message[i] == ':' && !firstColon) {
            firstColon = 1;
        } else if (message[i] == ':' && firstColon) {
            buff[counter] = message[i];
            break;
        } else if (firstColon) {
            buff[counter] = message[i];
            counter++;
        }
    }
    buff[++counter] = '\0';
    if (atoi(buff) == atof(buff)) {  
        return atoi(buff);
    } else {
        return 0;
    }
}

/**
    Reads the line from a given file descriptor until it reaches a new 
    line and returns the string that was found
    Params: file descriptor to read from
    Returns the read line from the file descriptor
*/
char* read_fd_line(int fd) {
    char* result = malloc(sizeof(char) * BUFFER_DEFAULT);
    int position = 0;
    char buff[1];
    while (1) { 
        int num = read(fd, buff, 1);
        if (buff[0] == '\n' || num == 0) {
            result[position] = '\0';
            return result;
        } else {
            result[position++] = buff[0];
        }
    }
}

/**
*   Reads the lines in a file and returns it
*   Params: the file to read from
*   Returns: The line read from the file if EOF or a new line is found
*/
char* read_line(FILE* fileName) {
    char* result = malloc(sizeof(char) * BUFFER_DEFAULT);
    int position = 0;
    int next = 0;
    while (1) { 
        next = fgetc(fileName);
        if (next == EOF || next == '\n') {
            result[position] = '\0';
            return result;
        } else {
            result[position++] = (char)next;
        }
    }
}

/**
    Gets the command from and deferred order
    Params: The message to get the command from
    Returns: The command contained within the message
*/
char* get_command(char* message) {
    int colonCount = 0, counter = 0;
    char* buff = malloc(sizeof(char) * BUFFER_DEFAULT);
    for (int i = 0; i < strlen(message); i++) {
        if (message[i] == ':' && colonCount < COMMAND_THRESHOLD) {
            colonCount++;
        } else if (colonCount >= COMMAND_THRESHOLD) {
            buff[counter] = message[i];
            counter++;
        }
    }
    buff[++counter] = '\0';
    return buff;

}

/** 
    Checks if the string passed in contains any illegal characters
    Params: String to check if there are any illegal characters in
    Returns 0 if it doesn't and INVALID_NAME (2) if it does
*/
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

/**
    Checks the args passed in and determines if each stringin 
    argsList is valid. If there are goods included, also check 
        if they are valid or not
    Params: the amount of args, the list of the args passed in
*/
int check_args(int argsNum, char** argsList) {
    char* depotName = argsList[DEPOT_NAME_ARGS];
    if (argsNum < MIN_ARGS) {
        return BAD_ARGS;
    } else if (contains_banned_char(depotName)) {
        return INVALID_NAME;
    } else if (argsNum % 2) { // Odd number of goods to quantity
        return BAD_ARGS;
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
    return OK;
}

/**
    Function for comparing resource names for passed in resource structs
    Params: Two void* structs
    Returns the result of comparing the two resource names in 
        lexicographic order
*/
int compare_resource_names(const void* s1, const void* s2) { 
    const Resource* firstResource = *((Resource**)s1);
    const Resource* secondResource = *((Resource**)s2);
    return strcmp(firstResource->name, secondResource->name); 
} 

/**
    Function for comparing neighbour names for passed in neighbour structs
    Params: Two void* structs
    Returns the result of comparing the two neighbour names in 
        lexicographic order
*/
int compare_neighbour_names(const void* s1, const void* s2) { 
    const Neighbour* firstNeighbour = *((Neighbour**)s1);
    const Neighbour* secondNeighbour = *((Neighbour**)s2);
    return strcmp(firstNeighbour->name, secondNeighbour->name); 
} 

/**
    Counts the amount of colons in a given message to check it is valid
    Params: the message to check
    Returns the amount of colons in the string
*/
int amount_of_colons(char* message) {

    int amountOfColons = 0;
    for (int i = 0; i < strlen(message); i++) {
        if (message[i] == ':') {
            amountOfColons++;
        }
    }
    return amountOfColons;
}

/**
    Splits the string by the corresponding split value and returns the 
    string array. It also modifies a value which is the array size.
    Params: The string to split, the value to split by and the array size
        to modify
    Returns: An array of strings with each string containing the value
        split by the passed in splitValue
*/
char** string_split(char* stringToSplit, char splitValue, int* arraySize) {
    int amountOfStrings = 0, currentStringSize = 0;
    char** stringToReturn;
    char* currentString = malloc(sizeof(char));
    stringToReturn = malloc(sizeof(char));
    int stringSize = strlen(stringToSplit);
    for (int i = 0; i < stringSize; i++) {
        if (stringToSplit[i] == splitValue && stringToSplit[i + 1] 
                != splitValue) {
            stringToReturn = realloc(stringToReturn, sizeof(char) * 
                    (amountOfStrings + 1));
            stringToReturn[amountOfStrings] = malloc(sizeof(char) * 
                    currentStringSize);
            currentString[currentStringSize] = '\0';
            stringToReturn[amountOfStrings] = currentString;
            amountOfStrings++;
            currentString = '\0';
            currentStringSize = 0;
        } else {
            currentString = realloc(currentString, sizeof(char) * 
                    currentStringSize + 1);
            currentString[currentStringSize] = stringToSplit[i];
            currentStringSize++;
        }
    }
    if (currentString != 0) {
        stringToReturn = realloc(stringToReturn, sizeof(char) * 
                (amountOfStrings + 1));
        stringToReturn[amountOfStrings] = malloc(sizeof(char) * 
                currentStringSize);
        stringToReturn[amountOfStrings] = currentString;
        amountOfStrings++;
    }
    *arraySize = amountOfStrings;
    return stringToReturn;
}
