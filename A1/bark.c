#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
/**
*   Defined a type of struct called piece, each piece contains a letter and 
*   number as stated by the Assignment spec so I thought this would be the 
*   most efficient way to implement this
*/
typedef struct {
    char letter;
    int number;
} Piece;

/**
*   Defined type Screen, Screen contains a column and row size to allow 
*   for easier printing of the board. Also contained in the screen type
*   is a board which is a 2D array of pieces allowing for easier printing
*/
typedef struct {
    int rowSize;
    int columnSize;
    Piece** board;
} Screen;

/**
*   Struct for a player so that I can keep track on whether or not a player
*   is automated or not
*/
typedef struct {
    Piece* playerHand;
    char type;
    int currentPlayer;
    int handSize;
} Player;

/**
*   Struct for the deck so that I can keep track on the deck size for realloc
*/
typedef struct {
    Piece* deckPile;
    int deckSize;
} Deck;

/** 
*   Initilises the screen usitilising dynamic memory allocation 
*   Parameter: Screen pointer
*/
void initialise_screen(Screen* screen) {
    screen->board = malloc(sizeof(Piece) * screen->columnSize);
    for (int i = 0; i < screen->rowSize; i++) {
        screen->board[i] = malloc(sizeof(Piece) * screen->rowSize);
    } 
    for (int i = 0; i < screen->rowSize; i++) {
        for (int j = 0; j < screen->columnSize; j++) {
            screen->board[i][j].number = 0;
            screen->board[i][j].letter = 0;
        }
    }
} 

/** 
*   Prints the screen utilising the screen's row and column size
*   Parameters: Screen pointer 
*/
void print_screen(Screen* screen) {
    Piece** board = screen->board;
    for (int i = 0; i < screen->rowSize; i++) {
        for (int j = 0; j < screen->columnSize; j++) {
            if (board[i][j].number != 0 && board[i][j].letter != 0) {
                printf("%d%c", board[i][j].number, board[i][j].letter);
            } else {
                printf("..");
            }
            if (j == screen->columnSize - 1) {
                printf("\n");
            } 
        }
    }
}

/**
*   Frees the heap of memory being played on. 
*   This will only happen when the game ends
*   Parameters: The screen being played on
*/
void free_board(Screen* screen) {
    for (int i = 0; i < screen->rowSize; i++) {
        free(screen->board[i]);   
    } 
    free(screen->board);
}

/**
*   Adds a piece to the board if it is valid, if it is not valid return 0
*   Parameters: The screen being played on, the piece to be added,
*   and the x/ y coordinates for the piece to be placed on the board
*   Returns: 1 if adding the piece was successful, 0 if not
* */
int add_piece(Screen* screen, Piece piece, int x, int y) {
    // User inputs will begin at 1 so decrement by 1
    x--;
    y--;
    //Piece is in a place that already has a piece
    if (screen->board[y][x].number != 0 && screen->board[y][x].number != 0) {
        return 0;
    } else {
        // Adds piece to the board
        screen->board[y][x] = piece;
        return 1;
    }
}

/**
*   Sets player types to be either a for automated or h for user input (stdin)
*/
void initialise_player_types(Player* player1, Player* player2, 
        int amountOfInputs, char** inputs) {
    if (amountOfInputs == 6) {
        player1->type = inputs[4][0];
        player2->type = inputs[5][0];
    } else if (amountOfInputs == 4) {
        player1->type = inputs[2][0];
        player2->type = inputs[3][0];
    }
}

