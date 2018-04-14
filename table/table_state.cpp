#include <algorithm>
#include "table_state.h"

namespace std {

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& container)
{
    for (const T &elem : container)
    {
        os << elem << ",, ";
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
           lhs.dealer_pos == rhs.dealer_pos &&
           lhs.small_blind_pos == rhs.small_blind_pos &&
           lhs.big_blind_pos == rhs.big_blind_pos;
}

std::ostream& operator<<(std::ostream &os, const table_state &state)
{
    return os << "small_blind_size: " << state.small_blind_size
              << ", big_blind_size: " << state.big_blind_size
              << ", pot: " << state.pot
              << ", communal_cards: " << state.communal_cards
              << ", players: " << state.players
              << ", acting_player_pos: " << state.acting_player_pos
              << ", dealer_pos: " << state.dealer_pos
              << ", small_blind_pos: " << state.small_blind_pos
              << ", big_blind_pos: " << state.big_blind_pos;
}

const player_state &get_current_player(const table_state &table)
{
    return table.players.at(table.acting_player_pos);
}

size_t get_active_player_count(const table_state& table)
{
    auto may_act = [](const player_state& player){ return !player.has_folded; };
    return static_cast<size_t>(std::count_if(table.players.begin(), table.players.end(), may_act));
}

bool move_to_next_betting_player(table_state& table)
{
    const auto pos_has_just_acted = table.acting_player_pos;
    for (size_t pos = get_next_pos(pos_has_just_acted, table.players.size());
         pos != pos_has_just_acted;
         pos = get_next_pos(pos, table.players.size()))
    {
        if (table.players.at(pos).may_act_in_this_betting_round())
        {
            table.acting_player_pos = pos;
            return true;
        }
    }

    return false;
}

void elect_next_acting_player_after_betting(table_state &table)
{
    // Find first active player after dealer
    for (auto pos = get_next_pos(table.dealer_pos, table.players.size());
         pos != table.dealer_pos;
         pos = get_next_pos(pos, table.players.size()))
    {
        auto& player = table.players.at(pos);
        if (!player.has_folded)
        {
            table.acting_player_pos = pos;
            return;
        }
    }
}

size_t get_next_pos(const size_t current_pos, const size_t num_of_players)
{
    return (current_pos + 1) % num_of_players;
}

void reset_players_after_betting(table_state& table)
{
    for (auto &player : table.players)
    {
        player.amount_needed_to_call = 0;
        player.has_acted_in_betting = false;
    }
}

} // end of namespace poker_lib
