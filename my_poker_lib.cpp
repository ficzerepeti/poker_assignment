#include <omp/CardRange.h>
#include <omp/EquityCalculator.h>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include "my_poker_lib.h"

namespace poker_lib {

omp::EquityCalculator::Results calculate_equity_results(const table_state &table, const bool is_showdown)
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
        if (player.has_folded())
        {
            continue;
        }
        if (is_showdown && !player.per_game_state.pocket_cards)
        {
            std::ostringstream oss;
            oss << "During showdown active player " << player.player_name << " has no pocket cards set";
            throw std::runtime_error(oss.str());
        }
        hands.emplace_back(player.per_game_state.pocket_cards.value_or("random"));
    }

    omp::EquityCalculator eq;

    const auto valid_cards = eq.start(std::vector<omp::CardRange>{hands.begin(), hands.end()},
                                      omp::CardRange::getCardMask(table.communal_cards),
                                      omp::CardRange::getCardMask(""),
                                      is_showdown);

    if (!valid_cards)
    {
        std::ostringstream oss;
        oss << "Cannot evaluate table. Table: " << table;
        throw std::invalid_argument(oss.str());
    }

    eq.wait();

    return eq.getResults();
}

std::vector<double> calculate_equities(const table_state &table)
{
    const auto &equity_result = calculate_equity_results(table, false);

    std::vector<double> result;

    size_t equity_pos = 0;
    for (const auto &player : table.players)
    {
        const auto equity = player.has_folded() ? 0 : equity_result.equity[equity_pos++];
        result.emplace_back(equity);
    }

    return result;
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

player_analysis my_poker_lib::make_acting_player_analysis(const table_state &table,
                                                          const double raise_pot_ratio_begin,
                                                          const double raise_pot_ratio_end)
{
    player_analysis analysis;

    // TODO: take minimum amount to call into account

    analysis.equity = calculate_equities(table).at(table.acting_player_pos);
    analysis.pot_equity = calculate_pot_equity(table.pot, table.get_acting_player_amount_to_call());

    const auto min_raise = table.pot * raise_pot_ratio_begin;

    if (analysis.pot_equity > analysis.equity)
    {
        analysis.recommended_action = player_action_fold{};
    }
    else if (const auto max_plus_ev_increment = calculate_increment_to_get_pot_eq(table.pot, analysis.equity); max_plus_ev_increment >= min_raise)
    {
        const auto max_raise = std::min(static_cast<const uint64_t>(table.pot * raise_pot_ratio_end), max_plus_ev_increment);
        analysis.recommended_action = player_action_raise{std::min(max_raise, table.get_acting_player().current_stack)};
    }
    else
    {
        analysis.recommended_action = player_action_check_or_call{};
    }

    return analysis;
}

std::unordered_set<size_t> my_poker_lib::get_winner_positions(const table_state &table)
{
    std::ostringstream oss;
    if (table.current_stage != game_stages::showdown)
    {
        oss << "Expected showdown stage as current stage but instead got " << table.current_stage;
        throw std::invalid_argument(oss.str());
    }

    if (table.get_active_player_count() == 1)
    {
        for (size_t pos = 0; pos < table.players.size(); ++pos)
        {
            if (!table.players.at(pos).has_folded())
            {
                return {pos};
            }
        }
    }

    if (const auto card_count = get_num_of_parsed_cards(table.communal_cards); card_count != 5)
    {
        oss << "Expected all 5 communal cards dealt but instead got " << card_count;
        throw std::invalid_argument(oss.str());
    }

    const auto &equity_result = calculate_equity_results(table, true);

    if (equity_result.evaluations != 1)
    {
        oss << "During showdown only one evaluation is expected but got " << equity_result.evaluations << " instead. Table: " << table;
        throw std::logic_error(oss.str());
    }

    std::unordered_set<size_t> winners;

    size_t equity_pos = 0;
    for (const auto &player : table.players)
    {
        if (player.has_folded())
        {
            continue;
        }

        // OMPEval is a bit odd in case of multiple winners this both wins and ties are checked
        if (equity_result.wins.at(equity_pos) || equity_result.ties.at(equity_pos))
        {
            winners.emplace(equity_pos);
        }
        ++equity_pos;
    }

    return winners;
}

} // end of namespace poker_lib
