#include <iostream>

#include "omp/CardRange.h"
#include "omp/EquityCalculator.h"

int main() {
    omp::EquityCalculator eq;

    const std::vector<omp::CardRange> hands = { "Ad Qh", "random", "random", "random", "random", "random"};
    const auto board = omp::CardRange::getCardMask("Qd Th 2d");
    const auto dead = omp::CardRange::getCardMask("");

    eq.start(hands, board, dead, false);
    eq.wait();

    const auto &result = eq.getResults();

    std::cout << "First player wins " << result.equity[0] << std::endl;
    return 0;
}
