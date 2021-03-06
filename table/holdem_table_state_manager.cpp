#include <algorithm>
#include <sstream>
#include <type_traits>
#include <set>

#include "holdem_table_state_manager.h"

static void throw_if_unexpected_call(const poker_lib::game_stages current_stage,
                                     const poker_lib::game_stages expected_stage,
                                     const char * const func_name)
{
    if (current_stage != expected_stage)
    {
        std::ostringstream oss;
        oss << "Call to " << func_name << " was expected to be called in stage "
            << expected_stage << " however current stage is " << current_stage;
        throw std::logic_error(oss.str());
    }
}

namespace poker_lib {

static game_stages get_next_card_deal_turn(const game_stages stage)
{
    switch (stage)
    {
    case game_stages::deal_pocket_cards:
    case game_stages::pre_flop_betting_round:
        return game_stages::deal_communal_cards;

    case game_stages::deal_communal_cards:
    case game_stages::flop_betting_round:
        return game_stages::deal_turn_card;

    case game_stages::deal_turn_card:
    case game_stages::turn_betting_round:
        return game_stages::deal_river_card;

    case game_stages::deal_river_card:
    case game_stages::river_betting_round:
    case game_stages::showdown:
        return game_stages::showdown;

    case game_stages::end_of_round:
        return game_stages::end_of_round;
    }

    return game_stages::end_of_round;
}

holdem_table_state_manager::holdem_table_state_manager(const std::vector<initial_player_state> &players,
                                                       const size_t dealer_position,
                                                       const uint64_t small_blind_size,
                                                       const uint64_t big_blind_size)
:
    _table_state()
{
    // Initialise player states
    for (const auto &initial_state : players)
    {
        if (initial_state.current_stack == 0)
        {
            throw std::invalid_argument("All players need to have non-zero stack");
        }
        _table_state.players.emplace_back(player_state{ initial_state.current_stack, {}, {}, initial_state.player_name });
    }

    _table_state.small_blind_size = small_blind_size;
    _table_state.big_blind_size = big_blind_size;
    _table_state.dealer_pos = dealer_position;

    set_up_table();
}

void holdem_table_state_manager::set_pocket_cards(size_t player_pos, const std::string &cards)
{
    _table_state.players.at(player_pos).per_game_state.pocket_cards = cards;

    if (_table_state.current_stage == game_stages::deal_pocket_cards)
    {
        move_to_next_stage_from_card_deal();
    }
}

void holdem_table_state_manager::set_flop(const std::string &cards)
{
    throw_if_unexpected_call(_table_state.current_stage, game_stages::deal_communal_cards, __func__);

    _table_state.communal_cards = cards;
    move_to_next_stage_from_card_deal();
}

void holdem_table_state_manager::set_turn(const std::string &card)
{
    throw_if_unexpected_call(_table_state.current_stage, game_stages::deal_turn_card, __func__);

    _table_state.communal_cards += ' ';
    _table_state.communal_cards += card;

    move_to_next_stage_from_card_deal();
}

void holdem_table_state_manager::set_river(const std::string &card)
{
    throw_if_unexpected_call(_table_state.current_stage, game_stages::deal_river_card, __func__);

    _table_state.communal_cards += ' ';
    _table_state.communal_cards += card;

    move_to_next_stage_from_card_deal();
}

void holdem_table_state_manager::set_acting_player_action(const player_action_t &action)
{
    switch (_table_state.current_stage)
    {
    case game_stages::deal_pocket_cards:
    case game_stages::deal_communal_cards:
    case game_stages::deal_turn_card:
    case game_stages::deal_river_card:
    case game_stages::showdown:
    case game_stages::end_of_round:
    {
        std::ostringstream oss;
        oss << __func__ << " was called when current stage is " << _table_state.current_stage;
        throw std::logic_error(oss.str());
    }

    case game_stages::pre_flop_betting_round:
    case game_stages::flop_betting_round:
    case game_stages::turn_betting_round:
    case game_stages::river_betting_round:
        break;
    }

    std::visit([this](const auto& arg){ handle_betting_player_action(arg); }, action);

    // Are there enough players who haven't folded yet?
    const auto active_player_count = _table_state.get_active_player_count();
    if (active_player_count < 2)
    {
        _table_state.current_stage = game_stages::showdown;
        return;
    }

    const bool is_betting_still_ongoing = _table_state.move_to_next_betting_player();
    if (is_betting_still_ongoing)
    {
        return;
    }

    _table_state.elect_next_acting_player_after_betting();

    // Are there enough players who can raise? If not then we just need to learn what all the communal cards are
    if (_table_state.get_active_and_not_all_in_player_count() < 2)
    {
        _table_state.current_stage = get_next_card_deal_turn(_table_state.current_stage);
    }
    else
    {
        const bool at_least_two_left = _table_state.get_active_player_count() > 1;
        _table_state.current_stage = at_least_two_left ? get_next_game_stage(_table_state.current_stage) : game_stages::showdown;
    }
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_fold &action)
{
    auto& player = _table_state.get_acting_player();
    player.per_game_state.has_folded = true;

    _table_state.get_acting_player().get_actions(_table_state.current_stage).emplace_back(action);
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_check_or_call &action)
{
    auto& player = _table_state.get_acting_player();

    const auto amount = std::min(_table_state.get_player_amount_to_call(_table_state.acting_player_pos),
                                 player.current_stack);

    _table_state.pot += amount;
    player.current_stack -= amount;
    player.per_game_state.contribution_to_pot += amount;

    _table_state.get_acting_player().get_actions(_table_state.current_stage).emplace_back(action);
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_raise &action)
{
    auto& player = _table_state.get_acting_player();

    const auto amount_to_call = _table_state.get_acting_player_amount_to_call();

    const bool is_really_a_raise = player.current_stack > amount_to_call;
    const auto amount_contributed = std::min(player.current_stack, amount_to_call + action.amount_raised_above_call);

    // Update all player states to reflect this raise
    if (is_really_a_raise)
    {
        const auto raised_amount = amount_contributed - amount_to_call;
        _table_state.total_contribution_to_stay_in_game += raised_amount;

        _table_state.get_acting_player().get_actions(_table_state.current_stage).emplace_back(player_action_raise{raised_amount});
    }
    else
    {
        _table_state.get_acting_player().get_actions(_table_state.current_stage).emplace_back(player_action_check_or_call{});
    }

    _table_state.pot += amount_contributed;
    player.current_stack -= amount_contributed;
    player.per_game_state.contribution_to_pot += amount_contributed;
}

std::vector<split_pot> holdem_table_state_manager::execute_showdown(const std::unordered_set<size_t> &winner_positions)
{
    throw_if_unexpected_call(_table_state.current_stage, game_stages::showdown, __func__);
    _table_state.current_stage = game_stages::end_of_round;

    const auto& players = _table_state.players;

    for (const auto &pos : winner_positions)
    {
        const auto &player = _table_state.players.at(pos);
        if (player.has_folded())
        {
            std::ostringstream oss;
            oss << "Player " << player.player_name << " at position " << pos << " is marked as winner as well as folded";
            throw std::invalid_argument(oss.str());
        }
    }

    std::set<uint64_t> contribution_limits;
    for (const auto &position : winner_positions)
    {
        const auto& player = players.at(position);
        contribution_limits.insert(player.per_game_state.contribution_to_pot);
    }

    std::vector<split_pot> split_pots;

    // Calculate split pots
    uint64_t prev_contr_limit = 0;
    for (const auto &limit : contribution_limits)
    {
        split_pots.emplace_back();
        auto& current_split = split_pots.back();

        for (size_t pos = 0; pos < players.size(); ++pos)
        {
            const auto& player = players.at(pos);
            if (player.per_game_state.contribution_to_pot > prev_contr_limit)
            {
                const auto increment = std::min(limit, player.per_game_state.contribution_to_pot) - prev_contr_limit;
                current_split.split_size += increment;

                if (winner_positions.count(pos) && player.per_game_state.contribution_to_pot >= limit)
                {
                    current_split.participant_positions.emplace(pos);
                }
            }
        }

        prev_contr_limit = limit;
    }

    // Give winners their chips
    for (const auto &split : split_pots)
    {
        const auto per_winner_split = split.split_size / split.participant_positions.size();

        for (const auto &pos : split.participant_positions)
        {
            auto& player = _table_state.players.at(pos);
            player.current_stack += per_winner_split;
        }
    }

    return split_pots;
}

bool holdem_table_state_manager::start_new_round()
{
    throw_if_unexpected_call(_table_state.current_stage, game_stages::end_of_round, __func__);
    _table_state.current_stage = game_stages::deal_pocket_cards;

    if (_table_state.start_new_round())
    {
        set_up_table();
        return true;
    }
    return false;
}

void holdem_table_state_manager::set_up_table()
{
    _table_state.current_stage = game_stages::deal_pocket_cards;
    if (_table_state.players.size() < 2)
    {
        throw std::invalid_argument("Not enough players: " + std::to_string(_table_state.players.size()));
    }
    if (_table_state.players.size() <= _table_state.dealer_pos)
    {
        std::ostringstream oss;
        oss << "Player count " << _table_state.players.size() << " is not enough to have the dealer at position (zero indexed) " << _table_state.dealer_pos;
        throw std::invalid_argument(oss.str());
    }
    if (_table_state.small_blind_size >= _table_state.big_blind_size)
    {
        std::ostringstream oss;
        oss << "Small blind size " << _table_state.small_blind_size << " is larger or equal to big blind size " << _table_state.big_blind_size;
        throw std::invalid_argument(oss.str());
    }

    // Heads-up has special rules
    const bool is_heads_up = _table_state.players.size() == 2;
    _table_state.acting_player_pos = is_heads_up ? _table_state.dealer_pos : get_next_pos(_table_state.dealer_pos, _table_state.players.size());

    auto &small_blind = _table_state.get_acting_player();
    if (small_blind.current_stack < _table_state.small_blind_size)
    {
        std::ostringstream oss;
        oss << "Small blind only has " << small_blind.current_stack
            << " which is less than small blind " << _table_state.small_blind_size;
        throw std::invalid_argument(oss.str());
    }
    handle_betting_player_action(player_action_raise{_table_state.small_blind_size});
    _table_state.get_acting_player().per_betting_state.pre_flop_bets.clear();
    _table_state.move_to_next_betting_player();

    auto &big_blind = _table_state.get_acting_player();
    if (big_blind.current_stack < _table_state.big_blind_size)
    {
        std::ostringstream oss;
        oss << "Big blind only has " << big_blind.current_stack
            << " which is less than big blind " << _table_state.big_blind_size;
        throw std::invalid_argument(oss.str());
    }
    handle_betting_player_action(player_action_raise{_table_state.big_blind_size - _table_state.small_blind_size});
    _table_state.get_acting_player().per_betting_state.pre_flop_bets.clear();
    _table_state.move_to_next_betting_player();
}

void holdem_table_state_manager::move_to_next_stage_from_card_deal()
{
    // Are there enough players who can raise? If not then we just need to learn what all the communal cards are
    const bool not_enough_players = _table_state.get_active_and_not_all_in_player_count() < 2;
    _table_state.current_stage = not_enough_players ? get_next_card_deal_turn(_table_state.current_stage) : get_next_game_stage(_table_state.current_stage);
}

} // end of namespace poker_lib
