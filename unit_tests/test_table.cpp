#include <gtest/gtest.h>
#include "table/holdem_table_state_manager.h"

TEST(test_holdem_table_state_manager, invalid_initialisation)
{
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager(0, 0, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager(1, 0, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager(2, 2, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager(2, 0, 20, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager(2, 0, 20, 10));
}

TEST(test_holdem_table_state_manager, heads_up_first_player_is_the_dealer)
{
    poker_lib::table_state expected_table_state{};
    expected_table_state.small_blind_size = 10;
    expected_table_state.big_blind_size = 20;
    expected_table_state.pot = 30;
    expected_table_state.acting_player_pos = 0;
    expected_table_state.dealer_pos = 0;
    expected_table_state.small_blind_pos = 0;
    expected_table_state.big_blind_pos = 1;

    expected_table_state.players.emplace_back(poker_lib::player_state{10, false, false, std::optional<std::string>{} });
    expected_table_state.players.emplace_back(poker_lib::player_state{0, false, false, std::optional<std::string>{} });

    poker_lib::holdem_table_state_manager state_manager(2,
                                                        expected_table_state.dealer_pos,
                                                        expected_table_state.small_blind_size,
                                                        expected_table_state.big_blind_size);

    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected_table_state, state_manager.get_table_state());

    expected_table_state.players.at(0).pocket_cards.emplace("As Ks");
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected_table_state.players.at(0).pocket_cards));
    EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected_table_state, state_manager.get_table_state());

    // Pre flop betting round
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));

        auto &acting_player = expected_table_state.players.at(0);
        expected_table_state.pot += acting_player.amount_needed_to_call;
        acting_player.amount_needed_to_call = 0;
        acting_player.has_called_or_checked_already = true;
        expected_table_state.acting_player_pos = 1;

        EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
        EXPECT_EQ(expected_table_state, state_manager.get_table_state());
    }
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
        expected_table_state.players.at(0).has_called_or_checked_already = false;

        EXPECT_EQ(poker_lib::game_stages::deal_communal_cards, state_manager.get_current_game_stage());
        EXPECT_EQ(expected_table_state, state_manager.get_table_state());
    }
}
