#include "hub.h"
pid_t* childrenPIDs; // global pointer for children's PIDs
int childrenAmount; // global variable for the amount of children

/**
    Output error message for status and return status 
*/  
typedef enum {                                          
    OK = 0,                                             
    BAD_ARGS = 1,                                      
    INVALID_THRESHOLD = 2,                                     
    BAD_DECK_FILE = 3,                                    
    SHORT_DECK = 4,                                    
    PLAYER_ERROR = 5,                                      
    PLAYER_EOF = 6,                                      
    INVALID_MESSAGE = 7,
    INVALID_CARD_CHOICE = 8,
    SIGHUP_END = 9                                         
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
    When a SIGHUP is caught, kill all the children
*/
void signal_handler(int signalNum) {
    int status = 0;
    if (signalNum == SIGHUP) {
        for (int i = 0; i < childrenAmount; i++) {
            kill(childrenPIDs[i], 9);
            waitpid(childrenPIDs[i], &status, 1);
        }
        exit(show_message(SIGHUP_END));
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
        if (result == 0 || strlen(result) > 1 || result[0] != '@') {   
            free(result);
            return PLAYER_ERROR;
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
    childrenPIDs = malloc(sizeof(pid_t) * amountOfChildren);
    gameHub->readFiles = malloc(sizeof(FILE*) * amountOfChildren);
    gameHub->writeFiles = malloc(sizeof(FILE*) * amountOfChildren);
    gameHub->leadPlayer = 0;
    gameHub->scoreboard.playerList = malloc(sizeof(Player) * amountOfChildren);
    for (int i = 0; i < amountOfChildren; i++) {
        gameHub->scoreboard.playerList[i].hand = 
                malloc(sizeof(Card) * HANDSIZE);
        gameHub->scoreboard.playerList[i].score = 0;
        gameHub->scoreboard.playerList[i].diamondsWon = 0;
    }
    gameHub->scoreboard.currentPlayer = 0;
    gameHub->playerBoard = malloc(sizeof(Card) * amountOfChildren);
    gameHub->playerBoardOrdered = malloc(sizeof(Card) * amountOfChildren);
    childrenAmount = gameHub->numberOfPlayers;
}

/**
    Spawns the children of the hub and starts them off according to the args
    passed in. The inputs are a hub pointer and an array of strings which will
    determine what is passed into the players
*/
int spawn_children(Hub* gameHub, char** args) {
    pid_t childPID;
    int writePipe[2], readPipe[2];
    init_hub(gameHub);
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        pipe(readPipe);
        pipe(writePipe);
        if (childPID = fork(), childPID == 0) { // Child
            int devNull = open("/dev/null", O_RDONLY); // open the voiddd
            dup2(devNull, STDERR_FILENO);
            close(readPipe[READ_END]); // Child does not read from read pipe
            close(writePipe[WRITE_END]); // Child does not write to write pipe
            dup2(readPipe[WRITE_END], STDOUT_FILENO); // C write to read pipe
            dup2(writePipe[READ_END], STDIN_FILENO); // C reads from write pipe
            get_child_args(gameHub, args, i);
            if (execvp(args[i + 3], gameHub->childArgs) == -1) {
                return PLAYER_ERROR; // If exec fails
            }
        } else { // Parent
            close(readPipe[WRITE_END]); // Parent does not write to read pipe
            close(writePipe[READ_END]); // Parent does not read from write pipe
            gameHub->readFiles[i] = fdopen(readPipe[READ_END], "r");
            gameHub->writeFiles[i] = fdopen(writePipe[WRITE_END], "w");
            gameHub->scoreboard.playerList[i].id = i; //Add ids to scoreboard
            gameHub->scoreboard.childPIDs[i] = childPID; // CPIDs to scoreboard
            childrenPIDs[i] = childPID;
        }
    } 
    if (check_children_init(gameHub)) {
        return PLAYER_ERROR;
    }
    return 0;
}

/** 
    Reaps all the children from the cur
    rent game by for looping through each 
    child's PID
*/
void reap_children(Hub* gameHub) {
    int status;
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        kill(gameHub->scoreboard.childPIDs[i], 9);
        waitpid(gameHub->scoreboard.childPIDs[i], &status, 1);
    }
}

/**
    Checks the args passed in and returns the corresponding 
    error message as defined in the spec. If the args are legal, it returns 0
*/
int check_args(int argsNum, char** args, Hub* gameHub) {
    if (argsNum < MIN_ARGS) {
        return BAD_ARGS;
    } 
    if ((strlen(THRESHOLD_ARG) == 1 && THRESHOLD_ARG[0] < '2') || 
            strlen(THRESHOLD_ARG) == 0) {
        return INVALID_THRESHOLD;
    } 
    // Iterate through each value in the threshold arg
    for (int i = 0; i < strlen(THRESHOLD_ARG); i++) { 
        if (THRESHOLD_ARG[i] < '0' || THRESHOLD_ARG[i] > '9' || 
                THRESHOLD_ARG[i] == 0) {
            return INVALID_THRESHOLD;
        }
    }
    gameHub->numberOfPlayers = argsNum - 3; // Minus first three args
    gameHub->deckFile = fopen(FILE_ARG, "r");
    gameHub->deckName = FILE_ARG;
    if (gameHub->deckFile == NULL) {
        return BAD_DECK_FILE;
    } 
    int status = check_deck_file(gameHub); // Check deck file, check null first
    if (status) {
        if (status != BAD_DECK_FILE) {
            return SHORT_DECK; 
        } else {
            return BAD_DECK_FILE;
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
        return BAD_DECK_FILE;
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
                strlen(readLine) != 2) { // Check first card name
            free(readLine);
            fclose(gameHub->deckFile);
            return BAD_DECK_FILE;
        } else if ((readLine[1] < '1' || readLine[1] > '9') 
                && (readLine[1] < 'a' || readLine[1] > 'f')) {
            free(readLine);
            fclose(gameHub->deckFile);
            return BAD_DECK_FILE;
        }
        free(readLine);
        deckSize++;
    }
    gameHub->deckSize = deckSize;
    if (deckSize != readDeckSize) {
        return BAD_DECK_FILE;
    } else if (readDeckSize < HANDSIZE) {
        return SHORT_DECK;
    }
    return 0;
}

/*
*   Initialise each player's hand when they are sent to each child. 
*   This is to track them to make sure they are not out of bounds.
*/
void init_player_hands(Hub* gameHub, int handSize, char* deck, 
        int currentIndex) {
    int counter = 0;
    int iterator = handSize >= 10 ? DOUBLE_DIGITS : SINGLE_DIGITS;
    for (int i = 0; i < strlen(deck) - iterator; i += 3) {
        gameHub->scoreboard.playerList[currentIndex].hand[counter].suit = 
                deck[i + iterator];
        gameHub->scoreboard.playerList[currentIndex].hand[counter].rank = 
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
        init_player_hands(gameHub, HANDSIZE, playerHand, i);
    }
    fclose(gameHub->deckFile);
}

/**
    Sends the announcement passed in to every child except for the negated 
    child passed in. The hub pointer is passed in to allow for direct writing
    to each child
*/
void broadcast(Hub* gameHub, int negatedChild, char* announcement) {
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        if (i != negatedChild) {
            fprintf(gameHub->writeFiles[i], "%s\n", announcement);
            fflush(gameHub->writeFiles[i]);
        }
    }

}

