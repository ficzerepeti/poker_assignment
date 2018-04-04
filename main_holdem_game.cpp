#include <iostream>

#include "my_poker_lib.h"
#include "texas_holdem_game.h"
#include "streamed_user_interaction.h"

int main()
{
    poker_lib::my_poker_lib poker_lib;
    poker_lib::streamed_user_interaction user_interaction(std::cout, std::cin);

    poker_lib::texas_holdem_game game(user_interaction, poker_lib, 1500, 20, 3, 2);

    game.run_game();

    return 0;
}