#pragma once

#include <cstdint>
#include <ostream>

namespace poker_lib {

struct player_action_fold{};
struct player_action_check_or_call{};
struct player_action_raise
{
    uint64_t amount_raised_above_call = 0;
};

std::ostream& operator<<(std::ostream& os, const player_action_fold& obj);
std::ostream& operator<<(std::ostream& os, const player_action_check_or_call& obj);
std::ostream& operator<<(std::ostream& os, const player_action_raise& obj);

} // end of namespace poker_lib
