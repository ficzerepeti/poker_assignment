#pragma once

#include "table_state.h"

namespace poker_lib {

class table_state_manager
{
public:
    table_state& get_table_state() { return _table_state; }
    const table_state& get_table_state() const { return _table_state; }

    player_state& get_acting_player();
    const player_state& get_acting_player() const;
    player_state& get_player_at_pos(size_t player_pos);

    void set_communal_cards(const std::string& cards);

    // Number of players that haven't folded so far
    size_t get_active_player_count() const;

    // Returns false if no player may act in this betting round meaning this round has finished. True otherwise.
    bool move_to_next_betting_player();

    void reset_players_after_betting();
    // Sets acting player to the first active player after the dealer.
    void elect_next_acting_player_after_betting();

private:
    table_state _table_state{};
};

} // end of namespace poker_lib
