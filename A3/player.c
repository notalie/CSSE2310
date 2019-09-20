#include "player.h"

//TODO: Take out, for debugging purposes only
void print_player_hand(Player* player) {
    for (int i = 0; i < player->handSize; i++) {
        printf("%c%c", player->hand[i].suit, 
                player->hand[i].rank);
        if (i + 1 == player->handSize) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
}


/**
    The player loop for the designated player, loops until EOF is reached
*/
int player_loop(Player* player) {
    char* readLine;
    while (readLine = read_fd_line(STDIN), strlen(readLine) > 4) {
        if (sanitise_input(readLine, player)) {
            return show_player_message(sanitise_input(readLine, player));
        }
    } 
    return show_player_message(HUBEOF);
}

/**
    Sanitises the inputs and checks the first letter to make sure they 
    are valid, the rest of the string will be checked.
    The default case if for if none of the inputs are valid. The rest of the
    string is checked in their corresponding functions.
*/
int sanitise_input(char* string, Player* player) {
    switch (string[0]) {
        case 'N':   // New round
            init_new_round(player, string);
            break;
        case 'P':   // Player played
            play_card(player, string);
            break;
        case 'G':   // Game over message
            game_over(player);
            break;
        case 'E':   // EOF message
            exit(show_player_message(EOF));
        default:
            return INVALIDHUBMESSAGE;
    }  
    return 0;
}

/** 
*   Removes the player's piece depending on the index and 
*   shuffles up the user's deck
*   Takes in the player who is to have a piece removed,
*       the index to shift cards up to
*/
void remove_player_card(Player* player, int index) {
    player->playedCards[player->id] = player->hand[index];
    for (int i = index; i < player->handSize; i++) {
        player->hand[i] = player->hand[i + 1];
    }
    (player->handSize)--;
}

/**
    The loop for a finding the highest card with a list of suits, takes in a
    player pointer, the list of suits to check for and a card to initialise.
    in this case the 'highest' card which serves as the breakpoint for when a 
    card in the suit is initialised
*/
int highest_card_loop(Player* player, char* suits) {
    Card highestCard;
    highestCard.suit = 'A';
    highestCard.rank = 0;
    int index;  // Index to remove card at
    for (int s = 0; s < 4; s++) {
        for (int i = 0; i < player->handSize; i++) {
            if (player->hand[i].suit == suits[s] && 
                    player->hand[i].rank > highestCard.rank) {
                highestCard = player->hand[i];
                index = i;
                if (i == player->handSize - 1) {
                    printf("PLAY%c%c\n", highestCard.suit, highestCard.rank);
                    remove_player_card(player, index); 
                    fflush(stdout);
                    return 1;
                }
            } else if (i == player->handSize - 1 && highestCard.rank != 0) {
                printf("PLAY%c%c\n", highestCard.suit, highestCard.rank);
                remove_player_card(player, index); 
                fflush(stdout);
                return 1;
            }
        }
    }
    return 0;
}



void find_lead_card(Player* player, int mode, char suit) {
    Card highestCard;
    highestCard.suit = 'S';
    highestCard.rank = 0;
    Card lowestCard;
    lowestCard.suit = 'S';
    lowestCard.rank = 'g';
    int index;  // Index saving for hand management and removal
    if (mode == HIGHEST) {
        for (int i = 0; i < player->handSize; i++) {
            if (player->hand[i].rank > highestCard.rank && 
                    player->hand[i].suit == suit) {
                highestCard = player->hand[i];
                index = i;
            }
        } 
        printf("PLAY%c%c\n", lowestCard.suit, lowestCard.rank);
        fflush(stdout);
        remove_player_card(player, index); 
    } else { // LOWEST MODE
        for (int i = 0; i < player->handSize; i++) {
            if (player->hand[i].rank < lowestCard.rank && 
                    player->hand[i].suit == suit ) {
                lowestCard = player->hand[i];
                index = i; // Save index of card of smallest rank
            }
        }
        printf("PLAY%c%c\n", lowestCard.suit, lowestCard.rank);
        fflush(stdout);
        remove_player_card(player, index); 
    }
}

/**
    The logic for the alice AI, depending on the conditions set by the spec,
    different moves can be executed. It takes in a player that is to move
*/
void alice_move(Player* player) {
    char suitsMove1[4] = {'S', 'C', 'D', 'H'}; // Suit order for move 1
    char suitsMove3[4] = {'D', 'H', 'S', 'C'}; // Suit order for move 3
    int containsLeadSuit = 0;
    contains_lead_suit(player, &containsLeadSuit);
    if (player->isLead) {   // Lead player move, this is gonna get messy
        highest_card_loop(player, suitsMove1);
    } else if (containsLeadSuit) { // Contains lead suit
        find_lead_card(player, LOWEST, player->leadSuit);
    } else {    // Third Alice Move
        highest_card_loop(player, suitsMove3);
    }
}

/**
    Checks the player's hand to see if the player contains the lead 
    suit at play
*/
void contains_lead_suit(Player* player, int* containsLeadSuit) {
    if (!player->isLead) {
        for (int i = 0; i < player->handSize; i++) { // Check if lead suit in hand
            if (player->hand[i].suit == player->leadSuit) {
                *containsLeadSuit = 1;
            }
        }
    }
    
}

/**
    The loop for a finding the lowest card with a list of suits, takes in a
    player pointer, the list of suits to check for. The loop keeps going until 
    the lowest card is initialised (rank != 'g')
*/
void lowest_card_loop(Player* player, char* suits) {
    Card lowestCard;
    lowestCard.suit = 'S';
    lowestCard.rank = 'g';  // Start high and impossible, go lower
    int index;  // Index to remove card at
    for (int s = 0; s < 4; s++) {
        if (lowestCard.rank != 'g') { // Stop once rank is initialised
            printf("PLAY%c%c\n", lowestCard.suit, lowestCard.rank);
            remove_player_card(player, index); 
            fflush(stdout);
            break;
        }
        for (int i = 0; i < player->handSize; i++) {
            if (player->hand[i].suit == suits[s] && 
                    player->hand[i].rank < lowestCard.rank) {
                lowestCard = player->hand[i];
                index = i;
            }
        }
    }
}

//TODO:
void bob_move(Player* player) {
    char suitsMove1[4] = {'D', 'H', 'S', 'C'}; // Suit order for move 1
    char suitsMove2[4] = {'S', 'C', 'H', 'D'}; // Suit order for move 2
    char suitsMove4[4] = {'S', 'C', 'D', 'H'}; // Suit order for move 4
    int containsLeadSuit = 0;
    int thresholdReached = 0;
    contains_lead_suit(player, &containsLeadSuit);
    //A player has reached threshold & diamonds have been played
    for (int i = 0; i < player->amountOfPlayers; i++) {
        if (player->playerDiamonds[i] >= player->threshold && 
                player->diamondsInRound > 0) {
            thresholdReached = 1;
        }
    }
    if (player->isLead) {
        lowest_card_loop(player, suitsMove1);
    } else if (thresholdReached) {
        if (containsLeadSuit) { // Has lead suit
             find_lead_card(player, HIGHEST, player->leadSuit);
        } else {
            lowest_card_loop(player, suitsMove2);
        }
    } else if (containsLeadSuit) {
        find_lead_card(player, LOWEST, player->leadSuit);
    } else {
        highest_card_loop(player, suitsMove4); //Same as alice move
    }
}

/**
    Plays the card if the person who played the card was the player before the
    current one. 
*/
void play_card(Player* player, char* string) {
    if (strncmp(string, "PLAYED", strlen("PLAYED"))) {
        exit(show_player_message(INVALIDHUBMESSAGE));
    }
    // Only play if the person who just played was before you
    if (can_play(player, string)) { 
        player->type == 'a' ? alice_move(player) : bob_move(player);
    } 
}

/**
    Checks if the current player can play, it also checks if the PLAYED
    message is valid. This method also changes all the diamonds currently 
    played in the round
*/
int can_play(Player* player, char* string) {
    int recentPlayer = atoi(&string[strlen("PLAYED")]);
    char suitPlayed = string[strlen("PLAYED") + 2]; // ,S is +2 on PLAYED index
    char rankPlayed = string[strlen("PLAYED") + 3]; // ,SR is +3 on index
    if (suitPlayed != 'S' && suitPlayed != 'D' && suitPlayed != 'C' && 
            suitPlayed != 'H') {
        exit(show_player_message(INVALIDHUBMESSAGE));
    } else if ((rankPlayed < '1' || rankPlayed > '9') && (rankPlayed < 'a' || 
            rankPlayed > 'f')) {
        exit(show_player_message(INVALIDHUBMESSAGE));
    } else if (recentPlayer == player->id) {
         exit(show_player_message(INVALIDHUBMESSAGE));
    } else if (recentPlayer >= player-> amountOfPlayers) {
        exit(show_player_message(INVALIDHUBMESSAGE));
    }
    // Update tracker of diamonds and current diamonds in round
    if (suitPlayed == 'D') {
        (player->playerDiamonds[recentPlayer])++;
        (player->diamondsInRound)++;
    }
    if (player->leadId == recentPlayer) {
        player->leadSuit = suitPlayed;
    }
    player->playedCards[recentPlayer].suit = suitPlayed;
    player->playedCards[recentPlayer].rank = rankPlayed;
    // If the player was the last player e.g. player 5 in a 6 player game
    // the player can play, this is a special base case for player 0
    // If the most recent player was the last player, only for games with
    // greater than 2 players otherwise one player won't play
    if (recentPlayer == player->lastPlayer && player->amountOfPlayers > 2) {
        return 0;
    } else if (player->id == 0 && recentPlayer 
            == player->amountOfPlayers - 1) {
        return 1;
    } else if (player->id == recentPlayer + 1) {
        return 1;
    }
    return 0;
}

void print_round_results(Player* player) {
    fprintf(stderr, "Lead player=%d: ", player->leadId);
    for (int i = 0; i < player->amountOfPlayers; i++) {
        fprintf(stderr, "%c.%c", player->playedCards[i].suit, 
                player->playedCards[i].rank);
        if (i != player->amountOfPlayers - 1) {
             fprintf(stderr, " ");
        } 
    }
    fprintf(stderr, "\n");
}

/**
    Check if the lead player is this player, if it is, 
    add to total diamonds earnt. Also increments the score for the player
    If the message is invalid (not NEWROUND but starting with N), exit with
    INVALIDHUBMESSAGE
*/
void init_new_round(Player* player, char* string) {
    if (!strncmp(string, "NEWROUND", strlen("NEWROUND"))) {
        int leadPlayer = atoi(&string[strlen("NEWROUND")]);
        if (leadPlayer > player->amountOfPlayers) {
            exit(show_player_message(INVALIDHUBMESSAGE));
        }
        if (leadPlayer == player->id) {
            player->isLead = 1;
            player->diamondsWon = player->diamondsInRound;
            if (0 == player->id && player->score == -1) { //First ever move
                player->type == 'a' ? alice_move(player) : bob_move(player);
            }
            (player->score)++;
        } else {
            player->isLead = 0;
        }
        if (leadPlayer == 0) { // Wrapping around queue
            player->lastPlayer = player->amountOfPlayers - 1;
        } else {
            player->lastPlayer = --leadPlayer;
        }
        if (player->playedCards[0].suit != 0) {
            print_round_results(player);
        }
        player->leadId = leadPlayer;
        player->diamondsInRound = 0;
    } else {    // Message doesn't have newround in it
        exit(show_player_message(INVALIDHUBMESSAGE));
    }
}

/**
    Frees everything that the player has and exits normally, exists in case
    I malloc more stuff later on, thanks for coming my ted talk
*/
void game_over(Player* player) {
    free(player->hand);
    free(player->playerDiamonds);
    print_round_results(player);
    exit(0);
}
                                    
/**
    Output error message for status and return status 
*/   
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

/**
    Initialises the player's cards, it also checks that the cards are formatted
    correctly and that the 
*/
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
    int iterator = player->handSize >= 10 ? DOUBLEDIGITS : SINGLEDIGITS;
    iterator += strlen("HAND") + strlen(","); // Create iterator for offset
    for (int i = 0; i < strlen(deck) - iterator; i += 3) {
        if (deck[i + iterator] != 'S' && deck[i + iterator] != 'C' && 
                deck[i + iterator] != 'D' && deck[i + iterator] != 'H') {
            return INVALIDHUBMESSAGE; // Check if the suit is correct
        } else if ((deck[i + iterator + 1] < '1' || 
                deck[i + iterator + 1] > '9') && 
                (deck[i + iterator + 1] < 'a' || 
                deck[i + iterator + 1] > 'f')) {
            return INVALIDHUBMESSAGE; // Check if the deck is correct
        } else if (i < strlen(deck) - iterator - 3 && 
                deck[i + iterator + 2] != ',' && player->handSize > 1) {
            return INVALIDHUBMESSAGE; // Check if the ',' is correct
        }
        player->hand[counter].suit = deck[i + iterator]; // Add to hand
        player->hand[counter].rank = deck[i + iterator + 1]; // Add to hand
        counter++;
    }
    return 0;
}

