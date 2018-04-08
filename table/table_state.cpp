#include <algorithm>
#include "table_state.h"

static size_t get_next_pos(const size_t current_pos, const size_t num_of_players)
{
    return (current_pos + 1) % num_of_players;
}

namespace poker_lib {

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

void clear_per_betting_round_state_and_elect_next_acting_player(table_state& table)
{
    // Reset current betting related state
    for (auto &player : table.players)
    {
        player.has_called_or_checked_already = false;
        player.amount_needed_to_call = 0;
    }

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

} // end of namespace poker_lib
