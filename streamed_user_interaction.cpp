#include <string>
#include "streamed_user_interaction.h"

enum class player_action_type
{
    fold,
    check,
    call,
    raise,
    unknown
};
std::istream& operator>>(std::istream& is, player_action_type& type)
{
    std::string str;
    is >> str;

    if (str == "fold") { type = player_action_type::fold; }
    else if (str == "check") { type = player_action_type::check; }
    else if (str == "call") { type = player_action_type::call; }
    else if (str == "raise") { type = player_action_type::raise; }
    else { type = player_action_type::unknown; }

    return is;
}

namespace poker_lib {

streamed_user_interaction::streamed_user_interaction(std::ostream &os, std::istream &is)
:
    _os(os),
    _is(is)
{
}

std::string streamed_user_interaction::get_pocket_cards()
{
    return "Ac Ad";
}

std::string streamed_user_interaction::get_flop()
{
    return "Ah As 3c";
}

std::string streamed_user_interaction::get_turn()
{
    return "4d";
}

std::string streamed_user_interaction::get_river()
{
    return "5h";
}

streamed_user_interaction::player_action_t streamed_user_interaction::get_user_action(size_t position,
                                                                                      const player_action_t &recommended_action)
{
    _os << "Your recommended action is ";
    std::visit([&](const auto &obj){ _os << obj; }, recommended_action);
    _os << std::endl;

    return read_player_action();
}

streamed_user_interaction::player_action_t streamed_user_interaction::get_opponent_action(size_t position)
{
    _os << "What did user at position " << position << " do?" << std::endl;
    return read_player_action();
}

i_user_interaction::player_action_t streamed_user_interaction::read_player_action()
{
    _os << "Please specify an action: fold, check, call, raise <value>" << std::endl;
    auto type = player_action_type::unknown;
    bool first_read = true;
    while (type == player_action_type::unknown)
    {
        if (!first_read)
        {
            _os << "Invalid input. Please specify either fold, check, call or raise" << std::endl;
        }
        first_read = false;
        _is >> type;
    }
    switch (type)
    {
    case player_action_type::fold:
        return player_action_fold{};

    case player_action_type::check:
    case player_action_type::call:
        return player_action_check_or_call{};

    case player_action_type::raise:
        return player_action_raise{read_amount()};

    case player_action_type::unknown:
        break;
    }

    throw std::logic_error("Reached unreachable code in get_user_action");
}

void streamed_user_interaction::notify_player(const std::string &message)
{
    _os << message << std::endl;
}

uint64_t streamed_user_interaction::read_amount()
{
    _os << "Please specify the amount" << std::endl;
    std::string str;

    while (true)
    {
        try
        {
            _is >> str;
            return std::stoull(str);
        }
        catch (const std::exception&)
        {
            _os << "Please enter a number, no other characters" << std::endl;
        }
    }
}

} // end of namespace poker_lib
