#pragma once

#include "i_my_poker_lib.h"

namespace poker_lib {

class my_poker_lib : public i_my_poker_lib
{
public:
    ~my_poker_lib() override = default;

    std::vector<double> calculate_equities(const std::vector<std::string> &hands, const std::string &board) override;
    double calculate_pot_percentage(uint64_t pot, uint64_t raise) override;
    uint64_t calculate_optimal_bet_size(uint64_t stack, uint64_t pot, double equity, size_t num_of_players) override;
};

} // end of namespace poker_lib
