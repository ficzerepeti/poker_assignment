#include <sstream>
#include <type_traits>

#include "holdem_table_state_manager.h"

static size_t get_next_pos(const size_t current_pos, const size_t num_of_players)
{
    return (current_pos + 1) % num_of_players;
}

namespace poker_lib {

holdem_table_state_manager::holdem_table_state_manager(const size_t num_of_players,
                                         const size_t dealer_position,
                                         const uint64_t small_blind_size,
                                         const uint64_t big_blind_size)
:
    _current_stage(game_stages::deal_pocket_cards),
    _table_state()
{
    if (num_of_players < 2)
    {
        throw std::invalid_argument("Not enough players: " + std::to_string(num_of_players));
    }
    if (num_of_players <= dealer_position)
    {
        std::ostringstream oss;
        oss << "Player count " << num_of_players << " is not enough to have the dealer at position (zero indexed) " << dealer_position;
        throw std::invalid_argument(oss.str());
    }

    _table_state.small_blind_size = small_blind_size;
    _table_state.big_blind_size = big_blind_size;
    _table_state.pot = small_blind_size + big_blind_size;

    _table_state.dealer_pos = dealer_position;

    // Heads-up has special rules
    if (num_of_players == 2)
    {
        _table_state.small_blind_pos = dealer_position;
        _table_state.big_blind_pos = get_next_pos(dealer_position, num_of_players);
        _table_state.acting_player_pos = dealer_position;
    }
    else
    {
        _table_state.small_blind_pos = get_next_pos(dealer_position, num_of_players);
        _table_state.big_blind_pos = get_next_pos(_table_state.small_blind_pos, num_of_players);
        _table_state.acting_player_pos = get_next_pos(_table_state.big_blind_pos, num_of_players);
    }

    _table_state.players.resize(num_of_players, player_state{ big_blind_size, false, false, {} });
    _table_state.players.at(_table_state.small_blind_pos).amount_needed_to_call = small_blind_size;
    _table_state.players.at(_table_state.big_blind_pos).amount_needed_to_call = big_blind_size;
}

bool holdem_table_state_manager::set_pocket_cards(const size_t player_pos, const std::string &cards)
{
    if (_current_stage != game_stages::deal_pocket_cards)
    {
        return false;
    }

    _table_state.players.at(player_pos).pocket_cards = cards;
    _current_stage = get_next_game_stage(_current_stage);
    return true;
}

bool holdem_table_state_manager::set_flop(const std::string &cards)
{
    if (_current_stage != game_stages::deal_communal_cards)
    {
        return false;
    }

    _table_state.communal_cards = cards;
    _current_stage = get_next_game_stage(_current_stage);
    return true;
}

bool holdem_table_state_manager::set_turn(const std::string &card)
{
    if (_current_stage != game_stages::deal_turn_card)
    {
        return false;
    }

    _table_state.communal_cards += card;
    _current_stage = get_next_game_stage(_current_stage);
    return true;
}

bool holdem_table_state_manager::set_river(const std::string &card)
{
    if (_current_stage != game_stages::deal_river_card)
    {
        return false;
    }

    _table_state.communal_cards += card;
    _current_stage = get_next_game_stage(_current_stage);
    return true;
}

bool holdem_table_state_manager::set_acting_player_action(const player_action_t &action)
{
    switch (_current_stage)
    {
    case game_stages::deal_pocket_cards:
    case game_stages::deal_communal_cards:
    case game_stages::deal_turn_card:
    case game_stages::deal_river_card:
    case game_stages::showdown:
        return false;

    case game_stages::pre_flop_betting_round:
    case game_stages::flop_betting_round:
    case game_stages::turn_betting_round:
    case game_stages::river_betting_round:
        break;
    }

    std::visit([this](const auto& arg){ handle_betting_player_action(arg); }, action);

    const bool is_betting_still_ongoing = move_to_next_betting_player(_table_state);
    if (!is_betting_still_ongoing)
    {
        clear_per_betting_round_state_and_elect_next_acting_player(_table_state);
        const bool at_least_two_left = get_active_player_count(_table_state) > 1;
        _current_stage = at_least_two_left ? get_next_game_stage(_current_stage) : game_stages::showdown;
    }

    return true;
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_fold &action)
{
    player_state& player = _table_state.players.at(_table_state.acting_player_pos);

    player.has_called_or_checked_already = true;
    player.has_folded = true;
    player.amount_needed_to_call = 0;
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_check_or_call &action)
{
    player_state& player = _table_state.players.at(_table_state.acting_player_pos);

    player.has_called_or_checked_already = true;
    _table_state.pot += player.amount_needed_to_call;
    player.amount_needed_to_call = 0;
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_raise &action)
{
    player_state& player = _table_state.players.at(_table_state.acting_player_pos);

    _table_state.pot += player.amount_needed_to_call + action.amount_raised_above_call;

    // Update all player states to reflect this raise
    for (player_state& a_player : _table_state.players)
    {
        a_player.has_called_or_checked_already = false;
        a_player.amount_needed_to_call += action.amount_raised_above_call;
    }
    player.has_called_or_checked_already = true;
    player.amount_needed_to_call = 0;
}

} // end of namespace poker_lib
