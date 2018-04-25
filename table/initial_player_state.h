#pragma once

#include <string>

namespace poker_lib {

struct initial_player_state
{
    size_t current_stack = 0;
    std::string player_name;
};

} // end of namespace poker_lib
