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

typedef struct {
    Piece* playerHand;
    char type;
} Player;

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
    for(int i = 0; i < screen->rowSize; i++) {
        screen->board[i] = malloc(sizeof(Piece) * screen->rowSize);
    } 
} 

/** 
*   Prints the screen utilising the screen's row and column size
*   Parameters: Screen pointer 
*/
void print_screen(Screen* screen) {
    Piece** board = screen->board;
    for(int i = 0; i < screen->rowSize; i++) {
        for(int j = 0; j < screen->columnSize; j++) {
            if(board[i][j].number > 0 && board[i][j].number < 10) {
                printf("%d%c ", board[i][j].number, 
                    board[i][j].letter);
            } else {
                printf(".. ");
            }
            if(j == screen->columnSize-1) {
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
    for(int i = 0; i < screen->rowSize; i++) {
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
    if(screen->board[y][x].number > 0 && screen->board[y][x].number < 10) {
        printf("This is not valid\n");
        return 0;
    } else{
        // Adds piece to the board
        screen->board[y][x] = piece;
        return 1;
    }
}

/**
*   //TODO
*/
void change_playerhand_size(Piece** playerHand, int size) {
    *playerHand = realloc(playerHand, sizeof(Piece) * size);
}


//TODO
void process_move() {

}

/**
*   //TODO
*/
int main_game_loop() {
    //Check if stdin has ended, check what the player type is
    exit(7);
    return 1;
}

/**
*   //TODO
*/
void initialise_player_types(Player* player1, Player* player2, 
    int argc, char** argv) {
    if(argc == 6) {
        player1->type = argv[4][0];
        player2->type = argv[5][0];
    } else {
        player1->type = argv[2][0];
        player2->type = argv[3][0];
    }
}

/**
*   //TODO
*/
void check_arg_types(int amountofInputs, char** inputs) {
    if(amountofInputs != 6 && amountofInputs != 4) {
        fprintf(stderr, "Usage: bark savefile p1type p2type\n"
            "bark deck width height p1type p2type\n");
        exit(1);
    } else if(amountofInputs == 6 && (atoi(inputs[2]) < 3 || 
        atoi(inputs[2]) > 100|| atoi(inputs[3]) < 3 || 
            atoi(inputs[3]) > 100)) {
        fprintf(stderr, "Incorrect arg types\n");
        exit(2);
    } else if(amountofInputs == 6 && ((strcmp(inputs[4], "a") && 
        strcmp(inputs[4], "h")) || (strcmp(inputs[5], "a") && 
            strcmp(inputs[5], "h")))) {
        fprintf(stderr, "Incorrect arg types\n");
        exit(2);
    } else if(amountofInputs == 4 && ((strcmp(inputs[2], "a") 
        && strcmp(inputs[2], "h")) || (strcmp(inputs[3], "a") && 
            strcmp(inputs[3], "h")))) { 
        fprintf(stderr, "Incorrect arg types\n");
        exit(2);
    } 
}

/**
*   Reads the line and sends back a char pointer/char array
*/
char* read_line(FILE* fileName) {
    char* result = malloc(sizeof(char));
    int position = 0;
    int next = 0;
    while (1) { 
        next = fgetc(fileName);
        if (next == EOF || next == '\n') {
            result[position] = '\0';
            return result;
        } else {
            result[position] = (char)next;
            position++;
            result = realloc(result, sizeof(char) * position+1);
        }
    }
}

/** 
*   Prints the deck being played in the game
*   Parameters: Takes in a deck to print
*/
void print_deck(Deck deck) {
    for(int i = 0; i < deck.deckSize; i++) {
        printf("%d%c", deck.deckPile[i].number, deck.deckPile[i].letter);
    }
    printf("\n");
}


/* 
*   Initialises the deck file being read and checks if it is valid
*   Parameters
*   Exits in error status 3 if the deck file is invalid
*/
void initialise_deck_file(char* fileName, FILE** deckFile, Deck* deck) {
    *deckFile = fopen(fileName, "r");
    // Read first line
    int readDeckSize = atoi(read_line(*deckFile));
    int actualDeckSize = -1;
    char* readLine;
    while((strcmp(readLine, ""))) {
        readLine = read_line(*deckFile);
        actualDeckSize++;
    }
    fclose(*deckFile);
    *deckFile = fopen(fileName, "r");
    // Read it once to negate the number of cards in a deck
    read_line(*deckFile);
    if(actualDeckSize != readDeckSize) {
        exit(3);
    } else {
        // If amount of cards in deck are correct, allocate memory for deck and
        // put each card in the deck
        deck->deckSize = actualDeckSize;
        deck->deckPile = malloc(sizeof(char) * actualDeckSize);
        for(int i = 0; i < actualDeckSize; i++) {
            readLine = read_line(*deckFile);
            deck->deckPile[i].number = atoi(&readLine[0]);
            deck->deckPile[i].letter = readLine[1];
        }
        print_deck(*deck);
    }
}

/**
*   Initialises the save file, if it is invalid, exit with code 4, 
*   if it is not, start parsing it
*/
void initialise_save_file(char* fileName, FILE** gameFile, FILE** deckFile,
    Player* player1, Player* player2) {
    *gameFile = fopen(fileName, "r");
}


/**
*   haha main loop 
*/
int main(int argc, char** argv) {
    Screen screen;
    Deck deck;
    Player player1;
    Player player2;
    //check_arg_types(argc, argv);
    FILE *gameFile;
    FILE *deckFile;
    
    // New Game
    initialise_deck_file(argv[1], &deckFile, &deck);
    if(argc == 6) {
        screen.columnSize = atoi(argv[2]);
        screen.rowSize = atoi(argv[3]);
        initialise_deck_file(argv[1], &deckFile, &deck);
    } else {
       initialise_save_file(argv[1], &gameFile, &deckFile, &player1, &player2);
    }
	screen.rowSize = 4;
	screen.columnSize = 4;	
    initialise_screen(&screen);
    print_screen(&screen);
    return 0;
}























//no more
