#include <algorithm>
#include "table_state.h"
#include "game_stages.h"

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

player_state &table_state::get_acting_player()
{
    return players.at(acting_player_pos);
}

const player_state &table_state::get_acting_player() const
{
    return players.at(acting_player_pos);
}

size_t table_state::get_active_player_count() const
{
    auto may_act = [](const player_state& player){ return !player.has_folded(); };
    return static_cast<size_t>(std::count_if(players.begin(), players.end(), may_act));
}

bool table_state::move_to_next_betting_player()
{
    const auto pos_has_just_acted = acting_player_pos;
    for (size_t pos = get_next_pos(pos_has_just_acted, players.size());
         pos != pos_has_just_acted;
         pos = get_next_pos(pos, players.size()))
    {
        if (may_act_in_betting_round(pos))
        {
            acting_player_pos = pos;
            return true;
        }
    }

    return false;
}

void table_state::elect_next_acting_player_after_betting()
{
    // Find first active player after dealer
    for (auto pos = get_next_pos(dealer_pos, players.size());
         pos != dealer_pos;
         pos = get_next_pos(pos, players.size()))
    {
        auto& player = players.at(pos);
        if (!player.has_folded())
        {
            acting_player_pos = pos;
            return;
        }
    }
}

uint64_t table_state::get_player_amount_to_call(size_t player_pos) const
{
    const auto &player = players.at(player_pos);
    const auto amount_to_call = total_contribution_to_stay_in_game - player.per_game_state.contribution_to_pot;

    return amount_to_call;
}

bool table_state::may_act_in_betting_round(size_t player_pos) const
{
    const auto& player = players.at(player_pos);
    if (player.has_folded() || player.is_all_in())
    {
        return false;
    }

    const bool has_taken_no_action = player.get_actions(current_stage).empty();
    const bool need_to_contribute_to_stay_in = get_player_amount_to_call(player_pos) > 0;
    return has_taken_no_action || need_to_contribute_to_stay_in;
}

bool operator==(const table_state &lhs, const table_state &rhs)
{
    return lhs.current_stage == rhs.current_stage &&
           lhs.small_blind_size == rhs.small_blind_size &&
           lhs.big_blind_size == rhs.big_blind_size &&
           lhs.pot == rhs.pot &&
           lhs.total_contribution_to_stay_in_game == rhs.total_contribution_to_stay_in_game &&
           lhs.communal_cards == rhs.communal_cards &&
           lhs.players == rhs.players &&
           lhs.acting_player_pos == rhs.acting_player_pos &&
           lhs.dealer_pos == rhs.dealer_pos;
}

std::ostream& operator<<(std::ostream &os, const table_state &state)
{
    return os << ", current_stage: " << state.current_stage
              << ", small_blind_size: " << state.small_blind_size
              << ", big_blind_size: " << state.big_blind_size
              << ", pot: " << state.pot
              << ", total_contribution_to_stay_in_game: " << state.total_contribution_to_stay_in_game
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
