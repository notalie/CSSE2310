#include "hub.h"
/**
    Output error message for status and return status 
*/  
typedef enum {                                          
    OK = 0,                                             
    BADARGS = 1,                                      
    INVALIDTHRESHOLD = 2,                                     
    BADDECKFILE = 3,                                    
    SHORTDECK = 4,                                    
    PLAYERERROR = 5,                                      
    PLAYEREOF = 6,                                      
    INVALIDMESSAGE = 7,
    INVALIDCARDCHOICE = 8,
    SIGHUPEND = 9                                         
} Status;                                               
                                                        
/**
    Output error message for status and return status 
*/   
Status show_message(Status s) {                         
    const char* messages[] = {"",                       
            "Usage: 2310hub deck threshold player0 {player1}\n",
            "Invalid threshold\n",       
            "Deck error\n",                        
            "Not enough cards\n",                   
            "Player error\n",                   
            "Player EOF\n",                                 
            "Invalid Message\n",                                 
            "Invalid card choice\n",
            "Ended due to signal\n"
};                              
    fputs(messages[s], stderr);                         
    return s;                                           
}  

/**
    Handles signals received from children, depending on 
    the signal passed in will determine the behaviour of the parent
*/
//TODO: fill this with more sanitising
void signal_handler(int signalNum) {
    if (signalNum == SIGHUP) {
        exit(show_message(SIGHUPEND));
    }
} 

/** 
    Creates the child's args to be passed into the child on execvp  
*/
void get_child_args(Hub* gameHub, char** args, int increment) {
    gameHub->childArgs[0] = args[increment + 3];
    for (int i = 1; i < 5; i++) {
        gameHub->childArgs[i] = malloc(sizeof(int) * INT_MAX);
    }
    sprintf(gameHub->childArgs[1], "%d", gameHub->numberOfPlayers);
    sprintf(gameHub->childArgs[2], "%d", increment);
    sprintf(gameHub->childArgs[3], "%d", gameHub->threshold);
    sprintf(gameHub->childArgs[4], "%d", HANDSIZE);
    gameHub->childArgs[5] = 0;    
}

/**
    Checks the children has all output @, if they have not, return PLAYERERROR
        as an error
*/
int check_children_init(Hub* gameHub) {
    char* result = malloc(sizeof(char) * 4);
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        result = fgets(result, 2, gameHub->readFiles[i]); 
        // Check that result is size 1
        if (strlen(result) > 1 || result[0] != '@') {   
            free(result);
            return PLAYERERROR;
        } 
    } 
    free(result);
    return 0;
}

/**
    Initialises the hub struct pointer passed in, basically mallocs the 
        list of players, the amount of child PIDs, the amount of read and 
        the amount of write files
*/
void init_hub(Hub* gameHub) {
    int amountOfChildren = gameHub->numberOfPlayers;
    gameHub->scoreboard.playerList = malloc(sizeof(Player) * amountOfChildren);
    gameHub->scoreboard.childPIDs = malloc(sizeof(pid_t) * amountOfChildren);
    gameHub->readFiles = malloc(sizeof(FILE*) * amountOfChildren);
    gameHub->writeFiles = malloc(sizeof(FILE*) * amountOfChildren);
    gameHub->leadPlayer = 0;
    gameHub->scoreboard.playerList = malloc(sizeof(Player) * amountOfChildren);
    for (int i = 0; i < HANDSIZE; i++) {
        gameHub->scoreboard.playerList[i].hand = 
                malloc(sizeof(Card) * HANDSIZE);
    }
}

