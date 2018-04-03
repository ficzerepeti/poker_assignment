#pragma once

#include <string>
#include <vector>

namespace poker_lib {

class i_my_poker_lib
{
public:
    virtual ~i_my_poker_lib() = default;

    virtual std::vector<double> calculate_equities(const std::vector<std::string> &hands, const std::string &board) = 0;
    virtual double calculate_pot_percentage(uint64_t pot, uint64_t raise) = 0;
    virtual uint64_t calculate_optimal_bet_size(uint64_t stack, uint64_t pot, double equity, size_t num_of_players) = 0;
};

} // end of namespace poker_lib