/**
*   Checks the arg types entered, if there is an incorrect number of numbers,
*   Parameters: The amount of inputs entered into the program, a string array
*   containing the inputs entered and pointer to players so that they can
*   be initialised if all the values are valid
*   exit with status 1, if the formatting is incorrect, exit with status 2
*/
void check_arg_types(int amountOfInputs, char** inputs, Player* player1, 
        Player* player2) {
    if (amountOfInputs != 6 && amountOfInputs != 4) {
        fprintf(stderr, "Usage: bark savefile p1type p2type\n"
                "bark deck width height p1type p2type\n");
        exit(1);
    } else if (amountOfInputs == 6 && (atoi(inputs[2]) < 3 || 
            atoi(inputs[2]) > 100 || atoi(inputs[3]) < 3 || 
            atoi(inputs[3]) > 100)) {
        fprintf(stderr, "Incorrect arg types\n");
        exit(2);
    } else if (amountOfInputs == 6 && ((strcmp(inputs[4], "a") && 
            strcmp(inputs[4], "h")) || (strcmp(inputs[5], "a") && 
            strcmp(inputs[5], "h")))) {
        fprintf(stderr, "Incorrect arg types\n");
        exit(2);
    } else if (amountOfInputs == 4 && ((strcmp(inputs[2], "a") 
            && strcmp(inputs[2], "h")) || (strcmp(inputs[3], "a") && 
            strcmp(inputs[3], "h")))) { 
        fprintf(stderr, "Incorrect arg types\n");
        exit(2);
    } else {
        // If all inputs are correct, initialise players
        initialise_player_types(player1, player2, amountOfInputs, inputs);
    }
}

/**
*   Reads the lines in a file and returns it, taken from our lord saviour Joel
*   Returns: The line read from the file
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
*   Prints the deck being played in the game, helper function
*   Parameters: Takes in a deck pointer and prints it
*/
void print_deck(Deck* deck) {
    printf("Deck: ");
    for (int i = 0; i < deck->deckSize; i++) {
        printf("%d%c ", deck->deckPile[i].number, deck->deckPile[i].letter);
    }
    printf("\n");
}
 
/**
*   Checks the deck file for a valid input and changes the actual deck size 
*   from a counter
*   Parameters: The file name, the pointer to the deck file, the actualDeckSize
*   Returns: The read deck size 
*/
int check_deck_file(char* fileName, FILE** deckFile, int* actualDeckSize) {
    if (*deckFile == NULL) {
        *deckFile = fopen(fileName, "r");
    }
    if (*deckFile == NULL) {
        fprintf(stderr, "Unable to parse deckfile\n");
        exit(3);
    }
    // Read first line 
    char* readLine = read_line(*deckFile);
    int readDeckSize = atoi(readLine);
    free(readLine);
    while (1) { 
        readLine = read_line(*deckFile);
        (*actualDeckSize)++;
        if (!(strcmp(readLine, ""))) {
            free(readLine);
            break;
            fclose(*deckFile);
        }
        free(readLine);
    }
    return readDeckSize;
}

/* 
*   Initialises the deck file being read and checks if it is valid
*   Parameters: The file name, the pointer to the deck file 
*   and the deck to intiialise
*   Exits in error
*/
void initialise_deck_file(char* fileName, FILE** deckFile, Deck* deck) {
    int actualDeckSize = -1;
    int readDeckSize = check_deck_file(fileName, deckFile, &actualDeckSize);
    if (actualDeckSize != readDeckSize) {
        fprintf(stderr, "Unable to parse deckfile\n");
        exit(3);
    } else if (actualDeckSize <= 0) {
        fprintf(stderr, "Unable to parse deckfile\n");
        exit(3);
    } else if (actualDeckSize < 11) {
        fprintf(stderr, "Short Deck\n");
        exit(5);
    } else {
        // If amount of cards in deck are correct, allocate memory for deck and
        // put each card in the deck
        char* readLine;
        *deckFile = fopen(fileName, "r");
        free(read_line(*deckFile));  
        deck->deckSize = actualDeckSize;
        deck->deckPile = malloc(sizeof(Piece) * actualDeckSize);
        for (int i = 0; i < actualDeckSize; i++) {
            readLine = read_line(*deckFile); 
            if (readLine[1] > 90 || readLine[1] < 65 || readLine[0] < 49 || 
                    readLine[0] > 57 || (readLine[2] != 32 
                    && readLine[2] != 0)) {
                free(deck->deckPile);
                fprintf(stderr, "%s", "Unable to parse deckfile\n");
                fclose(*deckFile);
                free(readLine);
                exit(3);
            } 
            deck->deckPile[i].number = atoi(&readLine[0]);
            deck->deckPile[i].letter = readLine[1];
            free(readLine);
        }
        fclose(*deckFile);
    }
}

