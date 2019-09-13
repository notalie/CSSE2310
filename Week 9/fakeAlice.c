#include "fakeAlice.h"

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

PlayerStatus show_player_message(PlayerStatus s) {                         
    const char* messages[] = {"",                       
            "Usage: player players myid threshold handsize\n",
            "Invalid players\n",
            "Invalid position\n",
            "Invalid threshold\n",
            "Invalid hand size\n",
            "Invalid message\n",
            "EOF\n"
};                              
    fputs(messages[s], stderr);                         
    return s;                                           
}  

char* read_fd_line(int fd) {
    char* result = malloc(sizeof(char) * 80);
    int position = 0;
    char buff[1];
    while (1) { 
        read(fd, buff, 1);
        if (buff[0] == EOF || buff[0] == '\n') {
            result[position] = '\0';
            return result;
        } else {
            result[position++] =  buff[0];
        }
    }
}

void init_player(Player* player, char** args) {
    player->id = atoi(PLAYER_POS);
    player->score = 0;
    player->diamondsWon = 0;
    player->hand = malloc(sizeof(Card) * atoi(HANDSIZE_ARGS));
    player->handSize = atoi(HANDSIZE_ARGS);
    player->isLead = atoi(PLAYER_POS)? 1 : 0;
    player->isLast = atoi(NUMPLAYER_ARGS) == player->id + 1? 1: 0;
    player->threshold = atoi(THRESHOLD_ARGS);
    player->playerDiamonds = malloc(sizeof(int) * atoi(NUMPLAYER_ARGS));
}

int init_player_hand(char* deck, Player* player) {
    if (deck[0] != 'H' || deck[1] != 'A' || deck[2] != 'N' || 
            deck[3] != 'D' || atoi(&deck[4]) != player->handSize) {
        return INVALIDHUBMESSAGE; // If the starting string isn't HAND
    } else if (player->handSize < 10 && (strlen(deck) - strlen("HAND") -
            SINGLEDIGITS - player->handSize) / 2 != player->handSize) {
        return INVALIDHUBMESSAGE; // Case if the hand size is a single digit
    } else if (player->handSize >= 10 && (strlen(deck) - strlen("HAND") -
            DOUBLEDIGITS - player->handSize) / 2 != player->handSize) {
        return INVALIDHUBMESSAGE; // Case if the hand size is double digits
    }
    int counter = 0; // Starting counter as the for 
                    // loop will go up in 3s for card, suit and ',''
    int iterator = player->handSize >= 10? DOUBLEDIGITS: SINGLEDIGITS;
    iterator += strlen("HAND") + strlen(","); // Create iterator for offset
    for (int i = 0; i < strlen(deck) - iterator; i += 3) {
        if (deck[i + iterator] != 'S' && deck[i + iterator] != 'C' && 
                deck[i + iterator]  != 'D' && deck[i + iterator] != 'H') {
            return INVALIDHUBMESSAGE; // Check if the suit is correct
        } else if ((deck[i + iterator + 1] < '1' || 
                deck[i + iterator + 1] > '9') && 
                (deck[i + iterator + 1] < 'a' || 
                deck[i + iterator + 1] > 'f')) {
            return INVALIDHUBMESSAGE; // Check if the deck is correct
        } else if (i < strlen(deck) - iterator - 3 && 
                deck[i + iterator + 2] != ',') {
            return INVALIDHUBMESSAGE; // Check if the ',' is correct
        }
        player->hand[counter].suit = deck[i + iterator]; // Add to hand
        player->hand[counter].rank = deck[i + iterator + 1]; // Add to hand
        counter++;
    }
    return 0;
}

/**
    Moves player counter up one, this function exits to avoid duplicated code
        and to save the need to calculate going one over the player limit
        and S E G F A U L T I N G
*/
void next_player(int amountOfPlayers, int* playerCounter) {
    if (*playerCounter + 1 == amountOfPlayers) {
        *playerCounter = 0;
    } else {
        (*playerCounter)++;
    }
} 

int last_player(int amountOfPlayers, int leadPlayer) {
    if (leadPlayer == 0) { // Check if the last player is the 0th player 
        return amountOfPlayers - 1;
    } else {
        return --leadPlayer;
    }
} 

int main(int argc, char** argv) { 
    Player player;
    init_player(&player, argv);
    printf("@");
    int status;
    if (status = init_player_hand(read_fd_line(0), &player), status != 0) {
        return show_player_message(status);
    }
    return 0;
}
