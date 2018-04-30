#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>
#include <type_traits>
#include <map>

#include "holdem_game_orchestrator.h"

namespace poker_lib {

holdem_game_orchestrator::holdem_game_orchestrator(i_my_poker_lib& poker_lib,
                                                   i_user_interaction &user_interaction,
                                                   i_table_state_manager &table_state_manager,
                                                   const size_t user_pos)
:
    _poker_lib(poker_lib),
    _user_interaction(user_interaction),
    _table_state_manager(table_state_manager),
    _user_pos(user_pos)
{
}

template<typename FuncT>
std::string holdem_game_orchestrator::read_valid_cards(FuncT get_cards, const size_t expected_num_of_cards)
{
    std::string cards;

    const auto& current_board = _table_state_manager.get_table_state().communal_cards;
    const auto board_cards_count = _poker_lib.get_num_of_parsed_cards(current_board);
    while (true)
    {
        cards = get_cards();
        if (_poker_lib.get_num_of_parsed_cards(cards) != expected_num_of_cards)
        {
            _user_interaction.notify_player("Cannot parse card(s), please try again");
        }
        else if (_poker_lib.get_num_of_parsed_cards(current_board + cards) != (board_cards_count + expected_num_of_cards))
        {
            _user_interaction.notify_player("At least one of the cards is already on the board");
        }
        else
        {
            return cards;
        }
    }
}

void holdem_game_orchestrator::run_game()
{
    while (true)
    {
        switch (_table_state_manager.get_table_state().current_stage)
        {
        case game_stages::deal_pocket_cards:
            _table_state_manager.set_pocket_cards(_user_pos, read_valid_cards([this](){ return _user_interaction.get_user_pocket_cards(); }, 2));
            break;

        case game_stages::deal_communal_cards:
            _table_state_manager.set_flop(read_valid_cards([this](){ return _user_interaction.get_flop(); }, 3));
            break;

        case game_stages::deal_turn_card:
            _table_state_manager.set_turn(read_valid_cards([this](){ return _user_interaction.get_turn(); }, 1));
            break;

        case game_stages::deal_river_card:
            _table_state_manager.set_river(read_valid_cards([this](){ return _user_interaction.get_river(); }, 1));
            break;

        case game_stages::pre_flop_betting_round:
        case game_stages::flop_betting_round:
        case game_stages::turn_betting_round:
        case game_stages::river_betting_round:
            _user_interaction.notify_player(table_state_and_stage_to_user_message());
            _table_state_manager.set_acting_player_action(get_acting_player_action());
            break;

        case game_stages::showdown:
            execute_showdown();
            _user_interaction.notify_player("State after this game after showdown: " + table_state_and_stage_to_user_message());
            break;

        case game_stages::end_of_game:
            // TODO: new game?
            _user_interaction.notify_player("Game ended, bye!");
            return;
        }

        _user_interaction.notify_player("\n\n\n\n\n\n");
    }
}

player_action_t holdem_game_orchestrator::get_acting_player_action()
{
    const auto& table = _table_state_manager.get_table_state();

    const bool is_opponent = table.acting_player_pos != _user_pos;
    if (is_opponent)
    {
        return _user_interaction.get_opponent_action();
    }

    const auto analysis = _poker_lib.make_acting_player_analysis(table);

    std::ostringstream oss;
    oss << "Your equity of winning is " << (100 * analysis.equity)
        << "% and your pot equity is " << (100 * analysis.pot_equity) << "%. Your recommended action is ";
    std::visit([&](const auto &obj){ oss << obj << std::endl; }, analysis.recommended_action);
    _user_interaction.notify_player(oss.str());

    return _user_interaction.get_user_action();
}

std::string holdem_game_orchestrator::table_state_and_stage_to_user_message() const
{
    const auto& table = _table_state_manager.get_table_state();

    std::ostringstream oss;
    oss << std::left;
    oss << table.current_stage << ": pot size: " << table.pot;
    if (!table.communal_cards.empty())
    {
        oss << ", communal cards: " << table.communal_cards;
    }
    oss << ", big blind size: " << table.big_blind_size;

    constexpr std::array<int, 4> text_width{10, 25, 20, 40};
    oss << std::endl << std::setw(text_width.at(0)) << "name"
                     << std::setw(text_width.at(1)) << "attributes"
                     << std::setw(text_width.at(2)) << "stack size"
                     << std::setw(text_width.at(3)) << "action";
    for (size_t pos = 0; pos < table.players.size(); ++pos)
    {
        const auto& player = table.players.at(pos);

        std::string attributes;
        if (table.acting_player_pos == pos) { attributes += "*** acting, "; }
        if (table.dealer_pos == pos) { attributes += "dealer, "; }
        if (_user_pos == pos) { attributes += "you, "; }

        oss << std::endl
            << std::setw(text_width.at(0)) << player.player_name
            << std::setw(text_width.at(1)) << attributes
            << std::setw(text_width.at(2));
        if (player.current_stack == 0)
        {
            oss << "all in";
        }
        else
        {
            oss << player.current_stack;
        }

        oss << std::setw(text_width.at(3));
        if (player.per_game_state.has_folded)
        {
            oss << "folded";
        }
        else if (const auto amount_to_call = table.get_player_amount_to_call(pos); amount_to_call > 0)
        {
            oss << ("amount needed to call: " + std::to_string(amount_to_call));
        }
        else if (const auto& actions = player.get_actions(table.current_stage); !actions.empty())
        {
            // To get positioning and width correct
            std::ostringstream oss2;
            std::visit([&oss2](const auto& action){ oss2 << action; }, actions.back());
            oss << oss2.str();
        }
    }

    return oss.str();
}

void holdem_game_orchestrator::execute_showdown()
{
    auto &table = _table_state_manager.get_table_state();
    const auto active_player_count = table.get_active_player_count();

    if (active_player_count > 1)
    {
        // Read in all pocket cards
        for (size_t pos = 0; pos < table.players.size(); ++pos)
        {
            const auto& player = table.players.at(pos);
            if (!player.has_folded() && !player.per_game_state.pocket_cards)
            {
                const auto cards = read_valid_cards([this, pos]() { return _user_interaction.get_player_pocket_cards(pos); }, 2);
                _table_state_manager.set_pocket_cards(pos, cards);
            }
        }
    }

    const auto winners = _poker_lib.get_winner_positions(table);
    const auto split_pots = _table_state_manager.execute_showdown(winners);

    std::ostringstream oss;
    if (active_player_count == 1)
    {
        if (split_pots.size() != 1 || split_pots.front().participant_positions.size() != 1)
        {
            throw std::logic_error("There's only one winner but there are " + std::to_string(split_pots.size()) + " split pots");
        }

        const auto pos = *split_pots.front().participant_positions.begin();
        const auto& winner = table.players.at(pos);
        oss << "Player " << winner.player_name << " at position " << pos << " has won this game. Pot was " << split_pots.front().split_size;
        return;
    }

    oss << "There are multiple winners.\n";
    for (const auto &split : split_pots)
    {
        const auto per_winner_split = split.split_size / split.participant_positions.size();

        oss << "Split pot with pot size " << split.split_size << " is divided up between " << split.participant_positions.size() << " winners.";
        for (const auto &pos : split.participant_positions)
        {
            oss << "Player " << table.players.at(pos).player_name << " at position " << (pos + 1) << " has won " << per_winner_split << " from this pot.";
        }
    }

    _user_interaction.notify_player(oss.str());
}

} // end of namespace poker_lib
