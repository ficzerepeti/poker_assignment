#pragma once

#include <string>
#include <vector>

#include "i_my_poker_lib.h"
#include "table/i_table_state_manager.h"
#include "i_user_interaction.h"

namespace poker_lib {

class holdem_game_orchestrator
{
public:
    holdem_game_orchestrator(i_my_poker_lib& poker_lib,
                             i_user_interaction &user_interaction,
                             i_table_state_manager &table_state_manager,
                             size_t user_pos);

    void run_game();

private:
    player_action_t get_acting_player_action();
    double calculate_user_equity(const table_state& table) const;
    std::string table_state_and_stage_to_user_message() const;

    template <typename FuncT>
    std::string read_valid_cards(FuncT get_cards, const size_t expected_num_of_cards)
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

    i_my_poker_lib& _poker_lib;
    i_user_interaction& _user_interaction;
    i_table_state_manager& _table_state_manager;

    const size_t _user_pos;
};

} // end of namespace poker_lib
