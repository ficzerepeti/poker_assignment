#include <gtest/gtest.h>
#include "table/holdem_table_state_manager.h"

void contribute_to_pot(poker_lib::table_state &expected, const size_t player_pos, const uint64_t amount)
{
    auto& player = expected.players.at(player_pos);

    expected.pot += amount;

    player.current_stack -= amount;
    player.per_game_state.contribution_to_pot += amount;
}

void acting_player_checks_or_calls(poker_lib::table_state &expected)
{
    auto &acting_player = expected.players.at(expected.acting_player_pos);

    const auto amount_to_call = expected.total_contribution_to_stay_in_game - acting_player.per_game_state.contribution_to_pot;
    const auto amount_available = std::min(amount_to_call, acting_player.current_stack);

    contribute_to_pot(expected, expected.acting_player_pos, amount_available);
    acting_player.per_betting_state.has_acted_in_betting = true;
}

void acting_player_raises(poker_lib::table_state &expected, const uint64_t raise_amount)
{
    auto &acting_player = expected.players.at(expected.acting_player_pos);

    const auto amount_to_call = expected.total_contribution_to_stay_in_game - acting_player.per_game_state.contribution_to_pot;
    const auto amount_available = std::min(amount_to_call + raise_amount, acting_player.current_stack);

    if (amount_available > amount_to_call)
    {
        expected.total_contribution_to_stay_in_game += amount_available - amount_to_call;
    }

    contribute_to_pot(expected, expected.acting_player_pos, amount_available);
    acting_player.per_betting_state.has_acted_in_betting = true;
}

TEST(test_holdem_table_state_manager, invalid_initialisations)
{
    // Not enough players
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({}, 0, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{100}}, 0, 10, 20));
    // Invalid dealer pos
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{100}, {100}}, 2, 10, 20));
    // Incorrect blind sizes
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{100}, {100}}, 0, 20, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{100}, {100}}, 0, 20, 10));
    // Not enough player stack
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{9}, {100}}, 0, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{20}, {19}}, 0, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{9}, {19}}, 0, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{100}, {100}, {100}, {19}}, 1, 10, 20));
    EXPECT_ANY_THROW(poker_lib::holdem_table_state_manager({{100}, {100}, {100}, {9}}, 2, 10, 20));
}

TEST(test_holdem_table_state_manager, starting_state_heads_up_1st_player_deals)
{
    poker_lib::table_state expected{};
    expected.small_blind_size = 10;
    expected.big_blind_size = 20;
    expected.pot = 30;
    expected.total_contribution_to_stay_in_game = expected.big_blind_size;
    expected.acting_player_pos = 0;
    expected.dealer_pos = 0;

    expected.players.emplace_back(poker_lib::player_state{90, {false, {}, 10}, {} });
    expected.players.emplace_back(poker_lib::player_state{80, {false, {}, 20}, {} });

    poker_lib::holdem_table_state_manager state_manager({{100}, {100}},
                                                        expected.dealer_pos,
                                                        expected.small_blind_size,
                                                        expected.big_blind_size);

    // Pocket card
    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());
}

TEST(test_holdem_table_state_manager, starting_state_heads_up_2nd_player_deals)
{
    poker_lib::table_state expected{};
    expected.small_blind_size = 10;
    expected.big_blind_size = 20;
    expected.pot = 30;
    expected.total_contribution_to_stay_in_game = expected.big_blind_size;
    expected.acting_player_pos = 1;
    expected.dealer_pos = 1;

    expected.players.emplace_back(poker_lib::player_state{80, {false, {}, 20}, {} });
    expected.players.emplace_back(poker_lib::player_state{90, {false, {}, 10}, {} });

    poker_lib::holdem_table_state_manager state_manager({{100}, {100}},
                                                        expected.dealer_pos,
                                                        expected.small_blind_size,
                                                        expected.big_blind_size);

    // Pocket card
    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());
}

TEST(test_holdem_table_state_manager, starting_state_3_players_1st_player_deals)
{
    poker_lib::table_state expected{};
    expected.small_blind_size = 10;
    expected.big_blind_size = 20;
    expected.pot = 30;
    expected.total_contribution_to_stay_in_game = expected.big_blind_size;
    expected.acting_player_pos = 0;
    expected.dealer_pos = 0;

    expected.players.emplace_back(poker_lib::player_state{100, {}, {} });
    expected.players.emplace_back(poker_lib::player_state{90, {false, {}, 10}, {} });
    expected.players.emplace_back(poker_lib::player_state{80, {false, {}, 20}, {} });

    poker_lib::holdem_table_state_manager state_manager({{100}, {100}, {100}},
                                                        expected.dealer_pos,
                                                        expected.small_blind_size,
                                                        expected.big_blind_size);

    // Pocket card
    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());
}

