#include "player.h"

int main(int argc, char** argv) {
    Player player; 
    int status;
    if (check_player_args(argc, argv)) {
        return show_player_message(check_player_args(argc, argv));
    }
    init_player(&player, argv);
    player.type = 'b';
    printf("@"); // Initialised
    fflush(stdout); // Flush uwu
    char* hand = read_fd_line(STDIN); // Read Hand
    if (status = init_player_hand(hand, &player), status != 0) {
        return show_player_message(status);
    }
    return player_loop(&player); 
}