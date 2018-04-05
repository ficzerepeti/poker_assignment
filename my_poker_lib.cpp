#include <omp/CardRange.h>
#include <omp/EquityCalculator.h>
#include <sstream>
#include "my_poker_lib.h"

namespace poker_lib {

bool my_poker_lib::is_valid_card_str(const std::string &cards)
{
    return omp::CardRange::getCardMask(cards) != 0;
}

std::vector<double> my_poker_lib::calculate_equities(const std::vector<std::string> &hands, const std::string &board)
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

double my_poker_lib::calculate_pot_percentage(uint64_t pot, uint64_t raise)
{
    const auto pot_d = static_cast<double>(pot);
    const auto raise_d = static_cast<double>(raise);
    return raise_d / (pot_d + raise_d);
}

uint64_t my_poker_lib::calculate_optimal_bet_size(uint64_t stack, uint64_t pot, double equity, size_t num_of_players)
{
    // Split to low and high SPR
    const bool is_low_spr = (stack / pot) < 4;
    if (is_low_spr)
    {
        return 0; // TODO
    }

    return pot / 2; // TODO
}

} // end of namespace poker_lib