/**
    Returns the next player in the line. 
    Takes in a hub pointer and bases the next player from the number of 
    players given
*/
int next_player(Hub* gameHub) {
    if (gameHub->scoreboard.currentPlayer == gameHub->numberOfPlayers - 1) {
        return 0;
    } else {
        return ++(gameHub->scoreboard.currentPlayer);
    }
}

/**
    Calculates the scores from the player board. Takes in the gamehub which 
    will then display the scores of each player
*/
void calculate_scores(Hub* gameHub) {
    Player* playerList = gameHub->scoreboard.playerList;
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        if (playerList[i].diamondsWon >= gameHub->threshold) {
            printf("%d:%d", i, playerList[i].score + 
                    playerList[i].diamondsWon);
        } else {
            printf("%d:%d", i, playerList[i].score - 
                    playerList[i].diamondsWon);
        }
        if (i + 1 != gameHub->numberOfPlayers) {
            printf(" ");
        }
    }
    printf("\n");
}

/**
    Determines the winner from the game board that was played
    Takes in a pointer to the hub and sets the winner of the round to the new
    lead player
*/
void determine_winner(Hub* gameHub) {
    Card* board = gameHub->playerBoard;
    char leadSuit = gameHub->leadSuit;
    Card winningCard = board[gameHub->leadPlayer];
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        if (board[i].suit == leadSuit && board[i].rank > winningCard.rank) {
            gameHub->leadPlayer = i;
        } 
    }
    (gameHub->scoreboard.playerList[gameHub->leadPlayer].score)++;
    (gameHub->scoreboard.playerList[gameHub->leadPlayer].diamondsWon) += 
            gameHub->diamondsPlayed;
    gameHub->diamondsPlayed = 0;
}

/**
    Displays the cards played each round
    The player is printed separately for convenience
*/
void display_round_results(Hub* gameHub) {
    printf("Cards=");
    for (int i = 0; i < gameHub->numberOfPlayers; i++) {
        printf("%c.%c", gameHub->playerBoardOrdered[i].suit, 
                gameHub->playerBoardOrdered[i].rank);
        if (i + 1 != gameHub->numberOfPlayers) {
            printf(" ");
        }
    }
    printf("\n");
    fflush(stdout);
}