/**
*   Splits the string by the corresponding split value and returns the 
*   string array. It also modifies a value which is the array size
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

/**
*   Helper function for my helper function that frees the memory
*   of a passed in allocated string array
*/
void free_string(char** stringToFree, int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        free(stringToFree[i]);
    }
    free(stringToFree);
} 

/**
*   Helper function for check save file, checks if the first line is valid
*   Exits: 4 if the save file is not valid
*/
char** check_first_save_line(char* lineToCheck, FILE** gameFile) {
    int arraySize = 0; 
    int lineSize = strlen(lineToCheck);
    //Check for things that aren't null, space or numbers
    for (int i = 0; i < lineSize; i++) {
        if (lineToCheck[i] > 58 || lineToCheck[i] < 48) {
            if ((lineToCheck[i] == 32 && lineToCheck[i + 1] == 32) || 
                    lineToCheck[i] != 32) {
                fprintf(stderr, "Unable to parse savefile\n");
                exit(4);
            }
        } 
    }
    char** returnedString = string_split(lineToCheck, ' ', &arraySize);
    if (atoi(returnedString[0]) > 100 || atoi(returnedString[0]) < 3 ||
            atoi(returnedString[1]) > 100 || atoi(returnedString[1]) < 3 ||
            arraySize != 4) {
        free_string(returnedString, arraySize);
        fprintf(stderr, "Unable to parse savefile\n");
        exit(4);
    } else if (atoi(returnedString[3]) != 2 && atoi(returnedString[3]) != 1) {
        free_string(returnedString, arraySize);
        fprintf(stderr, "Unable to parse savefile\n");
        exit(4);
    }
    return returnedString;
}

/**
*   Checks individual lines of player hands, helper function 
*/
void intialise_player_hand(Player* player, int playerLineLength, char* line) {
    int counter = 0;
    for (int i = 0; i < playerLineLength * 2; i++) {
        if (i % 2 == 0) {
            if (line[i] < 49 || line[i] > 57) {
                fprintf(stderr, "Unable to parse savefile\n");
                exit(4);
            }
        } else {
            if (line[i] > 90 || line[i] < 65) {
                free(player->playerHand);
                fprintf(stderr, "Unable to parse savefile\n");
                exit(4);
            } else {
                player->playerHand[counter].number = atoi(&line[i - 1]);
                player->playerHand[counter].letter = line[i];
                counter++;
            }
        } 
    }
}

/**
*   Draws a card from the corresponding deck and shuffles it down.
*   Once the shuffling is complete, return the top piece
*/
Piece draw_from_deck(Deck* deck) {
    Piece topPiece = deck->deckPile[0];
    for (int i = 0; i < deck->deckSize; i++) {
        deck->deckPile[i] = deck->deckPile[i + 1];
    }
    (deck->deckSize)--;
    deck->deckPile = realloc(deck->deckPile, sizeof(Piece) * deck->deckSize);
    return topPiece;
}

/**
*   Deals a card to the corresponding player
*/
void deal_to_player(Player* player, Deck* deck) {
    player->playerHand[player->handSize] = draw_from_deck(deck);
    (player->handSize)++;
}

/**
*   Sets player 1 and player 2 hands to be set by the lines given
*/
void intiailise_player_hands(Player* player1, Player* player2, 
        char* player2Hand, char* player1Hand) {
    int player1LineLength = strlen(player1Hand) / 2;
    int player2LineLength = strlen(player2Hand) / 2;
    if (player1->currentPlayer && player1LineLength != 6) {
        fprintf(stderr, "Unable to parse savefile\n");
        exit(4);
    } else if (player2->currentPlayer && player2LineLength != 6) {
        fprintf(stderr, "Unable to parse savefile\n");
        exit(4);
    } else if (player2LineLength == player1LineLength) {
        fprintf(stderr, "Unable to parse savefile\n");
        exit(4);
    }
    player1->handSize = player1LineLength;
    player2->handSize = player2LineLength;
    player1->playerHand = malloc(sizeof(Piece) * 6);
    player2->playerHand = malloc(sizeof(Piece) * 6);
    intialise_player_hand(player1, player1LineLength, player1Hand);
    intialise_player_hand(player2, player2LineLength, player2Hand);
}

