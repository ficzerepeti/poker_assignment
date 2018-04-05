#pragma once

#include <string>
#include <vector>

#include "i_game_engine.h"
#include "i_user_interaction.h"

namespace poker_lib {

class texas_holdem_game
{
public:
    texas_holdem_game(i_user_interaction &user_interaction,
                      i_game_engine &game_analysis_engine,
                      size_t user_stack,
                      size_t big_blind_size,
                      size_t num_of_players,
                      size_t user_pos);

    void run_game();

private:

    // Returns true if game ended. False otherwise
    bool run_betting_round();

    void execute_showdown();
    size_t get_active_player_count() const;

    double calculate_user_equity() const;

    i_user_interaction& _user_interaction;
    i_game_engine& _game_analysis_engine;

    // per_game_state
    uint64_t _big_blind_size = 0;

    uint64_t _pot_size = 0;
    uint64_t _max_investment = 0;
    std::string _pocket_cards;
    std::string _board;
};

} // end of namespace poker_lib
