#pragma once

#include <string>
#include <vector>

namespace poker_lib {

class i_equity_calculator
{
public:
    virtual ~i_equity_calculator() = default;

    virtual std::vector<double> calculate_equities(const std::vector<std::string> &hands, const std::string &board) = 0;
};

} // end of namespace poker_lib
