#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>
#include <type_traits>

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
        else if (_poker_lib.get_num_of_parsed_cards(cards) != (board_cards_count + expected_num_of_cards))
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
            _table_state_manager.set_pocket_cards(_user_pos, read_valid_cards([this](){ return _user_interaction.get_pocket_cards(); }, 2));
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

        case game_stages::end_of_game:
            _user_interaction.notify_player("End of game has been reached."); // TODO: print out winner
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

    constexpr std::array<int, 3> text_width{25, 20, 40};
    oss << std::endl << std::setw(text_width.at(0)) << " "
                     << std::setw(text_width.at(1)) << "stack size"
                     << std::setw(text_width.at(2)) << "action";
    for (size_t pos = 0; pos < table.players.size(); ++pos)
    {
        const auto& player = table.players.at(pos);

        std::string attributes(player.player_name);
        if (!attributes.empty())
        {
            attributes = ' ';
        }
        if (_user_pos == pos) { attributes += "* you, "; }
        if (table.acting_player_pos == pos) { attributes += "*** acting, "; }
        if (table.dealer_pos == pos) { attributes += "dealer, "; }
        if (player.per_game_state.has_folded) { attributes += "folded"; }

        oss << std::endl
            << std::setw(text_width.at(0)) << attributes
            << std::setw(text_width.at(1));
        if (player.current_stack == 0)
        {
            oss << "all in";
        }
        else
        {
            oss << player.current_stack;
        }

        oss << std::setw(text_width.at(2));
        if (const auto amount_to_call = table.get_player_amount_to_call(pos); amount_to_call > 0)
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

} // end of namespace poker_lib
