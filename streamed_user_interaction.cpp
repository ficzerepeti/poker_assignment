#include <string>
#include <sstream>
#include "streamed_user_interaction.h"

namespace poker_lib {

streamed_user_interaction::streamed_user_interaction(std::ostream &os, std::istream &is)
:
    _os(os),
    _is(is)
{
}

std::string streamed_user_interaction::get_pocket_cards()
{
    _os << "What pocket cards have you been dealt?\n";
    std::string cards;
    getline(_is, cards);
    return cards;
}

std::string streamed_user_interaction::get_flop()
{
    _os << "What is the flop?\n";
    std::string cards;
    getline(_is, cards);
    return cards;
}

std::string streamed_user_interaction::get_turn()
{
    _os << "What is the turn?\n";
    std::string card;
    getline(_is, card);
    return card;
}

std::string streamed_user_interaction::get_river()
{
    _os << "What is the river?\n";
    std::string card;
    getline(_is, card);
    return card;
}

player_action_t streamed_user_interaction::get_user_action(const player_action_t &recommended_action)
{
    _os << "Your recommended action is ";
    std::visit([&](const auto &obj){ _os << obj; }, recommended_action);
    _os << std::endl;

    return read_player_action();
}

player_action_t streamed_user_interaction::get_opponent_action()
{
    _os << "What did your opponent do?" << std::endl;
    return read_player_action();
}

player_action_t streamed_user_interaction::read_player_action()
{
    _os << "Please specify an action: fold, check, call, raise <value>" << std::endl;

    std::string action;

    while (true)
    {
        _is.clear();
        _is.ignore(std::numeric_limits<size_t>::max());

        _is >> action;

        if (action == "fold")
        {
            return player_action_fold{};
        }
        if (action == "check" || action == "call")
        {
            return player_action_check_or_call{};
        }
        if (action == "raise")
        {
            uint64_t amount = 0;
            _is >> amount;
            return player_action_raise{amount};
        }

        _os << "Invalid input. Please specify either fold, check, call or raise" << std::endl;
    }
}

void streamed_user_interaction::notify_player(const std::string &message)
{
    _os << message << std::endl;
}

} // end of namespace poker_lib
