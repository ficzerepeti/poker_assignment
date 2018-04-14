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
    poker_lib::table_state expected{};
    expected.small_blind_size = 10;
    expected.big_blind_size = 20;
    expected.pot = 30;
    expected.acting_player_pos = 0;
    expected.dealer_pos = 0;
    expected.small_blind_pos = 0;
    expected.big_blind_pos = 1;

    expected.players.emplace_back(poker_lib::player_state{10, false, false, std::optional<std::string>{} });
    expected.players.emplace_back(poker_lib::player_state{0, false, false, std::optional<std::string>{} });

    poker_lib::holdem_table_state_manager state_manager(2,
                                                        expected.dealer_pos,
                                                        expected.small_blind_size,
                                                        expected.big_blind_size);

    // Pocket card
    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    expected.players.at(0).pocket_cards.emplace("As Ks");
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).pocket_cards));
    EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Pre flop betting round
    {
        auto &acting_player = expected.players.at(0);
        expected.pot += acting_player.amount_needed_to_call;
        acting_player.amount_needed_to_call = 0;
        acting_player.has_acted_in_betting = true;
        expected.acting_player_pos = 1;
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
        EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
        EXPECT_EQ(expected, state_manager.get_table_state());
    }

    expected.players.at(0).has_acted_in_betting = false;
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::deal_communal_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Flop
    expected.communal_cards = "Ts 9d 3c";
    EXPECT_NO_THROW(state_manager.set_flop(expected.communal_cards));
    EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Flop betting round
    {
        const auto action = poker_lib::player_action_raise{200};
        expected.pot += action.amount_raised_above_call;
        expected.players.at(1).has_acted_in_betting = true;
        expected.players.at(0).amount_needed_to_call = action.amount_raised_above_call;
        expected.acting_player_pos = 0;
        EXPECT_NO_THROW(state_manager.set_acting_player_action(action));
        EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());
        EXPECT_EQ(expected, state_manager.get_table_state());
    }

    expected.pot += expected.players.at(0).amount_needed_to_call;
    expected.players.at(0).amount_needed_to_call = 0;
    expected.players.at(0).has_acted_in_betting = false;
    expected.players.at(1).has_acted_in_betting = false;
    expected.acting_player_pos = 1;
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::deal_turn_card, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Deal turn card
    expected.communal_cards += " 5c";
    EXPECT_NO_THROW(state_manager.set_turn("5c"));
    EXPECT_EQ(poker_lib::game_stages::turn_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Turn betting round
    expected.players.at(1).has_acted_in_betting = true;
    expected.acting_player_pos = 0;
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::turn_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    expected.players.at(0).has_acted_in_betting = false;
    expected.players.at(1).has_acted_in_betting = false;
    expected.acting_player_pos = 1;
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::deal_river_card, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Deal river card
    expected.communal_cards += " 5h";
    EXPECT_NO_THROW(state_manager.set_river("5h"));
    EXPECT_EQ(poker_lib::game_stages::river_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Turn betting round
    expected.players.at(1).has_acted_in_betting = true;
    expected.acting_player_pos = 0;
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::river_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    expected.players.at(0).has_acted_in_betting = false;
    expected.players.at(1).has_acted_in_betting = false;
    expected.acting_player_pos = 1;
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::end_of_game, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());
}
