#include <algorithm>
#include "table_state.h"

static size_t get_next_pos(const size_t current_pos, const size_t num_of_players)
{
    return (current_pos + 1) % num_of_players;
}

namespace poker_lib {

size_t table_state::get_active_player_count() const
{
    auto may_act = [](const player_state& player){ return !player.has_folded; };
    return static_cast<size_t>(std::count_if(players.begin(), players.end(), may_act));
}

bool table_state::move_to_next_betting_player()
{
    const auto pos_has_just_acted = acting_player_pos;
    for (size_t pos = get_next_pos(pos_has_just_acted, players.size());
         pos != pos_has_just_acted;
         pos = get_next_pos(pos, players.size()))
    {
        if (players.at(pos).may_act_in_this_betting_round())
        {
            acting_player_pos = pos;
            return true;
        }
    }

    for (auto &player : players)
    {
        player.has_called_or_checked_already = false;
        player.amount_needed_to_call = 0;
    }

    return false;
}

} // end of namespace poker_lib
