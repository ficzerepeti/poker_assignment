#pragma once

#include <memory>
#include <vector>

#include "i_game_engine.h"
#include "i_my_poker_lib.h"
#include "i_analysis_engine_factory.h"

namespace poker_lib {

class game_engine : public i_game_engine
{
public:
    game_engine(i_analysis_engine_factory& analysis_engine_factory,
                size_t num_of_players,
                size_t user_position);

    ~game_engine() override = default;

    operation_status set_pocket_cards(const std::string &cards) override;
    operation_status set_flop(const std::string &cards) override;
    operation_status set_turn(const std::string &card) override;
    operation_status set_river(const std::string &card) override;
    operation_status set_user_action(const player_action_t &action) override;
    operation_status set_opponent_action(size_t position, const player_action_t &action) override;

    user_analysis make_user_analysis() override;

private:
    struct player_state
    {
        std::vector<player_action_t> actions_taken;
        uint64_t investment_in_pot = 0;
        bool is_our_user = false;
        bool is_all_in = false;

        bool has_folded() const
        {
            return !actions_taken.empty() && std::holds_alternative<player_action_fold>(actions_taken.back());
        }
    };

    i_analysis_engine_factory& _analysis_engine_factory;
    std::vector<player_state> _players;
    size_t _user_position;

    std::unique_ptr<i_analysis_engine> _analysis_engine;

    std::string _pocket_cards;
    std::string _flop;
    std::string _turn;
    std::string _river;
};

} // end of namespace poker_lib
