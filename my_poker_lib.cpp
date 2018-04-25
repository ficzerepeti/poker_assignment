#include <omp/CardRange.h>
#include <omp/EquityCalculator.h>
#include <sstream>
#include "my_poker_lib.h"

namespace poker_lib {

size_t my_poker_lib::get_num_of_parsed_cards(const std::string &cards) const
{
    return omp::bitCount(omp::CardRange::getCardMask(cards));
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

double my_poker_lib::calculate_pot_equity(uint64_t pot, uint64_t increment)
{
    return 100 * static_cast<double>(increment) / static_cast<double>(pot + increment);
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

double my_poker_lib::calculate_expected_value(const table_state &table, double equity, size_t player_pos) const
{
    const auto amount_to_call = table.get_player_amount_to_call(player_pos);
    return (table.pot * equity) - (1 - equity) * amount_to_call;
}

double my_poker_lib::calculate_acting_player_expected_value(const table_state &table, double equity) const
{
    return calculate_expected_value(table, equity, table.acting_player_pos);
}

} // end of namespace poker_lib