TEST(test_holdem_table_state_manager, starting_state_3_players_2nd_player_deals)
{
    poker_lib::table_state expected{};
    expected.small_blind_size = 10;
    expected.big_blind_size = 20;
    expected.pot = 30;
    expected.total_contribution_to_stay_in_game = expected.big_blind_size;
    expected.acting_player_pos = 1;
    expected.dealer_pos = 1;

    expected.players.emplace_back(poker_lib::player_state{80, {false, {}, 20}, {} });
    expected.players.emplace_back(poker_lib::player_state{100, {}, {} });
    expected.players.emplace_back(poker_lib::player_state{90, {false, {}, 10}, {} });
    
    poker_lib::holdem_table_state_manager state_manager({{100}, {100}, {100}},
                                                        expected.dealer_pos,
                                                        expected.small_blind_size,
                                                        expected.big_blind_size);

    // Pocket card
    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());
}

TEST(test_holdem_table_state_manager, throws_if_unexpected_method_called)
{
    poker_lib::table_state expected{};
    expected.small_blind_size = 10;
    expected.big_blind_size = 20;
    expected.pot = 30;
    expected.total_contribution_to_stay_in_game = expected.big_blind_size;
    expected.acting_player_pos = 0;
    expected.dealer_pos = 0;

    expected.players.emplace_back(poker_lib::player_state{90, {false, {}, 10}, {} });
    expected.players.emplace_back(poker_lib::player_state{80, {false, {}, 20}, {} });

    poker_lib::holdem_table_state_manager state_manager({{100}, {100}},
                                                        expected.dealer_pos,
                                                        expected.small_blind_size,
                                                        expected.big_blind_size);

    // Dealing pocket cards
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_river("Ts"));
    EXPECT_ANY_THROW(state_manager.set_turn("Ts"));
    EXPECT_ANY_THROW(state_manager.set_acting_player_action(poker_lib::player_action_fold{}));
    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Move to pre-flop betting
    expected.players.at(0).per_game_state.pocket_cards.emplace("Ac Kd");
    expected.players.at(1).per_game_state.pocket_cards.emplace("As Ks");
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));

    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_turn("8s"));
    EXPECT_ANY_THROW(state_manager.set_river("9s"));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());

    // Move to flop
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    contribute_to_pot(expected, 0, expected.small_blind_size);
    expected.acting_player_pos = 1;
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::deal_communal_cards, state_manager.get_current_game_stage());

    // Flop
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));
    EXPECT_ANY_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_ANY_THROW(state_manager.set_turn("8s"));
    EXPECT_ANY_THROW(state_manager.set_river("9s"));

    // Move to flop betting
    expected.communal_cards = "Ts 9d 3c";
    EXPECT_NO_THROW(state_manager.set_flop(expected.communal_cards));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());

    // Flop betting
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_turn("8s"));
    EXPECT_ANY_THROW(state_manager.set_river("9s"));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());

    // Move to turn
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::deal_turn_card, state_manager.get_current_game_stage());

    // Turn
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));
    EXPECT_ANY_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_river("9s"));

    // Move to turn betting
    expected.communal_cards += " 9s";
    EXPECT_NO_THROW(state_manager.set_turn("9s"));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::turn_betting_round, state_manager.get_current_game_stage());

    // Turn betting
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_turn("8s"));
    EXPECT_ANY_THROW(state_manager.set_river("9s"));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::turn_betting_round, state_manager.get_current_game_stage());

    // Move to river
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::deal_river_card, state_manager.get_current_game_stage());

    // River
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));
    EXPECT_ANY_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_turn("8s"));

    // Move to river betting
    expected.communal_cards += " 8s";
    EXPECT_NO_THROW(state_manager.set_river("8s"));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::river_betting_round, state_manager.get_current_game_stage());

    // River betting
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_turn("8s"));
    EXPECT_ANY_THROW(state_manager.set_river("9s"));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::river_betting_round, state_manager.get_current_game_stage());

    // Move to showdown
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::end_of_game, state_manager.get_current_game_stage());

    // Showdown
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_NO_THROW(state_manager.set_pocket_cards(1, *expected.players.at(1).per_game_state.pocket_cards));
    EXPECT_ANY_THROW(state_manager.set_flop("Ts 9d 3c"));
    EXPECT_ANY_THROW(state_manager.set_turn("8s"));
    EXPECT_ANY_THROW(state_manager.set_river("9s"));
    EXPECT_ANY_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(expected, state_manager.get_table_state());
    EXPECT_EQ(poker_lib::game_stages::end_of_game, state_manager.get_current_game_stage());
}