//<==============LOAD GAME==========

/**
*   Sets the screen to the read save file
*/
int initialise_read_screen(Screen* screen, char* line, int row, int* pieces) {
    int isBoardFull = 1;
    for (int i = 0; i < screen->columnSize * 2; i++) {
        if (i % 2 == 0) {
            if (line[i] != 42 && (line[i] < 49 || line[i] > 57)) {
                fprintf(stderr, "Unable to parse savefile\n");
                exit(4);
            } 
        } else {
            if (line[i] != 42 && (line[i] > 90 || line[i] < 65)) {
                fprintf(stderr, "Unable to parse savefile\n");
                exit(4);
            } else if (line[i] != 42) {
                screen->board[row][i / 2].number = atoi(&line[i - 1]);
                screen->board[row][i / 2].letter = line[i];
            } else if ((line[i] == 42 && line[i - 1] != 42) ||
                    (line[i - 1] == 42 && line[i] != 42)) {
                fprintf(stderr, "Unable to parse savefile\n");
                exit(4);
            } else if (line[i] == 42) {
                isBoardFull = 0;
                (*pieces)++;
            }
        } 
    } 
    return isBoardFull;
}

/**
*   Checks the board section of a save file to make sure it is not over the 
*   specificed length and is not full.
*/
int check_saved_screen(Screen* screen, FILE** gameFile) {
    int boardIsFull = 0;
    int pieces = 0;
    for (int i = 0; i < screen->rowSize; i++) {
        if(initialise_read_screen(screen, read_line(*gameFile), i, &pieces)) {
            boardIsFull++;
        }
    }
    if (read_line(*gameFile)[0] != 0) {
        fclose(*gameFile);
        fprintf(stderr, "Unable to parse savefile\n");
        exit(4);
    }
    if (boardIsFull == screen->rowSize) {
        fprintf(stderr, "Board full\n");
        exit(6);
    }
    return (screen->rowSize * screen->columnSize) - pieces;
}

/**
*   Initialises the save file, if it is invalid, exit with code 4, 
*   if it is not, start parsing it, set active players
*/
void initialise_save_file(char** deckLocation, char* fileName, FILE** gameFile,
        FILE** deckFile, Player* player1, Player* player2, 
        Screen* screen, Deck* deck) {
    char* readLine;
    *gameFile = fopen(fileName, "r");
    if (*gameFile == NULL) {
        fprintf(stderr, "%s", "Unable to parse savefile\n");
        exit(4);
    } 
    readLine = read_line(*gameFile);
    if (strlen(readLine) < 4) {
        fclose(*gameFile);
        fprintf(stderr, "Unable to parse savefile\n");
        exit(4);
    }
    char** firstLine = check_first_save_line(readLine, gameFile);
    screen->columnSize = atoi(firstLine[0]);
    screen->rowSize = atoi(firstLine[1]);
    initialise_screen(screen);
    if (atoi(firstLine[3]) == 1) {
        player1->currentPlayer = 1;
        player2->currentPlayer = 0;
    } else {
        player1->currentPlayer = 0;
        player2->currentPlayer = 1;
    } 
    readLine = read_line(*gameFile);
    *deckLocation = readLine;
    initialise_deck_file(readLine, deckFile, deck);
    intiailise_player_hands(player1, player2, read_line(*gameFile), 
            read_line(*gameFile));
    int placedPieces = check_saved_screen(screen, gameFile);
    if((placedPieces + 11) != atoi(firstLine[2]) || atoi(firstLine[2]) < 11) {
        fprintf(stderr, "Unable to parse savefile\n");
        fclose(*gameFile);
        exit(4);
    } 
    for(int i = 0; i < atoi(firstLine[2]); i++) {
        draw_from_deck(deck);
    }
    free(readLine);
}

