#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "table/player_actions.h"
#include "table/table_state.h"

namespace poker_lib {

struct player_analysis
{
    double equity = 0;
    double pot_equity = 0;

    player_action_t recommended_action;
};

class i_my_poker_lib
{
public:
    virtual ~i_my_poker_lib() = default;

    virtual size_t get_num_of_parsed_cards(const std::string &cards) const = 0;

    virtual player_analysis make_acting_player_analysis(const table_state &table,
                                                            double raise_pot_ratio_begin,
                                                            double raise_pot_ratio_end) = 0;
    virtual std::unordered_set<size_t> get_winner_positions(const table_state &table) = 0;
};

} // end of namespace poker_lib
