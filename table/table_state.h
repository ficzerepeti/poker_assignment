#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ostream>

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
};
bool operator==(const table_state &lhs, const table_state &rhs);
std::ostream &operator<<(std::ostream &os, const table_state &state);

const player_state& get_current_player(const table_state& table);

// Number of players that haven't folded so far
size_t get_active_player_count(const table_state& table);

// Returns false if no player may act in this betting round meaning this round has finished. True otherwise.
bool move_to_next_betting_player(table_state& table);

void reset_players_after_betting(table_state& table);
// Sets acting player to the first active player after the dealer.
void elect_next_acting_player_after_betting(table_state &table);

size_t get_next_pos(size_t current_pos, size_t num_of_players);

} // end of namespace poker_lib