TEST(test_holdem_table_state_manager, full_game_4_players)
{
    poker_lib::table_state expected{};
    expected.small_blind_size = 10;
    expected.big_blind_size = 20;
    expected.pot = 30;
    expected.total_contribution_to_stay_in_game = expected.big_blind_size;
    expected.acting_player_pos = 1;
    expected.dealer_pos = 2;

    expected.players.emplace_back(poker_lib::player_state{4980, {false, {}, 20}, {} });
    expected.players.emplace_back(poker_lib::player_state{5000, {}, {} });
    expected.players.emplace_back(poker_lib::player_state{5000, {}, {} });
    expected.players.emplace_back(poker_lib::player_state{4990, {false, {}, 10}, {} });

    poker_lib::holdem_table_state_manager state_manager(std::vector<poker_lib::initial_player_state>(4, {5000}),
                                                        expected.dealer_pos,
                                                        expected.small_blind_size,
                                                        expected.big_blind_size);

    // Pocket card
    EXPECT_EQ(poker_lib::game_stages::deal_pocket_cards, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    expected.players.at(0).per_game_state.pocket_cards.emplace("As Ks");
    EXPECT_NO_THROW(state_manager.set_pocket_cards(0, *expected.players.at(0).per_game_state.pocket_cards));
    EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Pre flop betting round
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
        EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
        acting_player_checks_or_calls(expected);
        expected.acting_player_pos = 2;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_fold{}));
        EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
        expected.players.at(expected.acting_player_pos).per_game_state.has_folded = true;
        expected.acting_player_pos = 3;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
        EXPECT_EQ(poker_lib::game_stages::pre_flop_betting_round, state_manager.get_current_game_stage());
        acting_player_checks_or_calls(expected);
        expected.acting_player_pos = 0;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
        EXPECT_EQ(poker_lib::game_stages::deal_communal_cards, state_manager.get_current_game_stage());
        acting_player_checks_or_calls(expected);
        for (auto& player : expected.players)
        {
            player.per_betting_state.has_acted_in_betting = {};
        }
        expected.acting_player_pos = 3;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }

    // Flop
    expected.communal_cards = "Ts 9d 3c";
    EXPECT_NO_THROW(state_manager.set_flop(expected.communal_cards));
    EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Flop betting round
    {
        const auto action = poker_lib::player_action_raise{200};
        EXPECT_NO_THROW(state_manager.set_acting_player_action(action));
        EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());
        acting_player_raises(expected, action.amount_raised_above_call);
        expected.acting_player_pos = 0;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
        EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());
        acting_player_checks_or_calls(expected);
        expected.acting_player_pos = 1;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }
    // Re-raise
    {
        const auto action = poker_lib::player_action_raise{400};
        EXPECT_NO_THROW(state_manager.set_acting_player_action(action));
        EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());

        acting_player_raises(expected, action.amount_raised_above_call);
        expected.players.at(0).per_betting_state.has_acted_in_betting = false;
        expected.players.at(1).per_betting_state.has_acted_in_betting = true;
        expected.players.at(3).per_betting_state.has_acted_in_betting = false;
        expected.acting_player_pos = 3;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
        EXPECT_EQ(poker_lib::game_stages::flop_betting_round, state_manager.get_current_game_stage());

        acting_player_checks_or_calls(expected);
        expected.acting_player_pos = 0;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }
    {
        EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_fold{}));
        EXPECT_EQ(poker_lib::game_stages::deal_turn_card, state_manager.get_current_game_stage());

        auto &acting_player = expected.players.at(expected.acting_player_pos);
        acting_player.per_game_state.has_folded = true;
        for (auto& player : expected.players)
        {
            player.per_betting_state.has_acted_in_betting = {};
        }
        expected.acting_player_pos = 3;
        EXPECT_EQ(expected, state_manager.get_table_state());
    }

    // Deal turn card. Only pos 1 and 3 are active
    expected.communal_cards += " 5c";
    EXPECT_NO_THROW(state_manager.set_turn("5c"));
    EXPECT_EQ(poker_lib::game_stages::turn_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Turn betting round
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::turn_betting_round, state_manager.get_current_game_stage());
    acting_player_checks_or_calls(expected);
    expected.acting_player_pos = 1;
    EXPECT_EQ(expected, state_manager.get_table_state());

    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::deal_river_card, state_manager.get_current_game_stage());
    acting_player_checks_or_calls(expected);
    expected.acting_player_pos = 3;
    for (auto& player : expected.players)
    {
        player.per_betting_state.has_acted_in_betting = {};
    }
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Deal river card
    expected.communal_cards += " 5h";
    EXPECT_NO_THROW(state_manager.set_river("5h"));
    EXPECT_EQ(poker_lib::game_stages::river_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    // Turn betting round
    acting_player_checks_or_calls(expected);
    expected.acting_player_pos = 1;
    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::river_betting_round, state_manager.get_current_game_stage());
    EXPECT_EQ(expected, state_manager.get_table_state());

    EXPECT_NO_THROW(state_manager.set_acting_player_action(poker_lib::player_action_check_or_call{}));
    EXPECT_EQ(poker_lib::game_stages::end_of_game, state_manager.get_current_game_stage());
    acting_player_checks_or_calls(expected);
    expected.acting_player_pos = 3;
    for (auto& player : expected.players)
    {
        player.per_betting_state.has_acted_in_betting = {};
    }
    EXPECT_EQ(expected, state_manager.get_table_state());
}