/**
    Spawns the children of the hub and starts them off
    //TODO: Error checking
*/
int spawn_children(Hub* gameHub, char** args) {
    pid_t childPID;
    int writePipe[2], readPipe[2];
    init_hub(gameHub);
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        pipe(readPipe);
        pipe(writePipe);
        if (childPID = fork(), childPID == 0) { // Child
            close(readPipe[READ_END]); // Child does not read from read pipe
            close(writePipe[WRITE_END]); // Child does not write to write pipe
            dup2(readPipe[WRITE_END], STDOUT_FILENO); // C write to read pipe
            dup2(writePipe[READ_END], STDIN_FILENO); // C reads from write pipe
            get_child_args(gameHub, args, i);
            execvp(args[i + 3], gameHub->childArgs);
            return show_message(PLAYERERROR); // If exec fails
        } else { // Parent
            if (check_dead_children(gameHub)) {
                return PLAYERERROR;
            }
            close(readPipe[WRITE_END]); // Parent does not write to read pipe
            close(writePipe[READ_END]); // Parent does not read from write pipe
            gameHub->readFiles[i] = fdopen(readPipe[READ_END], "r");
            gameHub->writeFiles[i] = fdopen(writePipe[WRITE_END], "w");
            gameHub->scoreboard.playerList[i].id = i; //Add ids to scoreboard
            gameHub->scoreboard.childPIDs[i] = childPID; // CPIDs to scoreboard
        }
    } // Check if child died before checking they are initialised
    if (check_dead_children(gameHub)) {
        return PLAYERERROR;
    } else if (check_children_init(gameHub)) {
        return PLAYERERROR;
    }
    return 0;
}

/** 
    Reaps all the children from the cur
    rent game by for looping through each 
    child PID
*/
void reap_children(Hub* gameHub) {
    int status;
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        kill(gameHub->scoreboard.childPIDs[i], 9);
        waitpid(gameHub->scoreboard.childPIDs[i], &status, 1);
    }
}

/**
    Checks all the children in the game, if any of them have exited, 
        return SIGHUPEND as handled and return exit status 9
*/
//TODO: finish this when you need to, no rush though
int check_dead_children(Hub* gameHub) {
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        int status, returnedValue;
        returnedValue = waitpid(gameHub->scoreboard.childPIDs[i], 
                &status, WNOHANG);
        if (WIFSIGNALED(status) && returnedValue > 0) {
            reap_children(gameHub);
            return SIGHUP;
        } else if (WEXITSTATUS(status) > 0 && WEXITSTATUS(status) < 10 
                && returnedValue > -1) {
            reap_children(gameHub);
            return PLAYERERROR;
        }
    }
    return 0;
}

/**
    Checks the args passed in and returns the corresponding 
    error message as defined in the spec. If the args are legal, it returns 0
*/
int check_args(int argsNum, char** args, Hub* gameHub) {
    if (argsNum < MIN_ARGS) {
        return BADARGS;
    } 
    if ((strlen(THRESHOLD_ARG) == 1 && THRESHOLD_ARG[0] < '2') || 
            strlen(THRESHOLD_ARG) == 0) {
        return INVALIDTHRESHOLD;
    } 
    // Iterate through each value in the threshold arg
    for (int i = 0; i < strlen(THRESHOLD_ARG); i++) { 
        if (THRESHOLD_ARG[i] < '0' || THRESHOLD_ARG[i] > '9' || 
                THRESHOLD_ARG[i] == 0) {
            return INVALIDTHRESHOLD;
        }
    }
    gameHub->numberOfPlayers = argsNum - 3; // Minus first three args
    gameHub->deckFile = fopen(FILE_ARG, "r");
    gameHub->deckName = FILE_ARG;
    if (gameHub->deckFile == NULL) {
        return BADDECKFILE;
    } 
    int status = check_deck_file(gameHub); // Check deck file, check null first
    if (status) {
        if (status != BADDECKFILE) {
            return SHORTDECK; 
        } else {
            return BADDECKFILE;
        }
    } 
    gameHub->threshold = atoi(THRESHOLD_ARG);
    return 0;
}


