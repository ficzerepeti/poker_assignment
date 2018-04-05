#pragma once

#include <cstdint>
#include <string>

#include "i_analysis_engine.h"
#include "player_actions.h"

namespace poker_lib {

enum class operation_status
{
    success,
    invalid_format,
    unexpected_action,
};

class i_game_engine
{
public:
    virtual ~i_game_engine() = default;

    virtual operation_status set_pocket_cards(const std::string& cards) = 0;
    virtual operation_status set_flop(const std::string& card) = 0;
    virtual operation_status set_turn(const std::string& card) = 0;
    virtual operation_status set_river(const std::string& card) = 0;

    virtual operation_status set_user_action(const player_action_t& action) = 0;
    virtual operation_status set_opponent_action(size_t position, const player_action_t& action) = 0;

    virtual user_analysis make_user_analysis() = 0;
};

} // end of namespace poker_lib
