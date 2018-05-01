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
    game_stages current_stage = game_stages::deal_pocket_cards;

    uint64_t small_blind_size = 0;
    uint64_t big_blind_size = 0;

    uint64_t pot = 0;
    uint64_t total_contribution_to_stay_in_game = 0;
    std::string communal_cards;

    std::vector<player_state> players;
    // Positions are 0 based
    size_t acting_player_pos = 0;
    size_t dealer_pos = 0;

    // Methods
    player_state& get_acting_player();
    const player_state& get_acting_player() const;
    uint64_t get_player_amount_to_call(size_t player_pos) const;
    uint64_t get_acting_player_amount_to_call() const { return get_player_amount_to_call(acting_player_pos); }

    bool may_act_in_betting_round(size_t player_pos) const;

    // Number of players that haven't folded so far
    size_t get_active_player_count() const;
    size_t get_active_and_not_all_in_player_count() const;

    // Returns false if no player may act in this betting round meaning this round has finished. True otherwise.
    bool move_to_next_betting_player();

    // Sets acting player to the first active player after the dealer.
    void elect_next_acting_player_after_betting();
};
bool operator==(const table_state &lhs, const table_state &rhs);
std::ostream &operator<<(std::ostream &os, const table_state &state);

size_t get_next_pos(size_t current_pos, size_t num_of_players);

} // end of namespace poker_lib
