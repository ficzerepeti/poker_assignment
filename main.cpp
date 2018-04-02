#include <iostream>

#include <omp/CardRange.h>
#include <omp/EquityCalculator.h>

#include "poker_eval.h"

int main()
{
    poker_lib::poker_eval eq;

    {
        const auto result = eq.calculate_equities({ "6d 6c", "random", "random", "random", "random", "random"}, "Kc 6h 2h");
        std::cout << "First player has " << (result.at(0) * 100) << "% chance to win" << std::endl;
    }
    {
        const auto result = eq.calculate_equities({ "Ad Ac", "random", "random", "random", "random", "random"}, "");
        std::cout << "First player has " << (result.at(0) * 100) << "% chance to win" << std::endl;
    }

    const uint64_t pot_size = 1000;
    const uint64_t raise = 500;
    const auto pot_percent = eq.calculate_pot_percentage(pot_size, raise);
    std::cout << "Pot odds (in percentage) for pot size " << pot_size << " and raise " << raise << " is " << pot_percent << '%' << std::endl;

    return 0;
}