void zero_out_played_cards(Player* player) {
    for (int i = 0; i < player->amountOfPlayers; i++) {
        player->playedCards[i].rank = 0;
        player->playedCards[i].suit = 0; 
    }
}

/**
    Initialises the player once all of the args have been confirmed 
        to be valid, it also mallocs the player's hand and 
        tracks the amount of diamonds each player has mainly for bob logic
*/
void init_player(Player* player, char** args) {
    player->type = args[0][6];
    player->id = atoi(PLAYER_POS);
    player->score = 0; 
    player->diamondsWon = 0;
    player->hand = malloc(sizeof(Card) * atoi(HANDSIZE_ARGS));
    player->handSize = atoi(HANDSIZE_ARGS);
    player->isLead = 0;
    if (player->id == 0) {
        player->score = -1; // Base case as each new round increments the 
                            //new round player by 1
    } 
    player->lastPlayer = atoi(NUMPLAYER_ARGS) - 1;
    player->threshold = atoi(THRESHOLD_ARGS);
    player->playerDiamonds = malloc(sizeof(int) * atoi(NUMPLAYER_ARGS));
    player->amountOfPlayers = atoi(NUMPLAYER_ARGS);
    player->diamondsInRound = 0;
    player->playedCards = malloc(sizeof(Card) * atoi(NUMPLAYER_ARGS)); //TODO: Change
}

