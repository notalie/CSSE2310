#include <stdio.h>
#include <stdlib.h>

/* Defined a type of struct called piece, each piece contains a letter and 
 * number as stated by the Assignment spec so I thought this would be the 
 * most efficient way to implement this*/
typedef struct {
    char letter;
    int number;
} Piece;

/* Defined type Screen, Screen contains a column and row size to allow 
 * for easier printing of the board. Also contained in the screen type
 * is a board which is a 2D array of pieces allowing for easier printing*/
typedef struct {
    int rowSize;
    int columnSize;
    Piece** board;
} Screen;

/* Initilises the screen usitilising dynamic memory allocation 
 * Parameter: Screen pointer*/
void initialise_screen(Screen* screen) {
    screen->board = malloc(sizeof(Piece) * screen->columnSize);
    for(int i = 0; i < screen->rowSize; i++) {
        screen->board[i] = malloc(sizeof(Piece) * screen->rowSize);
    } 
} 

/* Prints the screen
 * Parameters: Screen pointer */
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

/* Frees the heap of memory being played on. 
 * This will only happen when the game ends
 * Parameters: The screen being played on
 * */
void free_board(Screen* screen) {
    for(int i = 0; i < screen->rowSize; i++) {
        free(screen->board[i]);   
     } 
    free(screen->board);
}

/* Adds a piece to the board if it is valid, if it is not valid return 0
 * Parameters: The screen being played on, the piece to be added,
 * and the x/ y coordinates for the piece to be placed on the board
 * Returns: 1 if adding the piece was successful, 0 if not
 * */
int add_piece(Screen* screen, Piece piece, int x, int y) {
    //User inputs will begin at 1 so decrement by 1
    x--;
    y--;
    if(screen->board[y][x].number > 0 && screen->board[y][x].number < 10) {
        //Do stuff
        printf("This is not valid\n");
        return 0;
    } else{
        screen->board[y][x] = piece;
        return 1;
    }
    //Do I want to reprint the board after it gets added? probs yeah
}



int main(int argc, char** argv) {
    Screen screen;
    screen.columnSize = 6;
    screen.rowSize = 6;
    initialise_screen(&screen);
    Piece x;
    x.number = 4;
    x.letter = 'A';
    add_piece(&screen, x, 2,3);
    print_screen(&screen);
    free_board(&screen);
    return 0;
}
