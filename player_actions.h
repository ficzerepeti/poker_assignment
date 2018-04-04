#pragma once

#include <cstdint>
#include <ostream>
#include <variant>

namespace poker_lib {

struct player_action_fold{};
struct player_action_check_or_call{};
struct player_action_raise
{
    uint64_t amount_raised_above_call = 0;
};

using player_action_t = std::variant<player_action_fold, player_action_check_or_call, player_action_raise>;

std::ostream& operator<<(std::ostream& os, const player_action_fold& obj);
std::ostream& operator<<(std::ostream& os, const player_action_check_or_call& obj);
std::ostream& operator<<(std::ostream& os, const player_action_raise& obj);

} // end of namespace poker_lib
