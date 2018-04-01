#pragma once

#include "i_equity_calculator.h"

namespace poker_lib {

class equity_calculator : public i_equity_calculator
{
public:
    ~equity_calculator() override = default;

    std::vector<double> calculate_equities(const std::vector<std::string> &hands, const std::string &board) override;
};

} // end of namespace poker_lib
