#include <iostream>
#include <sstream>

#include "my_poker_lib.h"
#include "holdem_game_orchestrator.h"
#include "streamed_user_interaction.h"
#include "table/initial_player_state.h"
#include "table/holdem_table_state_manager.h"

static std::vector<poker_lib::initial_player_state> get_num_of_players_and_stacks()
{
    std::cout << "Before a game starts I need to know what are the player names and their current stack size is.\n"
                 "Please use the format: <stack_size> <player_name>. Entering an empty line finishes the input\n";

    std::vector<poker_lib::initial_player_state> result;

    std::string input;
    while (std::getline(std::cin, input) && !input.empty())
    {
        std::istringstream iss(input);

        result.emplace_back();
        iss >> result.back().current_stack;
        iss >> result.back().player_name;
    }

    return result;
}

static size_t get_user_position()
{
    std::cout << "What is your position (zero based index)?\n";

    std::string input;
    while (std::getline(std::cin, input) && !input.empty())
    {
        return std::stoull(input);
    }
}

int main()
{
    poker_lib::my_poker_lib poker_lib;
    poker_lib::streamed_user_interaction user_interaction(std::cout, std::cin);
    poker_lib::holdem_table_state_manager state_manager(get_num_of_players_and_stacks(), 0, 10, 20);

    poker_lib::holdem_game_orchestrator game(poker_lib, user_interaction, state_manager, get_user_position());

    game.run_game();

    return 0;
}
