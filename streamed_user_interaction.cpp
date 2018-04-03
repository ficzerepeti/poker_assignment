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

streamed_user_interaction::player_action_t streamed_user_interaction::get_user_action(size_t position,
                                                                                    const player_action_t &recommended_action)
{
    _os << "Your recommended action is ";
    std::visit([&](const auto &obj){ _os << obj; }, recommended_action);
    _os << ". Please specify your action (fold, check, call, raise <value>" << std::endl;

    return read_player_action();
}

streamed_user_interaction::player_action_t streamed_user_interaction::get_opponent_action(size_t position)
{
    return read_player_action();
}

i_user_interaction::player_action_t streamed_user_interaction::read_player_action()
{
    auto type = player_action_type::unknown;
    bool first_read = true;
    while (type == player_action_type::unknown)
    {
        if (!first_read)
        {
            _os << "Invalid input. Please specify either fold, check, call or raise <value>" << std::endl;
        }
        first_read = false;
        _is >> type;
    }
    switch (type)
    {
    case player_action_type::fold:
        return player_action_fold{};

    case player_action_type::check:
        return player_action_check{};

    case player_action_type::call:
    {
        uint64_t amount = 0;
        _is >> amount;
        return player_action_call{amount};
    }

    case player_action_type::raise:
    {
        uint64_t amount = 0;
        _is >> amount;
        return player_action_raise{amount};
    }

    case player_action_type::unknown:
        break;
    }

    throw std::logic_error("Reached unreachable code in get_user_action");
}

} // end of namespace poker_lib
