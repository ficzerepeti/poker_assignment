#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "player_state.h"

namespace poker_lib {

struct table_state
{
    uint64_t small_blind_size;
    uint64_t big_blind_size;

    uint64_t pot;
    std::string communal_cards;

    std::vector<player_state> players;
    size_t acting_player_pos;
    size_t dealer_pos;
    size_t small_blind_pos;
    size_t big_blind_pos;

    player_state& get_current_player() { return players.at(acting_player_pos); }
    const player_state& get_current_player() const { return players.at(acting_player_pos); }

    // Number of players that haven't folded so far
    size_t get_active_player_count() const;

    // Returns false if no player may act in this round and sets up players for next betting round. True otherwise.
    bool move_to_next_betting_player();
};

} // end of namespace poker_lib
