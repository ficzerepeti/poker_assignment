#include <gtest/gtest.h>
#include "my_poker_lib.h"

TEST(test_my_poker_lib, get_num_of_parsed_cards)
{
    poker_lib::my_poker_lib poker_lib;

    EXPECT_EQ(0, poker_lib.get_num_of_parsed_cards(""));
    EXPECT_EQ(0, poker_lib.get_num_of_parsed_cards("random"));
    EXPECT_EQ(0, poker_lib.get_num_of_parsed_cards("blabla"));
    EXPECT_EQ(0, poker_lib.get_num_of_parsed_cards("blaAsbla"));
    EXPECT_EQ(0, poker_lib.get_num_of_parsed_cards("Ax"));
    EXPECT_EQ(0, poker_lib.get_num_of_parsed_cards("bla As bla"));
    EXPECT_EQ(1, poker_lib.get_num_of_parsed_cards("As"));
    EXPECT_EQ(1, poker_lib.get_num_of_parsed_cards("As As"));
    EXPECT_EQ(2, poker_lib.get_num_of_parsed_cards("As Ad"));
    EXPECT_EQ(5, poker_lib.get_num_of_parsed_cards("2s 3s Ts As Ad"));
    EXPECT_EQ(7, poker_lib.get_num_of_parsed_cards("2s 3c Td Jh Qs Ks As"));
    EXPECT_EQ(7, poker_lib.get_num_of_parsed_cards("2s 3c Td Jh Qs Ks As bl"));
}

TEST(test_my_poker_lib, get_winner_positions)
{
    poker_lib::my_poker_lib poker_lib;

    poker_lib::table_state table;
    auto& players = table.players;
    EXPECT_ANY_THROW(poker_lib.get_winner_positions(table));

    table.communal_cards = "As Ks Qs";

    players.emplace_back(poker_lib::player_state{100, {}, {}, "player1"});
    players.back().per_game_state.contribution_to_pot = 50;
    players.back().per_game_state.pocket_cards = "Js Ts";

    players.emplace_back(poker_lib::player_state{100, {}, {}, "player2"});
    players.back().per_game_state.contribution_to_pot = 50;
    players.back().per_game_state.pocket_cards = "Jd Td";

    EXPECT_NO_THROW(poker_lib.get_winner_positions(table));
}
