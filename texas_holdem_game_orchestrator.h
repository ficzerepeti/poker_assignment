#pragma once

#include <string>
#include <vector>

#include "i_my_poker_lib.h"
#include "i_table_state_manager.h"
#include "i_user_interaction.h"

namespace poker_lib {

class texas_holdem_game_orchestrator
{
public:
    texas_holdem_game_orchestrator(i_my_poker_lib& poker_lib,
                                   i_user_interaction &user_interaction,
                                   i_table_state_manager &table_state_manager,
                                   size_t user_pos);

    void run_game();

private:
    player_action_t get_acting_player_action();
    double calculate_user_equity(const table_state& table) const;

    i_my_poker_lib& _poker_lib;
    i_user_interaction& _user_interaction;
    i_table_state_manager& _table_state_manager;

    const size_t _user_pos;
};

} // end of namespace poker_lib
