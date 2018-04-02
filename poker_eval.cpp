#include <omp/CardRange.h>
#include <omp/EquityCalculator.h>
#include <sstream>
#include "poker_eval.h"

namespace poker_lib {

std::vector<double> poker_eval::calculate_equities(const std::vector<std::string> &hands,
                                                          const std::string &board)
{
    if (hands.size() > omp::MAX_PLAYERS)
    {
        std::ostringstream oss;
        oss << __func__ << ": requested calculation with " << hands.size()
            << " but at most " << omp::MAX_PLAYERS << " are supported";

        throw std::invalid_argument(oss.str());
    }

    omp::EquityCalculator eq;

    eq.start(std::vector<omp::CardRange>{hands.begin(), hands.end()},
             omp::CardRange::getCardMask(board),
             omp::CardRange::getCardMask(""),
             false);
    eq.wait();

    const auto &result = eq.getResults();
    return {result.equity, result.equity + hands.size()};
}

double poker_eval::calculate_pot_percentage(uint64_t pot, uint64_t raise)
{
    const auto pot_d = static_cast<double>(pot);
    const auto raise_d = static_cast<double>(raise);
    return raise_d / (pot_d + raise_d);
}

} // end of namespace poker_lib
