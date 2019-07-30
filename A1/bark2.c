#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char letter;
    int number;
} Piece;

typedef struct {
    int rowSize;
    int columnSize;
    Piece** board;
} Screen;

void initialise_screen(Screen* screen) {
    screen->board = malloc(sizeof(Piece) * screen->columnSize);
    for(int i = 0; i < screen->rowSize; i++) {
        screen->board[i] = malloc(sizeof(Piece) * screen->rowSize);
    } 
} 

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

void free_board(Screen* screen) {
    for(int i = 0; i < screen->rowSize; i++) {
        free(screen->board[i]);   
     } 
    free(screen->board);
}


void add_piece(Screen* screen, int x, int y) {
    //User inputs will begin at 1
    x--;
    y--;
    Piece** tempBoard = screen->board;
    
    //Do I want to reprint the board after it gets added? probs yeah
}

int is_vaild_position(Piece*** board, int rowSize, int columnSize) {
    return 0;
}

int main(int argc, char** argv) {
    Screen screen;
    screen.columnSize = 6;
    screen.rowSize = 6;
    initialise_screen(&screen);
    screen.board[0][0].letter = 'A';
    screen.board[0][0].number = 4;
    screen.board[1][2].number = 6;
    screen.board[1][2].letter = 'x';
    screen.board[1][3].number = 4;
    screen.board[1][3].letter = 'S';
    print_screen(&screen);
    free_board(&screen);
    return 0;
}
