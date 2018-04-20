#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ostream>

#include "player_state.h"

namespace poker_lib {

struct table_state
{
    // Fields
    uint64_t small_blind_size;
    uint64_t big_blind_size;

    uint64_t pot;
    uint64_t total_contribution_to_stay_in_game;
    std::string communal_cards;

    std::vector<player_state> players;
    // Positions are 0 based
    size_t acting_player_pos;
    size_t dealer_pos;

    // Methods
    player_state& get_acting_player();
    const player_state& get_acting_player() const;
    uint64_t get_acting_player_amount_to_call() const;

    // Number of players that haven't folded so far
    size_t get_active_player_count() const;

    // Returns false if no player may act in this betting round meaning this round has finished. True otherwise.
    bool move_to_next_betting_player();

    void reset_per_betting_state();
    // Sets acting player to the first active player after the dealer.
    void elect_next_acting_player_after_betting();
};
bool operator==(const table_state &lhs, const table_state &rhs);
std::ostream &operator<<(std::ostream &os, const table_state &state);

size_t get_next_pos(size_t current_pos, size_t num_of_players);

} // end of namespace poker_lib
