#pragma once

#include "i_user_interaction.h"

namespace poker_lib {

class user_interaction : public i_user_interaction
{
public:
    ~user_interaction() override = default;

    std::string get_pocket_cards() override;

    std::string get_flop() override;

    std::string get_turn() override;

    std::string get_river() override;

    user_action_t get_user_action(size_t position,
                                  const user_action_t &recommended_action) override;

    user_action_t get_opponent_action(size_t position) override;

public:
};

} // end of namespace poker_lib