/**
*   Reads the lines in a file and returns it, taken from our lord saviour Joel
*   Returns: The line read from the file if EOF or a new line is found
*/
char* read_line(FILE* fileName) {
    char* result = malloc(sizeof(char) * 80);
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
    Checks the deck file and seed if it is valid. 
    If it is not, return with exit status 3 (BADDECKFILE)
*/
int check_deck_file(Hub* gameHub) {
    int deckSize = 0;
    char* readLine = read_line(gameHub->deckFile);
    int readDeckSize = atoi(readLine);
    if (readDeckSize > 60) {
        fclose(gameHub->deckFile);
        return BADDECKFILE;
    } else if (readDeckSize < HANDSIZE) {
        fclose(gameHub->deckFile);
        return SHORTDECK;
    }
    while (1) { // Read until EOF
        readLine = read_line(gameHub->deckFile);
        if (!(strcmp(readLine, ""))) {
            free(readLine);
            fclose(gameHub->deckFile);
            break;
        }
        if ((readLine[0] != 'S' && readLine[0] != 'C' && 
                readLine[0] != 'D' && readLine[0] != 'H') || 
                strlen(readLine) != 2) {
            free(readLine);
            fclose(gameHub->deckFile);
            return BADDECKFILE;
        } else if ((readLine[1] < '1' || readLine[1] > '9') 
                && (readLine[1] < 'a' || readLine[1] > 'f')) {
            free(readLine);
            fclose(gameHub->deckFile);
            return BADDECKFILE;
        }
        free(readLine);
        deckSize++;
    }
    if (deckSize != readDeckSize) {
        return BADDECKFILE;
    }
    gameHub->deckSize = deckSize;
    return 0;
}

/*
*   Initialise each player's hand when they are sent to each child. 
*   This is to track them to make sure they are not out of bounds.
*/
void init_player_hands(Scoreboard* scoreboard, int handSize, char* deck, 
        int currentIndex) {
    int counter = 0;
    int iterator = handSize >= 10 ? DOUBLEDIGITS : SINGLEDIGITS;
    for (int i = 0; i < strlen(deck) - iterator; i += 3) {
        scoreboard->playerList[currentIndex].hand[counter].suit = 
                deck[i + iterator];
        scoreboard->playerList[currentIndex].hand[counter].rank = 
                deck[i + iterator + 1]; // Add to hand
        counter++;
    }
}

/**
*   Draws a card from the corresponding deck and shuffles it down.
*   Once the shuffling is complete, return the top piece
*   Parameters: The deck to draw from
*   Returns: The piece at the top of the deck (array at 0)
*/
char* draw_player_hand(Hub* gameHub) {
    char* playerHand = malloc(sizeof(char) * (gameHub->deckSize * 2) + 6);
    sprintf(playerHand, "HAND%d,", HANDSIZE);
    for (int i = 0; i < HANDSIZE; i++) {
        strcat(playerHand, read_line(gameHub->deckFile));
        if (i < HANDSIZE - 1) {
            strcat(playerHand, ",");
        }
    }
    return playerHand;
}

/**
    Deals each player their corresponding cards. 
    To do this I reopen the FILE* as it was closed in the deck file check,
    I then iterate through each player and send them their cards
*/
void deal_to_players(Hub* gameHub) {
    char* playerHand;
    gameHub->deckFile = fopen(gameHub->deckName, "r");
    free(read_line(gameHub->deckFile));
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        playerHand = draw_player_hand(gameHub);
        fprintf(gameHub->writeFiles[i], "%s\n", playerHand); 
        fflush(gameHub->writeFiles[i]); //Sends hand down pipe
        init_player_hands(&gameHub->scoreboard, HANDSIZE, playerHand, i);
        if (check_dead_children(gameHub)) {
            exit(show_message(PLAYERERROR));
        } 
    }
    fclose(gameHub->deckFile);
}

/**
    Checks if the game is over by seeing if the current round is bigger 
    than the hand size meaning that it will end before broadcasting a new 
    round to all the children
*/
int is_game_over(Hub* gameHub) {
    if (gameHub->scoreboard.currentRound > HANDSIZE) {
        return 1;
    }
    return 0;
}

/**
//TODO
*/
void braodcast(Hub* gameHub) {

}

//TODO: 
int game_loop(Hub* gameHub) {
    char* line;
    while (1) {
        if (is_game_over(gameHub)) {
            break;
        } else {
            fprintf(stderr, "Lead Player=%d\n", gameHub->leadPlayer);

        }
        (gameHub->scoreboard.currentRound)++;
    }
    reap_children(gameHub);
    return 0;
}

int main(int argc, char** argv) {
    struct sigaction signalHandler;
    signalHandler.sa_handler = signal_handler;   
    signalHandler.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &signalHandler, 0);
    //| SIGPIPE | SIGCHILD
    //
    Hub gameHub;
    if (check_args(argc, argv, &gameHub)) {
        return show_message(check_args(argc, argv, &gameHub));
    } else if (spawn_children(&gameHub, argv) == PLAYERERROR) {
        return show_message(PLAYERERROR);
    } 
    // Start dealing to players
    deal_to_players(&gameHub);
    if (check_dead_children(&gameHub)) {
        return show_message(PLAYERERROR);
    } 
    return game_loop(&gameHub);
}
