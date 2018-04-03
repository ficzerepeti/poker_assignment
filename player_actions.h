#pragma once

#include <cstdint>
#include <ostream>

namespace poker_lib {

struct player_action_fold{};
struct player_action_check{};
struct player_action_call
{
    uint64_t amount = 0;
};
struct player_action_raise
{
    uint64_t amount = 0;
};

std::ostream& operator<<(std::ostream& os, const player_action_fold& obj);
std::ostream& operator<<(std::ostream& os, const player_action_check& obj);
std::ostream& operator<<(std::ostream& os, const player_action_call& obj);
std::ostream& operator<<(std::ostream& os, const player_action_raise& obj);

} // end of namespace poker_lib