//<==============PLAYER AND MAIN GAME==========

/** 
*   //TODO
*/
void switch_player(Player* player1, Player* player2, int* currentPlayer) {
    if (player1->currentPlayer) {
        *currentPlayer = 2;
        player1->currentPlayer ^= 1;
        player2->currentPlayer ^= 1;
    } else {
        *currentPlayer = 1;
        player1->currentPlayer ^= 1;
        player2->currentPlayer ^= 1;
    }
}

/** 
*   //TODO
*/
Player* get_current_player(Player* player1, Player* player2, 
        int currentPlayer) {
    if (currentPlayer == 1) {
        return player1;
    } else {
        return player2;
    }
}

/** 
*   //TODO
*/
void remove_player_piece(Player* player, int index) {
    for (int i = 0; i < player->handSize; i++) {
        player->playerHand[index + i] = player->playerHand[index + i + 1];
    }
    (player->handSize)--;
}

/** 
*   //TODO: might change on spec
*/
int process_user_input(char** returnedString, Screen* screen, Player* player, 
        int cardsPlayed) {
    //Check every input is only a number with no space
    for (int i = 0; i < 3; i++) {
        int stringLength = strlen(returnedString[i]);
        for (int j = 0; j < stringLength; j++) {
            if (returnedString[i][j] == 32) {
                return 0;
            }
        }        
    }
    if (returnedString[0][0] > 54 || returnedString[0][0] < 49) {
        return 0;
    } else if (atoi(returnedString[1]) > screen->columnSize || 
            atoi(returnedString[1]) < 1) {
        return 0;
    } else if (atoi(returnedString[2]) > screen->rowSize || 
            atoi(returnedString[2]) < 1) {
        return 0;
    } 
    int column = atoi(returnedString[1]) - 1;
    int row = atoi(returnedString[2]) - 1;
    Piece** board = screen->board;
    //TODO: shorten this
    if (cardsPlayed > 0) {
        if (row == screen->rowSize - 1 && board[0][column].number != 0) {
        } else if (column == screen->columnSize - 1 && 
                board[row][0].number != 0) {
        } else if (row == 0 && 
                board[screen->rowSize - 1][column].number != 0) {
        } else if (column == 0 && 
                board[row][screen->columnSize - 1].number != 0) {
        } else if (row < screen->rowSize - 1 &&
                board[row + 1][column].number != 0) {   //Below
        } else if (column < screen->columnSize - 1 && 
                board[row][column + 1].number != 0) {    //Right
        } else if (column > 0 && 
                board[row][column - 1].number != 0) {    //Left
        } else if (row > 0 && 
                board[row - 1][column].number != 0) {    //Above
        } else {
            return 0;
        }
    }
    if (add_piece(screen, player->playerHand[atoi(returnedString[0]) - 1], 
            atoi(returnedString[1]), atoi(returnedString[2])) == 1) {
        remove_player_piece(player, atoi(returnedString[0]) - 1);
        return 1;
    } else {
        return 0;
    } 
}

/** 
*   //TODO: might change on spec
*/
int is_save_file_line(char* line) {
    if (line[0] != 'S' || line[1] != 'A' || line[2] != 'V' || line[3] != 'E') {
        return 0;
    } else {
        int saveLineLength = strlen(line);
        if (saveLineLength < 10) {
            printf("Unable to save\n");
            return 0;
        }
    }
    return 1;
}

