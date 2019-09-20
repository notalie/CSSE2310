#ifndef PLAYER_H
#define PLAYER_H

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
#define STDIN 0
#define STDOUT 1
#define MIN_INPUT_SIZE 8
#define LOWEST 1
#define HIGHEST 2

#include "player.h"
#include "structs.h"


/**
    Output error message for status and return status 
*/  
typedef enum {                                          
    GOOD = 0,                                             
    INCORRECTARGS = 1,                                      
    INVALIDPLAYERS = 2,                                     
    INVALIDPOSITION = 3,                                    
    BADTHRESHOLD = 4,                                    
    INVALIDHANDSIZE = 5,                                      
    INVALIDHUBMESSAGE = 6,                                      
    HUBEOF = 7                                        
} PlayerStatus;  

int main(int, char**); 
void init_hand(Player*, char*);
void init_player(Player*, char**);
int check_player_args(int, char**);
PlayerStatus show_player_message(PlayerStatus);
void next_player(int, int*);
int init_player_hand(char*, Player*);
int player_loop(Player*);
char* read_fd_line(int);
int sanitise_input(char*, Player*);
void init_new_round(Player*, char*);
void play_card(Player*, char*);
int can_play(Player*, char*); 
void game_over(Player*);
void alice_move(Player*);
void bob_move(Player*);
int  highest_card_loop(Player*, char*); 
void remove_player_card(Player*, int);
void lowest_card_loop(Player*,char*);
void contains_lead_suit(Player*, int*);
void find_lead_card(Player*, int, char);

#endif
