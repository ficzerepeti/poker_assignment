#include <iostream>

#include "deck_interaction.h"
#include "texas_holdem_game.h"
#include "streamed_user_interaction.h"

int main()
{
    poker_lib::streamed_user_interaction user_interaction(std::cout, std::cin);
    poker_lib::deck_interaction deck_interaction;

    poker_lib::texas_holdem_game game(user_interaction, deck_interaction, 1500, 20, 3, 2);

    game.run_game();

    return 0;
}