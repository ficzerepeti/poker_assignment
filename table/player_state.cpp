#include "player_state.h"

namespace poker_lib {

std::ostream& operator<<(std::ostream &os, const per_game_player_state &state)
{
    os << "has_folded: " << state.has_folded
       << ", pocket_cards: " << state.pocket_cards.value_or("")
       << ", contribution_to_pot: " << state.contribution_to_pot
       << '.';
    return os;
}

std::ostream& operator<<(std::ostream &os, const per_betting_player_state &state)
{
    os << "amount_needed_to_call: " << state.amount_needed_to_call
       << ", has_acted_in_betting: " << state.has_acted_in_betting
       << '.';
    return os;
}

std::ostream& operator<<(std::ostream &os, const player_state &state)
{
    os << "current_stack: " << state.current_stack
       << ", per_game_state: " << state.per_game_state
       << ", per_betting_state: " << state.per_betting_state
       << '.';
    return os;
}

bool operator==(const per_game_player_state &lhs, const per_game_player_state &rhs)
{
    return lhs.has_folded == rhs.has_folded &&
           lhs.pocket_cards == rhs.pocket_cards &&
           lhs.contribution_to_pot == rhs.contribution_to_pot;
}

bool operator==(const per_betting_player_state &lhs, const per_betting_player_state &rhs)
{
    return lhs.amount_needed_to_call == rhs.amount_needed_to_call &&
           lhs.has_acted_in_betting == rhs.has_acted_in_betting;
}

bool operator==(const player_state &lhs, const player_state &rhs)
{
    return lhs.current_stack == rhs.current_stack &&
           lhs.per_game_state == rhs.per_game_state &&
           lhs.per_betting_state == rhs.per_betting_state;
}

} // end of namespace poker_lib
