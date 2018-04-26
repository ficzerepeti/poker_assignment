#pragma once

#include "i_my_poker_lib.h"

namespace poker_lib {

class my_poker_lib : public i_my_poker_lib
{
public:
    ~my_poker_lib() override = default;

    size_t get_num_of_parsed_cards(const std::string &cards) const override;

    player_analysis make_acting_player_analysis(const table_state &table) override;

    std::vector<double> calculate_equities(const table_state &table) override;
    double calculate_expected_value(const table_state &table, double equity, size_t player_pos) const override;
    double calculate_acting_player_expected_value(const table_state &table, double equity) const override;

    double calculate_pot_equity(uint64_t pot, uint64_t increment) override;
    uint64_t calculate_optimal_bet_size(uint64_t stack, uint64_t pot, double equity, size_t num_of_players) override;
};

} // end of namespace poker_lib
