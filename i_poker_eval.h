#pragma once

#include <string>
#include <vector>

namespace poker_lib {

class i_poker_eval
{
public:
    virtual ~i_poker_eval() = default;

    virtual std::vector<double> calculate_equities(const std::vector<std::string> &hands, const std::string &board) = 0;
    virtual double calculate_pot_percentage(uint64_t pot, uint64_t raise) = 0;
};

} // end of namespace poker_lib