/** 
*   //TODO
*/
int process_manual_move(char** saveFileName, Screen* screen, Player* player,
        Deck* deck, int cardsPlayed) {
    char* userInput = malloc(sizeof(char) * 80);
    while (1) {
        printf("Move? ");
        fflush(stdout);
        userInput = fgets(userInput, 80, stdin);
        if (userInput == NULL) {
            fprintf(stderr, "End of input\n"); 
            exit(7);
        } else {
            int inputSize = 0;
            char** returnedString = string_split(userInput, ' ', &inputSize);
            if (is_save_file_line(userInput) == 1) {
                int userInputSize = strlen(userInput);
                *saveFileName = malloc(sizeof(char) * userInputSize);
                *saveFileName = userInput;
                return 1;
            }
            if (inputSize != 3) {
                //Repeat if invalid move
                continue;
            } else {
                int successfulMove = 1;
                //Index for user, check valid inputs
                successfulMove = process_user_input(returnedString, screen, 
                        player, cardsPlayed);
                if (successfulMove) {
                    break;
                } 
            }
        }
    }
    return 0;
}

/**
*   Returns 1 if the game is over, returns 0 is it isn't
*/
int is_game_over(Screen* screen, Deck* deck) {
    Piece** board = screen->board;
    if (deck->deckSize == 0) {
        return 1;
    } else {
        for (int i = 0; i < screen->rowSize; i++) {
            for (int j = 0; j < screen->columnSize; j++) {
                if (board[i][j].number == 0 && board[i][j].letter == 0) {
                    return 0;
                } 
            }
        }
        return 1;
    }
}

