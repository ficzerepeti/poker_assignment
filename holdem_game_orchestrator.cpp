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

void holdem_game_orchestrator::run_game()
{
    while (true)
    {
        switch (_table_state_manager.get_table_state().current_stage)
        {
        case game_stages::deal_pocket_cards:
            _table_state_manager.set_pocket_cards(_user_pos, _user_interaction.get_pocket_cards());
            break;

        case game_stages::deal_communal_cards:
            _table_state_manager.set_flop(_user_interaction.get_flop());
            break;

        case game_stages::deal_turn_card:
            _table_state_manager.set_turn(_user_interaction.get_turn());
            break;

        case game_stages::deal_river_card:
            _table_state_manager.set_river(_user_interaction.get_river());
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

        _user_interaction.notify_player("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
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

    const auto user_equity = calculate_user_equity(table);

    std::ostringstream oss;
    oss << "Your equity of winning is " << (user_equity * 100) << '%' << std::endl;
    _user_interaction.notify_player(oss.str());

    const auto recommended_action = player_action_check_or_call{}; // TODO
    return _user_interaction.get_user_action(recommended_action);
}

double holdem_game_orchestrator::calculate_user_equity(const table_state& table) const
{
    std::vector<std::string> hands;
    for (const auto& player : table.players)
    {
        hands.emplace_back(player.per_game_state.pocket_cards.value_or("random"));
    }

    return _poker_lib.calculate_equities(hands, table.communal_cards).at(table.acting_player_pos);
}

std::string holdem_game_orchestrator::table_state_and_stage_to_user_message() const
{
    const auto& table = _table_state_manager.get_table_state();

    std::ostringstream oss;
    oss << table.current_stage << ": pot size: " << table.pot;
    if (!table.communal_cards.empty())
    {
        oss << ", communal cards: " << table.communal_cards;
    }
    oss << ", big blind size: " << table.big_blind_size;

    constexpr std::array<int, 3> text_width{25, 10, 10};
    oss << std::endl << std::setw(text_width.at(0)) << " "
                     << std::setw(text_width.at(1)) << "stack size"
                     << std::setw(text_width.at(2)) << "action";
    for (size_t pos = 0; pos < table.players.size(); ++pos)
    {
        oss << std::endl;

        const auto& player = table.players.at(pos);

        std::string attributes;
        if (table.acting_player_pos == pos) { attributes += "acting, "; }
        if (table.dealer_pos == pos) { attributes += "dealer, "; }
        if (_user_pos == pos) { attributes += "you"; }
        if (player.per_game_state.has_folded) { attributes = "folded"; }

        oss << std::setw(text_width.at(0)) << attributes;
        oss << std::setw(text_width.at(1));
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
            oss << "amount needed to call:" << amount_to_call;
        }
        else if (const auto& actions = player.get_actions(table.current_stage); !actions.empty())
        {
            std::visit([&oss](const auto& action){ oss << action; }, actions.back());
        }
    }

    oss << "\namount needed to call: " << table.get_acting_player_amount_to_call()
        << "\nacting player state: " << table.get_acting_player();

    return oss.str();
}

} // end of namespace poker_lib
