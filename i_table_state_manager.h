#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "game_stages.h"
#include "player_actions.h"

namespace poker_lib {

struct player_state
{
    uint64_t amount_needed_to_call = 0;
    bool has_called_or_checked_already = false;
    bool has_folded = false;

    std::optional<std::string> pocket_cards;
};

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
};

class i_table_state_manager
{
public:
    virtual ~i_table_state_manager() = default;

    virtual game_stages get_current_game_stage() const = 0;
    virtual const table_state& get_table_state() const = 0;

    virtual bool set_pocket_cards(size_t player_pos, const std::string &cards) = 0;
    virtual bool set_flop(const std::string& card) = 0;
    virtual bool set_turn(const std::string& card) = 0;
    virtual bool set_river(const std::string& card) = 0;

    virtual bool set_acting_player_action(const player_action_t &action) = 0;
};

} // end of namespace poker_lib
