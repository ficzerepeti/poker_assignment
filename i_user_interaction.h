#pragma once

#include <variant>
#include "table/player_actions.h"

namespace poker_lib {

class i_user_interaction
{
public:
    virtual ~i_user_interaction() = default;

    virtual std::string get_pocket_cards() = 0;
    virtual std::string get_flop() = 0;
    virtual std::string get_turn() = 0;
    virtual std::string get_river() = 0;

    virtual player_action_t get_user_action(size_t position, const player_action_t& recommended_action) = 0;
    virtual player_action_t get_opponent_action(size_t position) = 0;

    virtual void notify_player(const std::string& message) = 0;
};

} // end of namespace poker_lib