/**
    Checks the args passed in and returns the corresponding 
    statuses if the input entered is incorrect according to the specification
*/
int check_player_args(int argsNum, char** args) {
    if (argsNum != 5) {
        return INCORRECTARGS;
    } 
    if ((strlen(NUMPLAYER_ARGS) == 1 && NUMPLAYER_ARGS[0] < '2') || 
            NUMPLAYER_ARGS[0] == 0) {    //Check for Invalid Players
        return INVALIDPLAYERS;
    } 
    for (int i = 0; i < strlen(NUMPLAYER_ARGS); i++) { 
        if (NUMPLAYER_ARGS[i] < '0' || NUMPLAYER_ARGS[i] > '9') {
            return INVALIDPLAYERS;
        }
    }
    if (strlen(PLAYER_POS) == 1 && (atoi(PLAYER_POS) < 0 || // Check Position
            atoi(PLAYER_POS) >= atoi(NUMPLAYER_ARGS))) {
        return INVALIDPOSITION;
    } else if (PLAYER_POS[0] == 0) {
        return INVALIDPOSITION;
    }
    for (int i = 0; i < strlen(PLAYER_POS); i++) { 
        if (PLAYER_POS[i] < '0' || PLAYER_POS[i] > '9') {
            return INVALIDPOSITION;
        }
    }
    if (strlen(THRESHOLD_ARGS) == 1 && THRESHOLD_ARGS[0] < '2') {
        return BADTHRESHOLD; //// Check Threshold
    } else if (THRESHOLD_ARGS[0] == 0) {
        return BADTHRESHOLD;
    }

    for (int i = 0; i < strlen(THRESHOLD_ARGS); i++) { 
        if (THRESHOLD_ARGS[i] < '0' || THRESHOLD_ARGS[i] > '9') {
            return BADTHRESHOLD;
        }
    }

    // Check Hand size
    if (strlen(HANDSIZE_ARGS) == 1 && HANDSIZE_ARGS[0] < '1') {
        return INVALIDHANDSIZE;
    } else if (HANDSIZE_ARGS[0] == 0) {
        return INVALIDHANDSIZE;
    }

    for (int i = 0; i < strlen(HANDSIZE_ARGS); i++) { 
        if (HANDSIZE_ARGS[i] < '0' || HANDSIZE_ARGS[i] > '9') {
            return INVALIDHANDSIZE;
        }
    }
    return GOOD;
} 
