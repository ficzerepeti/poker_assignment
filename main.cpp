#include <iostream>

#include "omp/CardRange.h"
#include "omp/EquityCalculator.h"
#include "equity_calculator.h"

int main()
{
    poker_lib::equity_calculator eq;

    const auto result = eq.calculate_equities({ "Ad Qh", "random", "random", "random", "random", "random"}, "Qd Th 2d");

    std::cout << "First player has " << (result.at(0) * 100) << "% chance to win" << std::endl;
    return 0;
}
