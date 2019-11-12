#ifndef HUB_H
#define HUB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h> 
#include <signal.h>
#include <sys/types.h>
#include <stdbool.h>
#include "structs.h"

/** 
    Defines a scoreboard that is in charge of managing the players IDS,
    their scores and their diamonds played. It also keeps track of the 
    current player in the game
*/
typedef struct { 
    int currentPlayer;
    int currentRound;   // Current round to signal game over
    Player* playerList; // an array of player structs
    pid_t* childPIDs;
} Scoreboard;

/** 
    Defines the struct for the hub data, contains a deck pointer, 
    number of players spawned, the number of diamonds played, 
    a pointer to an array of cards aka the board and a scoreboard
    to keep track of players and make displays easier
*/
typedef struct {
    FILE* deckFile;
    int numberOfPlayers;
    int diamondsPlayed;
    Card* playerBoard;  // For tracking which player played what
    Card* playerBoardOrdered; // For display
    Scoreboard scoreboard;
    int deckSize;
    int threshold;
    int leadPlayer;
    char leadSuit;
    char* childArgs[5];
    FILE** readFiles;
    FILE** writeFiles;
    char* deckName;
} Hub;

int check_args(int, char**, Hub*);
void signal_handler(int);
int check_deck_file(Hub*);
char* read_line(FILE*);
int game_loop(Hub*);
int check_dead_children(Hub*);
int spawn_children(Hub*, char**);
void get_child_args(Hub*, char**, int);
void init_player_hands(Hub*, int, char*, int);

void deal_to_player(Hub*);

#define READ_END 0
#define WRITE_END 1
#define SIGHUP 1
#define SIGPIPE 13
#define SIGCHILD 20

#define FILE_ARG args[1]
#define THRESHOLD_ARG args[2]
#define MIN_ARGS 4
#define INT_MAX 10
#define DOUBLE_DIGITS 2
#define SINGLE_DIGITS 1
#define ALL_CHILDREN -1
#define MAX_STRING_SIZE 10
#define MIN_STRING_SIZE 5
#define HANDSIZE (gameHub->deckSize/gameHub->numberOfPlayers)

#endif
