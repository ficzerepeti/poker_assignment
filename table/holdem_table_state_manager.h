#pragma once

#include <memory>
#include <vector>

#include "i_table_state_manager.h"
#include "i_my_poker_lib.h"
#include "table_state.h"
#include "initial_player_state.h"

namespace poker_lib {

class holdem_table_state_manager : public i_table_state_manager
{
public:
    holdem_table_state_manager(const std::vector<initial_player_state> &players,
                               size_t dealer_position,
                               uint64_t small_blind_size,
                               uint64_t big_blind_size);

    ~holdem_table_state_manager() override = default;

    const table_state &get_table_state() const override { return _table_state; }
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

    std::vector<split_pot> execute_showdown(const std::unordered_set<size_t> &winner_positions) override;

    bool start_new_round() override;

private:
    void handle_betting_player_action(const player_action_fold &action);
    void handle_betting_player_action(const player_action_check_or_call &action);
    void handle_betting_player_action(const player_action_raise &action);

    void set_up_table(size_t dealer_position);

    void move_to_next_stage_from_card_deal();

    table_state _table_state;
};

} // end of namespace poker_lib
