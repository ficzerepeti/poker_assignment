#include <iostream>

#include "my_poker_lib.h"
#include "table_state_manager.h"
#include "texas_holdem_game_orchestrator.h"
#include "streamed_user_interaction.h"

int main()
{
    poker_lib::my_poker_lib poker_lib;
    poker_lib::streamed_user_interaction user_interaction(std::cout, std::cin);
    poker_lib::table_state_manager state_manager(3, 0, 10, 20);

    poker_lib::texas_holdem_game_orchestrator game(poker_lib, user_interaction, state_manager, 2);

    game.run_game();

    return 0;
}