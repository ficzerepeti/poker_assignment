#pragma once

#include "i_my_poker_lib.h"

namespace poker_lib {

std::vector<double> calculate_equities(const table_state &table);
double calculate_pot_equity(uint64_t pot, uint64_t increment);
uint64_t calculate_increment_to_get_pot_eq(uint64_t pot, double equity);

class my_poker_lib : public i_my_poker_lib
{
public:
    ~my_poker_lib() override = default;

    size_t get_num_of_parsed_cards(const std::string &cards) const override;
    player_analysis make_acting_player_analysis(const table_state &table) override;

    std::unordered_set<size_t> get_winner_positions(const table_state &table) override;
};

} // end of namespace poker_lib
