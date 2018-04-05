#pragma once

#include <string>

#include "player_actions.h"

namespace poker_lib {

struct user_analysis
{
    double equity = 0;
    player_action_t recommended_action;
};

class i_analysis_engine
{
public:
    virtual ~i_analysis_engine() = default;

    // Returns true if cards are in correct format. False otherwise
    virtual bool set_board(const std::string& cards) = 0;
    // Returns true if cards are in correct format. False otherwise
    virtual bool set_player_pocket_cards(size_t distance_from_dealer, const std::string& cards) = 0;

    virtual void set_player_folded(size_t distance_from_dealer) = 0;

    virtual user_analysis make_user_analysis(size_t distance_from_dealer) = 0;
};

} // end of namespace poker_lib
