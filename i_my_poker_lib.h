#pragma once

#include <string>
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

    virtual player_analysis make_acting_player_analysis(const table_state &table) = 0;

    virtual std::vector<double> calculate_equities(const table_state &table) = 0;
    virtual double calculate_expected_value(const table_state &table, double equity, size_t player_pos) const = 0;
    virtual double calculate_acting_player_expected_value(const table_state &table, double equity) const = 0;

    virtual double calculate_pot_equity(uint64_t pot, uint64_t increment) = 0;
    virtual uint64_t calculate_optimal_bet_size(uint64_t stack, uint64_t pot, double equity, size_t num_of_players) = 0;
};

} // end of namespace poker_lib
