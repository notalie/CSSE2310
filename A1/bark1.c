#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char letter;
    int number;
} Piece;

typedef struct {
    int rowSize;
    int columnSize;
    Piece** visualBoard;
} Board;

void initialise_board(Piece*** board, int rowSize, int columnSize) {
    *board = malloc(sizeof(Piece) * rowSize);
    for(int i = 0; i < rowSize; i++) {
        (*board)[i] = malloc(sizeof(Piece) * columnSize);
    } 
} 

void print_board(Piece*** board, int rowSize, int columnSize) {
    Piece** tempBoard = *(board);
    for(int i = 0; i < rowSize; i++) {
        for(int j = 0; j < columnSize; j++) {
            if(tempBoard[i][j].number > 0 && tempBoard[i][j].number < 10) {
                printf("%d%c", tempBoard[i][j].number, tempBoard[i][j].letter);
            } else {
                printf("..");
            }
            if(j == columnSize-1) {
                printf("\n");
            } 
        }
    }
}

void add_piece(Piece*** board, int rowSize, int columnSize, int x, int y) {
    //User inputs will begin at 1
    x--;
    y--;
    Piece** tempBoard = *(board);
    if(tempBoard
    //Do I want to reprint the board after it gets added? probs yeah
}

int is_vaild_position(Piece*** board, int rowSize, int columnSize) {
    if(
}

void free_board(Piece*** board, int rowSize, int columnSize) {
    for(int i = 0; i < rowSize; i++) {
        free((*board)[i]);   
     } 
    free(*board);
}


int main(int argc, char** argv) {
    Piece** board; 
    int columnSize, rowSize;
    rowSize = 5;
    columnSize = 4;
    initialise_board(&board, rowSize, columnSize);
    board[0][0].letter = 'A';
    board[0][0].number = 4;
    board[1][2].number = 6;
    board[1][2].letter = 'x';
    print_board(&board, rowSize, columnSize);
    free_board(&board, rowSize, columnSize);
    return 0;
}
