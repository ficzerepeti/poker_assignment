#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <ostream>
#include <vector>

#include "game_stages.h"
#include "player_actions.h"

namespace poker_lib {

struct per_game_player_state
{
    bool has_folded = false;
    std::optional<std::string> pocket_cards;
    uint64_t contribution_to_pot = 0;
};

struct per_betting_player_state
{
    std::vector<player_action_t> pre_flop_bets;
    std::vector<player_action_t> flop_bets;
    std::vector<player_action_t> turn_bets;
    std::vector<player_action_t> river_bets;
};

struct player_state
{
    uint64_t current_stack = 0;

    per_game_player_state per_game_state{};
    per_betting_player_state per_betting_state{};

    bool is_all_in() const { return current_stack == 0 && !per_game_state.has_folded; }
    bool has_folded() const { return per_game_state.has_folded; }
    std::vector<player_action_t>& get_actions(game_stages stage);
    const std::vector<player_action_t>& get_actions(game_stages stage) const;
};

std::ostream &operator<<(std::ostream &os, const per_game_player_state &state);
std::ostream &operator<<(std::ostream &os, const per_betting_player_state &state);
std::ostream &operator<<(std::ostream &os, const player_state &state);
bool operator==(const per_game_player_state &lhs, const per_game_player_state &rhs);
bool operator==(const per_betting_player_state &lhs, const per_betting_player_state &rhs);
bool operator==(const player_state &lhs, const player_state &rhs);

} // end of namespace poker_lib
