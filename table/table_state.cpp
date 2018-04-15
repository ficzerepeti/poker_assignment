#include <algorithm>
#include "table_state.h"

namespace std {

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& container)
{
    for (size_t pos = 0; pos < container.size(); ++pos)
    {
        const auto& elem = container[pos];
        os << "pos: " << pos << ", elem: " << elem << ",, ";
    }
    return os;
}

} // end of namespace std

namespace poker_lib {

bool operator==(const table_state &lhs, const table_state &rhs)
{
    return lhs.small_blind_size == rhs.small_blind_size &&
           lhs.big_blind_size == rhs.big_blind_size &&
           lhs.pot == rhs.pot &&
           lhs.communal_cards == rhs.communal_cards &&
           lhs.players == rhs.players &&
           lhs.acting_player_pos == rhs.acting_player_pos &&
           lhs.dealer_pos == rhs.dealer_pos;
}

std::ostream& operator<<(std::ostream &os, const table_state &state)
{
    return os << "small_blind_size: " << state.small_blind_size
              << ", big_blind_size: " << state.big_blind_size
              << ", pot: " << state.pot
              << ", communal_cards: " << state.communal_cards
              << ", players: " << state.players
              << ", acting_player_pos: " << state.acting_player_pos
              << ", dealer_pos: " << state.dealer_pos;
}

size_t get_next_pos(const size_t current_pos, const size_t num_of_players)
{
    return (current_pos + 1) % num_of_players;
}

} // end of namespace poker_lib
