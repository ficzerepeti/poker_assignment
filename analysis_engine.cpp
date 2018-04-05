#include "analysis_engine.h"

namespace poker_lib {

analysis_engine::analysis_engine(i_my_poker_lib &poker_lib, size_t num_of_players)
:
    _poker_lib(poker_lib),
    _players(num_of_players)
{
}

bool analysis_engine::set_board(const std::string &cards)
{
    if (_poker_lib.is_valid_card_str(cards))
    {
        _board = cards;
        return true;
    }
    return false;
}

bool analysis_engine::set_player_pocket_cards(size_t distance_from_dealer, const std::string &cards)
{
    if (_poker_lib.is_valid_card_str(cards))
    {
        _players.at(distance_from_dealer).pocket_cards = cards;
        return true;
    }
    return false;
}

void analysis_engine::set_player_folded(size_t distance_from_dealer)
{
    _players.at(distance_from_dealer).has_folded = true;
}

user_analysis analysis_engine::make_user_analysis(size_t distance_from_dealer)
{
    std::vector<std::string> hands;
    for (const auto &player : _players)
    {
        hands.emplace_back(player.pocket_cards);
    }

    const auto equity = _poker_lib.calculate_equities(hands, _board).at(distance_from_dealer);
    const auto recommended_action = player_action_check_or_call{}; // TODO

    return user_analysis{equity, recommended_action};
}

} // end of namespace poker_lib
