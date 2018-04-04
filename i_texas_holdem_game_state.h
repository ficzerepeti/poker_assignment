#pragma once

#include <cstdint>
#include <string>

#include "player_actions.h"

namespace poker_lib {

class i_texas_holdem_game_state
{
public:
    virtual bool set_pocket_cards(const std::string& cards) = 0;
    virtual bool set_flop(const std::string& card) = 0;
    virtual bool set_turn(const std::string& card) = 0;
    virtual bool set_river(const std::string& card) = 0;

    virtual bool set_user_action(const player_action_t& action) = 0;
    virtual bool set_opponent_action(size_t position, const player_action_t& action) = 0;

    virtual player_action_t get_user_recommended_action() = 0;
};

} // end of namespace poker_lib