/**
*   Prints the player's hand, if the moving player is 3, print the hand of the
*   automated player removing the () part
*/
void print_player_hand(Player* player, int movingPlayer) {
    if (movingPlayer == 3) {
        printf("Hand: ");
    } else {
        printf("Hand(%d): ", movingPlayer);
    }
    for (int i = 0; i < player->handSize; i++) {
        printf("%d%c", player->playerHand[i].number, 
                player->playerHand[i].letter);
        if (i + 1 == player->handSize) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
}

/**
*   Saves the file according the specifications set by the spec
*/
void save_file(char* deckLocation, Screen* screen, Deck* deck, Player* player1,
        Player* player2, int cardsPlayed, int movingPlayer, 
        char* fileName) {
    char* another = malloc(sizeof(char) * (strlen(fileName) - 5));
    for (int i = 0; i < strlen(fileName) - 5; i++) {
        another[i] = fileName[4 + i];
    }
    if(player2->handSize == 6 && movingPlayer != 2) {
        player2->handSize = 5;
    } else {
        player1->handSize = 5;
    }
    another[strlen(another)] = '\0';
    FILE* saveFile = fopen(another, "w");
    fprintf(saveFile, "%d %d %d %d\n", screen->columnSize, screen->rowSize,
            cardsPlayed + 11, movingPlayer);
    fprintf(saveFile, "%s\n", deckLocation);
    for (int i = 0; i < player1->handSize; i++) {
        fprintf(saveFile, "%d%c", player1->playerHand[i].number, 
                player1->playerHand[i].letter);
    }
    fprintf(saveFile, "\n");
    for (int i = 0; i < player2->handSize; i++) {
        fprintf(saveFile, "%d%c", player2->playerHand[i].number, 
                player2->playerHand[i].letter);
    }
    fprintf(saveFile, "\n");
    Piece** board = screen->board;
    for (int i = 0; i < screen->rowSize; i++) {
        for (int j = 0; j < screen->columnSize; j++) {
            if (board[i][j].number != 0 && board[i][j].letter != 0) {
                fprintf(saveFile, "%d%c", board[i][j].number, 
                        board[i][j].letter);
            } else {
                fprintf(saveFile, "**");
            }
            if (j == screen->columnSize - 1) {
                fprintf(saveFile, "\n");
            } 
        }
    }
    fclose(saveFile);
}


/** 
*   Checks if the board is full by returning 0 when a valid piece is found
*/
int is_board_full(Screen* screen) {
    Piece** board = screen->board;
    for (int i = 0; i < screen->rowSize; i++) {
        for (int j = 0; j < screen->columnSize; j++) {
            if (board[i][j].number != 0) {
                return 0;
            }
        }
    }
    return 1;
}

/** 
*   //TODO
*/
int is_empty_position(Piece** board, int* column, int* row) {
    if (board[*row][*column].number != 0) {
        return 0;
    } else {
        return 1;
    }

} 

/** 
*   //TODO
*/
void automated_move_player_2(Screen* screen, int* column, int* row) {
    Piece** board = screen->board;
    while (1) {
        if (is_empty_position(board, column, row)) {
            if (board[*row - 1][*column].number != 0) {
                break;
            } else if (board[*row][*column - 1].number != 0) {
                break;
            } else if (board[*row][*column + 1].number != 0) {
                break;
            } else if (*row > screen->rowSize - 1 && 
                    board[*row + 1][*column].number != 0) {
                break;
            } else if (*column - 1 == 0 && 
                    board[*row][*column].number != 0) {
                break;
            } else if (*column + 1 == screen->columnSize && 
                    board[*row][0].number != 0) {
                break;
            } else if (*row == screen->rowSize - 1 && 
                    board[0][*column].number != 0) {
                break;
            } else if (*column == screen->columnSize - 1 && 
                    board[0][*column].number != 0) {
                break;
            }
        }
        if (*column - 1 == -1) {
            *(column) = screen->columnSize - 1;
            (*row)--;
        } else {
            (*column)--;
        }
    }
}

/** 
*   Automated player move for player 1
*   start from the top and moves right then down
*/
void automated_move_player_1(Screen* screen, int* column, int* row) {
    Piece** board = screen->board;
    while (1) {
        if (is_empty_position(board, column, row)) {
            if (board[*row + 1][*column].number != 0) {
                break;
            } else if (board[*row][*column + 1].number != 0) {
                break;
                //Above
            } else if (*row > 0 && board[*row - 1][*column].number != 0) {
                break;
            } else if (board[*row][*column - 1].number != 0) {
                break;
            } else if (*column == screen->columnSize - 1 && 
                    board[*row][0].number != 0) {
                break;
            } else if (*row == 0 && 
                    board[screen->rowSize - 1][*column].number != 0) {
                break;
            } else if (*column == 0 && 
                    board[*row][screen->columnSize - 1].number != 0) {
                break;
            }
        }
        if (*column + 1 == screen->columnSize) {
            *column = 0;
            (*row)++;
        } else {
            (*column)++;
        }
    }
}

/** 
*   //TODO
*/
void automated_move(Screen* screen, Player* player, int movingPlayer) {
    int column = 0;
    int row = 0;
    if(is_board_full(screen)) {
        column = screen->columnSize / 2;
        row = screen->rowSize / 2;
        if (screen->columnSize % 2 != 0) {
            column++;
        } 
        if (screen->rowSize % 2 != 0) {
            row++;
        }
        add_piece(screen, player->playerHand[0], column, row);
    } else {
        if (movingPlayer == 2) {
            column = screen->columnSize - 1;
            row = screen->rowSize - 1;
            automated_move_player_2(screen, &column, &row);
        } else {
            column = 0;
            row = 0;
            automated_move_player_1(screen, &column, &row);
        }
        column++;
        row++;
        add_piece(screen, player->playerHand[0], column, row);
    }
    printf("Player %d plays %d%c in column %d row %d\n", movingPlayer, 
            player->playerHand[0].number, player->playerHand[0].letter, 
            column, row);
    remove_player_piece(player, 0);
}

/**
*   //TODO
*/
void main_game_loop(char* deckLocation, Screen* screen, Player* player1, 
        Player* player2, Deck* deck) {
    int movingPlayer = 2;
    int cardsPlayed = 0;
    Player* currentPlayer;
    //Set current player
    if (player1->currentPlayer) {
        movingPlayer = 1;
    } 
    while (1) {
        if (is_game_over(screen, deck)) {
            break;
        } else {
            currentPlayer = get_current_player(player1, player2, movingPlayer);
            print_screen(screen);
            fflush(stdout);
            if (currentPlayer->handSize != 6) {
                deal_to_player(currentPlayer, deck);
            }
            if (currentPlayer->type == 'a') {
                print_player_hand(currentPlayer, 3);
                automated_move(screen, currentPlayer, movingPlayer);
            } else {
                print_player_hand(currentPlayer, movingPlayer);
                char* saveName;
                if (process_manual_move(&saveName, screen, currentPlayer, deck,
                        cardsPlayed) == 1) {
                    save_file(deckLocation, screen, deck, player1, player2, 
                            cardsPlayed, movingPlayer, saveName);
                    process_manual_move(&saveName, screen, currentPlayer, deck,
                            cardsPlayed);
                }
            }
            switch_player(player1, player2, &movingPlayer);
            cardsPlayed++;    
        }
    }
}


void find_longest_path(int originalRow, int originalColumn, int pathSize,
        Screen screen, int* biggestSize, char suit) {
    pathSize++;
    int right;
    int left;
    int below;
    int above;

    if (originalRow == screen.rowSize - 1) {
        below = 0;
    } else {
        below = originalRow + 1;
    }

    if (originalRow == 0) {
        above = screen.rowSize - 1;
    } else {
        above = originalRow - 1;
    }

    if (originalColumn == screen.columnSize - 1) {
        right = 0;
    } else {
        right = originalColumn + 1;
    }

    if (originalRow == 0) {
        left = screen.rowSize - 1;
    } else {
        left = originalColumn - 1;
    }

    /*
    (int originalRow, int originalColumn, int pathSize,
        Screen screen, int* biggestSize, char suit) {
    */
    
    if (pathSize > *biggestSize 
            && screen.board[originalRow][originalColumn].letter == suit) {
        *biggestSize = pathSize;
    } 

    if (screen.board[below][originalColumn].number > 
            screen.board[originalRow][originalColumn].number) {
        find_longest_path(below, originalColumn, pathSize, screen, biggestSize,
            suit);
    } 

    if (screen.board[originalRow][right].number > 
            screen.board[originalRow][originalColumn].number) {
        find_longest_path(right, originalColumn, pathSize, screen, biggestSize,
            suit);
    } 

    if (screen.board[above][originalColumn].number > 
            screen.board[originalRow][originalColumn].number) {
        find_longest_path(above, originalColumn, pathSize, screen, biggestSize,
            suit);
    } 

    if (screen.board[originalRow][left].number > 
            screen.board[originalRow][left].number) {
        find_longest_path(left, originalColumn, pathSize, screen, biggestSize,
            suit);
    } 
}

/**
*   haha main loop 
*/
int main(int argc, char** argv) {
    Screen screen;
    Deck deck;
    Player player1;
    Player player2;
    check_arg_types(argc, argv, &player1, &player2);
    FILE* gameFile;
    FILE* deckFile;
    char* deckLocation;
    // New Game 
    if (argc == 6) {
        screen.columnSize = atoi(argv[2]);
        screen.rowSize = atoi(argv[3]);
        initialise_screen(&screen);
        initialise_deck_file(argv[1], &deckFile, &deck);
        fflush(stdout);
        deckLocation = argv[1];
        player1.handSize = 0;
        player1.playerHand = malloc(sizeof(Piece) * 6);        
        deal_to_player(&player1, &deck);
        deal_to_player(&player1, &deck);
        deal_to_player(&player1, &deck);
        deal_to_player(&player1, &deck);
        deal_to_player(&player1, &deck);    
        player1.currentPlayer = 1;
        player2.handSize = 0;
        player2.playerHand = malloc(sizeof(Piece) * 6);        
        deal_to_player(&player2, &deck);
        deal_to_player(&player2, &deck);
        deal_to_player(&player2, &deck);
        deal_to_player(&player2, &deck);
        deal_to_player(&player2, &deck);    
        player2.currentPlayer = 1;
    } else {
        initialise_save_file(&deckLocation, argv[1], &gameFile, &deckFile, 
                &player1, &player2, &screen, &deck);
    }
    main_game_loop(deckLocation, &screen, &player1, &player2, &deck);
    print_screen(&screen);
    printf("Player 1=%d Player 2=%d\n", 4, 4);
    // free(deck.deckPile);
    // free(player1.playerHand);
    // free(player2.playerHand);
    // free(deckLocation);
    // free_board(&screen);
    return 0;
}