/**
    Sanitises the inputs passed in by the players, also checks that the player
    actually had the card in their hand and that the card played either was
    because they did not have the lead suit in their hand
*/
int sanitise_inputs(char* string, Hub* gameHub, int counter, int id) {
    if (strlen(string) <= MIN_STRING_SIZE) {
        return PLAYER_EOF;
    } else if (strncmp(string, "PLAY", strlen("PLAY")) || 
            strlen(string) > MAX_STRING_SIZE) {
        return INVALID_MESSAGE;
    }
    Card playedCard;
    playedCard.suit = string[strlen("PLAY")];
    playedCard.rank = string[strlen("PLAYS")];
    if (id == gameHub->leadPlayer) { // Invalid card checks
        gameHub->leadSuit = playedCard.suit;
    } else if (playedCard.suit != 'S' && playedCard.suit != 'C' && 
            playedCard.suit != 'D' && playedCard.suit != 'H') {
        return INVALID_MESSAGE;
    } else if ((playedCard.rank < '1' || playedCard.rank > '9') && 
            (playedCard.rank < 'a' || playedCard.rank > 'f')) {
        return INVALID_MESSAGE;
    }
    if (playedCard.suit == 'D') { // If the card was diamond
        (gameHub->diamondsPlayed)++;
    }
    gameHub->playerBoard[id] = playedCard;
    gameHub->playerBoardOrdered[counter] = playedCard;

    for (int i = 0; i < HANDSIZE; i++) {
        if (gameHub->scoreboard.playerList[id].hand[i].suit == 
                playedCard.suit && 
                gameHub->scoreboard.playerList[id].hand[i].rank == 
                playedCard.rank) {
            return 0;
        }
    }
    if (playedCard.suit != gameHub->leadSuit) {
        for (int i = 0; i < HANDSIZE; i++) {
            if (gameHub->scoreboard.playerList[id].hand[i].suit == 
                    playedCard.suit) {
                return INVALID_CARD_CHOICE;
            }
        }
    }
    return 0;
}

/**
    Game loop that continues until the amount of rounds == the hand size since
    one card is played per round. Takes in a hub pointer to run the game and
    pass around everywhere. During each round, the players are played clockwise
    from the lead player and continue per the amount of players. Once this has
    been done, the winner is evaluated and the round starts a new.
*/
int game_loop(Hub* gameHub) {
    char* line;
    char* broadcastString = malloc(sizeof(char) * MAX_STRING_SIZE);
    gameHub->leadPlayer = 0;
    gameHub->scoreboard.currentRound = 0;
    while (gameHub->scoreboard.currentRound < HANDSIZE) {
        int player;
        sprintf(broadcastString, "NEWROUND%d", gameHub->leadPlayer);
        printf("Lead player=%d\n", gameHub->leadPlayer);
        fflush(stdout);
        broadcast(gameHub, ALL_CHILDREN, broadcastString);
        for (int i = 0; i < gameHub->numberOfPlayers; i++) {
            player = (i + gameHub->leadPlayer) % gameHub->numberOfPlayers;
            line = read_line(gameHub->readFiles[player]);
            if (sanitise_inputs(line, gameHub, i, player)) {
                reap_children(gameHub);
                return sanitise_inputs(line, gameHub, i, player);
            }
            sprintf(broadcastString, "PLAYED%d,%c%c", player, 
                    line[strlen("PLAY")], line[strlen("PLAYS")]);
            broadcast(gameHub, player, broadcastString);
        }
        display_round_results(gameHub);
        determine_winner(gameHub);
        (gameHub->scoreboard.currentRound)++;
    }
    sprintf(broadcastString, "GAMEOVER");
    broadcast(gameHub, ALL_CHILDREN, broadcastString);
    reap_children(gameHub);
    calculate_scores(gameHub);
    return 0;
}

int main(int argc, char** argv) {
    struct sigaction signalHandler;
    signalHandler.sa_handler = signal_handler;   
    signalHandler.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &signalHandler, 0);
    sigaction(SIGPIPE, &signalHandler, 0);
    sigaction(SIGCHILD, &signalHandler, 0);
    Hub gameHub;
    if (check_args(argc, argv, &gameHub)) {
        return show_message(check_args(argc, argv, &gameHub));
    } else if (spawn_children(&gameHub, argv) == PLAYER_ERROR) {
        return show_message(PLAYER_ERROR);
    } 
    // Start dealing to players
    deal_to_players(&gameHub);
    return show_message(game_loop(&gameHub));
}
