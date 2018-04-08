#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <type_traits>

#include "holdem_game_orchestrator.h"

template<class T> struct always_false : std::false_type {};

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
        bool success = false;
        const auto current_stage = _table_state_manager.get_current_game_stage();
        switch (current_stage)
        {
        case game_stages::deal_pocket_cards:
            success = _table_state_manager.set_pocket_cards(_user_pos, _user_interaction.get_pocket_cards());
            break;

        case game_stages::deal_communal_cards:
            success = _table_state_manager.set_flop(_user_interaction.get_flop());
            break;

        case game_stages::deal_turn_card:
            success = _table_state_manager.set_turn(_user_interaction.get_turn());
            break;

        case game_stages::deal_river_card:
            success = _table_state_manager.set_river(_user_interaction.get_river());
            break;

        case game_stages::pre_flop_betting_round:
        case game_stages::flop_betting_round:
        case game_stages::turn_betting_round:
        case game_stages::river_betting_round:
            success = _table_state_manager.set_acting_player_action(get_acting_player_action());
            break;

        case game_stages::showdown:
            return;
        }

        if (!success)
        {
            std::ostringstream oss;
            oss << "Failed to handle game stage " << current_stage;
            throw std::logic_error(oss.str());
        }
    }
}

player_action_t holdem_game_orchestrator::get_acting_player_action()
{
    const auto& table = _table_state_manager.get_table_state();
    const auto& player = get_current_player(table);

    const bool is_opponent = table.acting_player_pos != _user_pos;
    if (is_opponent)
    {
        return _user_interaction.get_opponent_action(table.acting_player_pos);
    }

    std::ostringstream oss;
    if (player.amount_needed_to_call != 0)
    {
        oss << "It's your turn and you need to invest " << player.amount_needed_to_call << " to call. ";
    }

    const auto user_equity = calculate_user_equity(table);

    oss << "Your equity of winning is " << (user_equity * 100) << '%' << std::endl;
    _user_interaction.notify_player(oss.str());

    const auto recommended_action = player_action_check_or_call{}; // TODO
    return _user_interaction.get_user_action(table.acting_player_pos, recommended_action);
}

double holdem_game_orchestrator::calculate_user_equity(const table_state& table) const
{
    std::vector<std::string> hands;
    for (const auto& player : table.players)
    {
        hands.emplace_back(player.pocket_cards.value_or("random"));
    }

    return _poker_lib.calculate_equities(hands, table.communal_cards).at(table.acting_player_pos);
}

} // end of namespace poker_lib
