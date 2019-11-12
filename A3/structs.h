#ifndef STRUCT_H
#define STRUCT_H


/** 
    Defines the struct for a card that can be played, 
    contains a suit and a rank
*/
typedef struct {
    char suit;
    char rank;
} Card;

/**
    Define a struct player that will be inherited from 
    both the different versions
*/
typedef struct {
    char type;
    int id;
    int score;
    int diamondsWon;
    Card* hand;
    int handSize;
    int isLead;
    int lastPlayer;
    int threshold;
    int* playerDiamonds;
    int amountOfPlayers; // includes the amount of players 
                        // including the current player
    int diamondsInRound;
    char leadSuit;
    int leadId;
    Card* playedCards;
    int playedCardsCounter;
} Player;

#endif