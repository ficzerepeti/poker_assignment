#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <ostream>

namespace poker_lib {

struct player_state
{
    uint64_t amount_needed_to_call = 0;
    bool has_called_or_checked_already = false;
    bool has_folded = false;

    std::optional<std::string> pocket_cards;

    bool may_act_in_this_betting_round() const { return !has_folded && !has_called_or_checked_already; }
};

bool operator==(const player_state &lhs, const player_state &rhs);
std::ostream& operator<<(std::ostream &os, const player_state &state);

} // end of namespace poker_lib
