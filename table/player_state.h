#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <ostream>

namespace poker_lib {

struct per_game_player_state
{
    bool has_folded = false;
    std::optional<std::string> pocket_cards;
    uint64_t contribution_to_pot = 0;
};

struct per_betting_player_state
{
    bool has_acted_in_betting = false;
};

struct player_state
{
    uint64_t current_stack = 0;

    per_game_player_state per_game_state{};
    per_betting_player_state per_betting_state{};

    bool is_all_in() const { return current_stack == 0 && !per_game_state.has_folded; }
    bool has_folded() const { return per_game_state.has_folded; }
    bool may_act_in_this_betting_round() const { return !per_game_state.has_folded
                                                     && !per_betting_state.has_acted_in_betting
                                                     && current_stack > 0; }

};

std::ostream &operator<<(std::ostream &os, const per_game_player_state &state);
std::ostream &operator<<(std::ostream &os, const per_betting_player_state &state);
std::ostream &operator<<(std::ostream &os, const player_state &state);
bool operator==(const per_game_player_state &lhs, const per_game_player_state &rhs);
bool operator==(const per_betting_player_state &lhs, const per_betting_player_state &rhs);
bool operator==(const player_state &lhs, const player_state &rhs);

} // end of namespace poker_lib
