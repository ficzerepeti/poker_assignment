#include <sstream>
#include <type_traits>

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

holdem_table_state_manager::holdem_table_state_manager(const std::vector<initial_player_state> &players,
                                                       const size_t dealer_position,
                                                       const uint64_t small_blind_size,
                                                       const uint64_t big_blind_size)
:
    _current_stage(game_stages::deal_pocket_cards),
    _table_state()
{
    if (players.size() < 2)
    {
        throw std::invalid_argument("Not enough players: " + std::to_string(players.size()));
    }
    if (players.size() <= dealer_position)
    {
        std::ostringstream oss;
        oss << "Player count " << players.size() << " is not enough to have the dealer at position (zero indexed) " << dealer_position;
        throw std::invalid_argument(oss.str());
    }
    if (small_blind_size >= big_blind_size)
    {
        std::ostringstream oss;
        oss << "Small blind size " << small_blind_size << " is larger or equal to big blind size " << big_blind_size;
        throw std::invalid_argument(oss.str());
    }

    auto& table = _table_state.get_table_state();

    table.small_blind_size = small_blind_size;
    table.big_blind_size = big_blind_size;
    table.pot = small_blind_size + big_blind_size;

    table.dealer_pos = dealer_position;

    const bool is_heads_up = players.size() == 2;

    const auto small_blind_pos = is_heads_up ? dealer_position : get_next_pos(dealer_position, players.size());
    const auto big_blind_pos = get_next_pos(small_blind_pos, players.size());

    // Heads-up has special rules
    if (is_heads_up)
    {
        table.acting_player_pos = dealer_position;
    }
    else
    {
        table.acting_player_pos = get_next_pos(big_blind_pos, players.size());
    }

    // Initialise player states
    for (const auto &initial_state : players)
    {
        if (initial_state.current_stack == 0)
        {
            throw std::invalid_argument("All players need to have non-zero stack");
        }
        table.players.emplace_back(player_state{ initial_state.current_stack, {}, per_betting_player_state{big_blind_size, false} });
    }

    auto &small_blind = _table_state.get_player_at_pos(small_blind_pos);
    if (small_blind.current_stack < small_blind_size)
    {
        std::ostringstream oss;
        oss << "Small blind at position " << small_blind_pos
            << " only has " << small_blind.current_stack
            << " which is less than small blind " << small_blind_size;
        throw std::invalid_argument(oss.str());
    }
    small_blind.per_game_state.contribution_to_pot = small_blind_size;
    small_blind.per_betting_state.amount_needed_to_call = big_blind_size - small_blind_size;

    auto &big_blind = _table_state.get_player_at_pos(big_blind_pos);
    if (big_blind.current_stack < big_blind_size)
    {
        std::ostringstream oss;
        oss << "Big blind at position " << big_blind_pos
            << " only has " << big_blind.current_stack
            << " which is less than big blind " << big_blind_size;
        throw std::invalid_argument(oss.str());
    }
    small_blind.per_game_state.contribution_to_pot = big_blind_size;
    small_blind.per_betting_state.amount_needed_to_call = 0;
}

void holdem_table_state_manager::set_pocket_cards(size_t player_pos, const std::string &cards)
{
    _table_state.get_player_at_pos(player_pos).per_game_state.pocket_cards = cards;
    if (_current_stage == game_stages::deal_pocket_cards)
    {
        _current_stage = get_next_game_stage(_current_stage);
    }
}

void holdem_table_state_manager::set_flop(const std::string &cards)
{
    throw_if_unexpected_call(_current_stage, game_stages::deal_communal_cards, __func__);

    _table_state.set_communal_cards(cards);
    _current_stage = get_next_game_stage(_current_stage);
}

void holdem_table_state_manager::set_turn(const std::string &card)
{
    throw_if_unexpected_call(_current_stage, game_stages::deal_turn_card, __func__);

    auto new_communal_cards = _table_state.get_table_state().communal_cards;
    new_communal_cards += ' ';
    new_communal_cards += card;
    _table_state.set_communal_cards(new_communal_cards);

    _current_stage = get_next_game_stage(_current_stage);
}

void holdem_table_state_manager::set_river(const std::string &card)
{
    throw_if_unexpected_call(_current_stage, game_stages::deal_river_card, __func__);

    auto new_communal_cards = _table_state.get_table_state().communal_cards;
    new_communal_cards += ' ';
    new_communal_cards += card;
    _table_state.set_communal_cards(new_communal_cards);

    _current_stage = get_next_game_stage(_current_stage);
}

void holdem_table_state_manager::set_acting_player_action(const player_action_t &action)
{
    switch (_current_stage)
    {
    case game_stages::deal_pocket_cards:
    case game_stages::deal_communal_cards:
    case game_stages::deal_turn_card:
    case game_stages::deal_river_card:
    case game_stages::end_of_game:
    {
        std::ostringstream oss;
        oss << __func__ << " was called when current stage is " << _current_stage;
        throw std::logic_error(oss.str());
    }

    case game_stages::pre_flop_betting_round:
    case game_stages::flop_betting_round:
    case game_stages::turn_betting_round:
    case game_stages::river_betting_round:
        break;
    }

    std::visit([this](const auto& arg){ handle_betting_player_action(arg); }, action);

    const bool is_betting_still_ongoing = _table_state.move_to_next_betting_player();
    if (!is_betting_still_ongoing)
    {
        _table_state.reset_players_after_betting();
        _table_state.elect_next_acting_player_after_betting();

        const bool at_least_two_left = _table_state.get_active_player_count() > 1;
        _current_stage = at_least_two_left ? get_next_game_stage(_current_stage) : game_stages::end_of_game;
    }
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_fold &action)
{
    auto& player = _table_state.get_acting_player();

    player.per_betting_state = {};
    player.per_game_state.has_folded = true;
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_check_or_call &action)
{
    auto& player = _table_state.get_acting_player();

    const auto amount_contributed = std::min(player.current_stack, player.per_betting_state.amount_needed_to_call);

    _table_state.get_table_state().pot += amount_contributed;
    player.current_stack -= amount_contributed;
    player.per_game_state.contribution_to_pot += amount_contributed;

    player.per_betting_state.has_acted_in_betting = true;
    player.per_betting_state.amount_needed_to_call = 0;
}

void holdem_table_state_manager::handle_betting_player_action(const player_action_raise &action)
{
    auto& player = _table_state.get_acting_player();

    const bool is_really_a_raise = player.current_stack > player.per_betting_state.amount_needed_to_call;
    const auto amount_contributed = std::min(player.current_stack, player.per_betting_state.amount_needed_to_call + action.amount_raised_above_call);

    // Update all player states to reflect this raise
    if (is_really_a_raise)
    {
        const auto raised_amount = amount_contributed - player.per_betting_state.amount_needed_to_call;

        for (player_state& a_player : _table_state.get_table_state().players)
        {
            if (!a_player.has_folded())
            {
                a_player.per_betting_state.has_acted_in_betting = false;
                a_player.per_betting_state.amount_needed_to_call += raised_amount;
            }
        }
    }

    _table_state.get_table_state().pot += amount_contributed;
    player.current_stack -= amount_contributed;
    player.per_betting_state.has_acted_in_betting = true;
    player.per_betting_state.amount_needed_to_call = 0;
}

} // end of namespace poker_lib
