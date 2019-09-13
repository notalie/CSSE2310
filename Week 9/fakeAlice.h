#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>   
#include <unistd.h>
#define NUMPLAYER_ARGS args[1] 
#define PLAYER_POS args[2]
#define THRESHOLD_ARGS args[3]
#define HANDSIZE_ARGS args[4]
#define DOUBLEDIGITS 2
#define SINGLEDIGITS 1

typedef struct {
    char suit;
    char rank;
} Card;

typedef struct {
    int id;
    int score;
    int diamondsWon;
    Card* hand;
    int handSize;
    bool isLead;
    int threshold;
    int* playerDiamonds;
    bool isLast;
} Player;

