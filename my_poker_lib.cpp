#include <omp/CardRange.h>
#include <omp/EquityCalculator.h>
#include <sstream>
#include <iostream>
#include "my_poker_lib.h"

namespace poker_lib {

std::vector<double> calculate_equities(const table_state &table)
{
    if (table.players.size() > omp::MAX_PLAYERS)
    {
        std::ostringstream oss;
        oss << __func__ << ": requested calculation with " << table.players.size()
            << " but at most " << omp::MAX_PLAYERS << " are supported";

        throw std::invalid_argument(oss.str());
    }

    std::vector<omp::CardRange> hands;
    for (const auto &player : table.players)
    {
        hands.emplace_back(player.per_game_state.pocket_cards.value_or("random"));
    }

    omp::EquityCalculator eq;

    eq.start(std::vector<omp::CardRange>{hands.begin(), hands.end()},
             omp::CardRange::getCardMask(table.communal_cards),
             omp::CardRange::getCardMask(""),
             false);
    eq.wait();

    const auto &result = eq.getResults();
    return {result.equity, result.equity + hands.size()};
}

double calculate_pot_equity(uint64_t pot, uint64_t increment)
{
    return static_cast<double>(increment) / static_cast<double>(pot + increment);
}

uint64_t calculate_increment_to_get_pot_eq(const uint64_t pot, const double equity)
{
    return static_cast<uint64_t>(equity * pot / (1 - equity));
}

size_t my_poker_lib::get_num_of_parsed_cards(const std::string &cards) const
{
    return omp::bitCount(omp::CardRange::getCardMask(cards));
}

player_analysis my_poker_lib::make_acting_player_analysis(const table_state &table)
{
    player_analysis analysis;

    analysis.equity = calculate_equities(table).at(table.acting_player_pos);
    analysis.pot_equity = calculate_pot_equity(table.pot, table.get_acting_player_amount_to_call());

    const auto fixed_raise = table.pot * 2 / 3;

    if (analysis.pot_equity > analysis.equity)
    {
        analysis.recommended_action = player_action_fold{};
    }
    else if (const auto max_plus_ev_increment = calculate_increment_to_get_pot_eq(table.pot, analysis.equity); max_plus_ev_increment > fixed_raise)
    {
        analysis.recommended_action = player_action_raise{fixed_raise};
    }
    else
    {
        analysis.recommended_action = player_action_check_or_call{};
    }

    return analysis;
}

} // end of namespace poker_lib
