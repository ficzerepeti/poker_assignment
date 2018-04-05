#pragma once

#include <memory>
#include <vector>

#include "i_table_state_manager.h"
#include "i_my_poker_lib.h"

namespace poker_lib {

class table_state_manager : public i_table_state_manager
{
public:
    table_state_manager(size_t num_of_players,
                        size_t dealer_position,
                        uint64_t small_blind_size,
                        uint64_t big_blind_size);

    ~table_state_manager() override = default;

    game_stages get_current_game_stage() const override { return _current_stage; }
    const table_state &get_table_state() const override { return _table_state; }

    bool set_pocket_cards(size_t player_pos, const std::string &cards) override;
    bool set_flop(const std::string &cards) override;
    bool set_turn(const std::string &card) override;
    bool set_river(const std::string &card) override;

    bool set_acting_player_action(const player_action_t &action) override;

private:
    void handle_betting_player_action(const player_action_fold &action);
    void handle_betting_player_action(const player_action_check_or_call &action);
    void handle_betting_player_action(const player_action_raise &action);

    game_stages _current_stage;
    table_state _table_state;
};

} // end of namespace poker_lib
