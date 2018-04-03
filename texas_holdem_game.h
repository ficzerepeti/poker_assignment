#pragma once

#include <string>
#include <vector>

#include "i_user_interaction.h"
#include "i_deck_interaction.h"

namespace poker_lib {

class texas_holdem_game
{
public:
    texas_holdem_game(i_user_interaction& user_interaction,
                      i_deck_interaction& deck_interaction,
                      size_t user_stack,
                      size_t big_blind_size,
                      size_t num_of_players,
                      size_t user_pos);

    void run_game();

private:
    struct player_state
    {
        std::vector<i_user_interaction::player_action_t> actions_taken;
        size_t user_stack = 0;
        bool is_our_user = false;

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
    i_deck_interaction& _deck_interaction;

    // per_game_state
    size_t _big_blind_size = 0;
    std::vector<player_state> _players;

    size_t _pot_size = 0;
    std::string _pocket_cards;
    std::string _board;
};

} // end of namespace poker_lib
