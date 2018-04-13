#include "player_state.h"

namespace poker_lib {

bool operator==(const player_state &lhs, const player_state &rhs)
{
    return lhs.amount_needed_to_call == rhs.amount_needed_to_call &&
           lhs.has_called_or_checked_already == rhs.has_called_or_checked_already &&
           lhs.has_folded == rhs.has_folded &&
           lhs.pocket_cards == rhs.pocket_cards;
}

std::ostream& operator<<(std::ostream &os, const player_state &state)
{
    return os << "amount_needed_to_call: " << state.amount_needed_to_call
              << ", has_called_or_checked_already: " << state.has_called_or_checked_already
              << ", has_folded: " << state.has_folded
              << ", pocket_cards: " << state.pocket_cards.value_or("");
}

} // end of namespace poker_lib
