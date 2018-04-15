#pragma once

#include <cstdint>
#include <string>

#include "table/game_stages.h"
#include "table/player_actions.h"
#include "table/table_state.h"

namespace poker_lib {

class i_table_state_manager
{
public:
    virtual ~i_table_state_manager() = default;

    virtual game_stages get_current_game_stage() const = 0;
    virtual const table_state& get_table_state() const = 0;
    virtual const player_state& get_acting_player_state() const = 0;

    virtual void set_pocket_cards(size_t player_pos, const std::string &cards) = 0;
    virtual void set_flop(const std::string &card) = 0;
    virtual void set_turn(const std::string &card) = 0;
    virtual void set_river(const std::string &card) = 0;

    virtual void set_acting_player_action(const player_action_t &action) = 0;
};

} // end of namespace poker_lib
