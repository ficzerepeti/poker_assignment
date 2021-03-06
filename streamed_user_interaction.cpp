#include <cctype>
#include <locale>
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

std::string streamed_user_interaction::get_user_pocket_cards()
{
    _os << "What pocket cards have you been dealt?\n";
    std::string cards;
    getline(_is, cards);
    return cards;
}

std::string streamed_user_interaction::get_player_pocket_cards(const size_t user_pos)
{
    _os << "What pocket cards were dealt to player at position:" << (user_pos + 1) << std::endl;
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

player_action_t streamed_user_interaction::get_user_action()
{
    return read_player_action();
}

player_action_t streamed_user_interaction::get_opponent_action()
{
    _os << "What did your opponent do? ";
    return read_player_action();
}

player_action_t streamed_user_interaction::read_player_action()
{
    _os << "Please specify an action: fold, check/call, raise <value>" << std::endl;

    std::string line;
    std::string action;

    while (getline(_is, line))
    {
        std::istringstream iss(line);

        iss >> action;
        for (auto& c : action)
        {
            c = std::tolower(c, std::locale());
        }

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
            iss >> amount;
            return player_action_raise{amount};
        }

        _os << "Invalid input. Please specify either fold, check, call or raise" << std::endl;
    }

    throw std::runtime_error("Cannot read user input");
}

void streamed_user_interaction::notify_player(const std::string &message)
{
    _os << message << std::endl;
}

} // end of namespace poker_lib
