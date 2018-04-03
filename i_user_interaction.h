#pragma once

#include <variant>
#include "player_actions.h"

namespace poker_lib {

class i_user_interaction
{
public:
    using player_action_t = std::variant<player_action_fold, player_action_check, player_action_call, player_action_raise>;

    virtual ~i_user_interaction() = default;

    virtual player_action_t get_user_action(size_t position, const player_action_t& recommended_action) = 0;
    virtual player_action_t get_opponent_action(size_t position) = 0;
};

} // end of namespace poker_lib
