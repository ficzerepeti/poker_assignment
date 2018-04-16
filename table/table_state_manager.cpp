#include <algorithm>
#include "table_state_manager.h"

namespace poker_lib {

player_state &table_state_manager::get_acting_player()
{
    return _table_state.players.at(_table_state.acting_player_pos);
}

const player_state &table_state_manager::get_acting_player() const
{
    return _table_state.players.at(_table_state.acting_player_pos);
}

player_state &table_state_manager::get_player_at_pos(size_t player_pos)
{
    return _table_state.players.at(player_pos);
}

void table_state_manager::set_communal_cards(const std::string &cards)
{
    _table_state.communal_cards = cards;
}

size_t table_state_manager::get_active_player_count() const
{
    auto may_act = [](const player_state& player){ return !player.has_folded(); };
    return static_cast<size_t>(std::count_if(_table_state.players.begin(), _table_state.players.end(), may_act));
}

bool table_state_manager::move_to_next_betting_player()
{
    const auto pos_has_just_acted = _table_state.acting_player_pos;
    for (size_t pos = get_next_pos(pos_has_just_acted, _table_state.players.size());
         pos != pos_has_just_acted;
         pos = get_next_pos(pos, _table_state.players.size()))
    {
        if (_table_state.players.at(pos).may_act_in_this_betting_round())
        {
            _table_state.acting_player_pos = pos;
            return true;
        }
    }

    return false;
}

void table_state_manager::elect_next_acting_player_after_betting()
{
    // Find first active player after dealer
    for (auto pos = get_next_pos(_table_state.dealer_pos, _table_state.players.size());
         pos != _table_state.dealer_pos;
         pos = get_next_pos(pos, _table_state.players.size()))
    {
        auto& player = _table_state.players.at(pos);
        if (!player.has_folded())
        {
            _table_state.acting_player_pos = pos;
            return;
        }
    }
}

void table_state_manager::reset_per_betting_state()
{
    for (auto &player : _table_state.players)
    {
        player.per_betting_state = {};
    }
}

} // end of namespace poker_lib
