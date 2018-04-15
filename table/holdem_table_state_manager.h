#pragma once

#include <memory>
#include <vector>

#include "i_table_state_manager.h"
#include "i_my_poker_lib.h"
#include "table_state_manager.h"

namespace poker_lib {

struct initial_player_state
{
    uint64_t current_stack = 0;
};

class holdem_table_state_manager : public i_table_state_manager
{
public:
    holdem_table_state_manager(const std::vector<initial_player_state> &players,
                               size_t dealer_position,
                               uint64_t small_blind_size,
                               uint64_t big_blind_size);

    ~holdem_table_state_manager() override = default;

    game_stages get_current_game_stage() const override { return _current_stage; }
    const table_state &get_table_state() const override { return _table_state.get_table_state(); }
    const player_state &get_acting_player_state() const override { return _table_state.get_acting_player(); }

    // Never throws, can set pocket cards at any stage of the game
    void set_pocket_cards(size_t player_pos, const std::string &cards) override;

    // Throws if current stage is not deal_communal_cards
    void set_flop(const std::string &cards) override;
    // Throws if current stage is not deal_turn_card
    void set_turn(const std::string &card) override;
    // Throws if current stage is not deal_river_card
    void set_river(const std::string &card) override;

    // Throws if current stage is not any of the betting rounds
    void set_acting_player_action(const player_action_t &action) override;

private:
    void handle_betting_player_action(const player_action_fold &action);
    void handle_betting_player_action(const player_action_check_or_call &action);
    void handle_betting_player_action(const player_action_raise &action);

    game_stages _current_stage;
    table_state_manager _table_state;
};

} // end of namespace poker_lib
