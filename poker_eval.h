#pragma once

#include "i_poker_eval.h"

namespace poker_lib {

class poker_eval : public i_poker_eval
{
public:
    ~poker_eval() override = default;

    std::vector<double> calculate_equities(const std::vector<std::string> &hands, const std::string &board) override;
    double calculate_pot_percentage(uint64_t pot, uint64_t raise) override;
};

} // end of namespace poker_lib
