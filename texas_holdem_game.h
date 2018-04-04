#pragma once

#include <string>
#include <vector>

#include "i_user_interaction.h"

namespace poker_lib {

class texas_holdem_game
{
public:
    texas_holdem_game(i_user_interaction& user_interaction,
                      size_t user_stack,
                      size_t big_blind_size,
                      size_t num_of_players,
                      size_t user_pos);

    void run_game();

private:
    struct player_state
    {
        uint64_t stack = 0;

        std::vector<i_user_interaction::player_action_t> actions_taken;
        uint64_t investment_in_pot = 0;
        bool is_our_user = false;
        bool is_all_in = false;

        bool has_folded() const
        {
            return !actions_taken.empty() && std::holds_alternative<player_action_fold>(actions_taken.back());
        }
    };

    // Returns true if game ended. False otherwise
    bool run_betting_round();

    void execute_showdown();
    size_t get_active_player_count() const;

    i_user_interaction& _user_interaction;

    // per_game_state
    uint64_t _big_blind_size = 0;
    std::vector<player_state> _players;

    uint64_t _pot_size = 0;
    uint64_t _max_investment = 0;
    std::string _pocket_cards;
    std::string _board;
};

} // end of namespace poker_lib
