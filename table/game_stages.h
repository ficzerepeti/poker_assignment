#pragma once

#include <ostream>

namespace poker_lib {

enum class game_stages
{
    deal_pocket_cards = 0,
    pre_flop_betting_round,
    deal_communal_cards,
    flop_betting_round,
    deal_turn_card,
    turn_betting_round,
    deal_river_card,
    river_betting_round,
    showdown,
    end_of_round,
};
std::ostream& operator<<(std::ostream& os, game_stages stage);

game_stages get_next_game_stage(game_stages stage);

} // end of namespace poker_lib
